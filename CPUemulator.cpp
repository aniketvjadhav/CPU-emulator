#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>

using namespace std;

void error_exit()
{
   cout<<"Error occurred"<<endl;
      exit(1);
}

int main()
{
  int c2p[2]; //child to parent pipe
	int p2c[2]; //parent to child pipe

	int i=0;
	pid_t chpid; // processor id variables

	string line, count;
	string ram[1000]; //memory of 1000 locations

	char buf[10]; // buffer for reading pipes


	//processor variables
	int pc=0, ir, sp=999,ac=0,x=0,y=0;
	
	
	ifstream myfile ("program.txt");

	if ((pipe(p2c))== -1 || (pipe(c2p))== -1 )
	  exit(1);



	switch(fork())
	{
		case -1: //error occured in forking.

					exit(1);
					

		case 0: //child process

				
					if (myfile.is_open())
					{
						while ( myfile.good() )
						{
							getline (myfile,line);
							ram[i] = line;
							i++;
						}
					myfile.close();
					}
					else cout << "Unable to open file";


					
					
					while(read(p2c[0], buf, 10))
					{
						
						// temp1 will tell whether processor has demanded read or write instruction.
						char temp1 = buf[0];	
						

						if(temp1 == '1')//this condition checks whether read or write. 1 when read
						{
							char temp2[4]; // program counter will come in temp2
							memcpy( temp2, &buf[1], 4 );
							if(write(c2p[1], (void*)ram[atoi(temp2)].c_str(), 10)==-1)
							error_exit();
							
							
						}
						else
						{
							char temp2[4]; // program counter will come in temp2
							memcpy( temp2, &buf[1], 4 );
							int address = atoi(temp2);
							write(c2p[1], "1", 10);	//demo write to maintain sync
							read(p2c[0], buf, 10); //reads value in AC

							//stores value in AC at address specified.

							ram[address] = buf;
						
						}
					}
					
					_exit(0);
		default:
					while(1)
					{
						
						stringstream ss;						

						ss.str("");
						ss << 1;
						ss << pc;
						count = ss.str();

						write(p2c[1], (void*)count.c_str(), 10);

						read(c2p[0], buf, 10);
						
						ir=atoi(buf);

						switch(ir)
						{

							case 1: //Load the value into the AC
									
									pc++;
									
									ss.str("");

									ss << 1;
									ss << pc;
									count = ss.str();
									
									//cout<<count;
									write(p2c[1], (void*)count.c_str(), 10);
									read(c2p[0], buf, 10);
									
									ac = atoi(buf);
									
									//cout<<ac<<endl;
									break;
							
							case 2: // Load the value at the address into the AC
									
									pc++;
									ss.str("");
									ss << 1;
									ss << pc;
									count = ss.str();
									
									write(p2c[1], (void*)count.c_str(), 10);									
									read(c2p[0], buf, 10);

									ac = atoi(buf);

									ss.str("");
									ss << 1;
									ss << ac;
									count = ss.str();

									write(p2c[1], (void*)count.c_str(), 10);									
									read(c2p[0], buf, 10);

									ac = atoi(buf);
									//cout<<ac<<endl;
									
									break;

							case 3:	//Store the value in the AC into the address
									
									pc++;

									ss.str("");
									ss << 1;
									ss << pc;
									count = ss.str();

									write(p2c[1], (void*)count.c_str(), 10);
									
									read(c2p[0], buf, 10); // reads the address where AC needs to be copied
									
									ss.str("");
									ss << 2;
									ss << atoi(buf); // address at which value should be copied.									
									count = ss.str();
									
									write(p2c[1], (void*)count.c_str(), 10);
									read(c2p[0], buf, 10); // waiting for memory to copy address
									
									ss.str("");
									ss << ac;//value to be copied
									count = ss.str();
									
									write(p2c[1], (void*)count.c_str(), 10);

									break;

							case 4: //Add the value in X to the AC
									
									ac = ac + x;
									break;
							
							case 5: //Add the value in Y to the AC
									
									ac = ac + y;
									break;
							
							case 6: //Subtract the value in X from the AC
									
									ac = ac - x;
									break;

							case 7: //Subtract the value in Y from the AC
									
									ac = ac - y;
									break;

							case 8: //Get port

									pc++;

									ss.str("");
									ss << 1;
									ss << pc;
									count = ss.str();

									write(p2c[1], (void*)count.c_str(), 10);								
									read(c2p[0], buf, 10); // read the port number

									if (buf[0] == '1')
									{
										cout<<"Enter one value - ";
										cin>>ac;
									}
									else
									{
										cout<<"Enter a character - ";
										char temp;
										fflush(NULL);
										cin>>temp;
										ac = temp;

									}

									break;

							case 9: // Put Port

									pc++;

									ss.str("");
									ss << 1;
									ss << pc;
									count = ss.str();

									write(p2c[1], (void*)count.c_str(), 10);								
									read(c2p[0], buf, 10); // read the port number

									if (buf[0] == '1')
									{
										cout<<ac<<" ";
									}
									else
									{
										cout<<(char)ac<<" ";
									}

									break;

							case 10: // Copy the value of AC to x 

									x = ac;
									break;
							
							case 11: // Copy the value of AC to y

									y = ac;
									break;

							case 12: // Copy the value of x to AC
									
									ac = x;
									break;

							case 13: // Copy the value of y to AC

									ac = y;
									break;

							case 14: // Jump to address

									pc++; // to get the value of the jump address

									ss.str("");
									ss << 1;
									ss << pc;
									count = ss.str();

									write(p2c[1], (void*)count.c_str(), 10);								
									read(c2p[0], buf, 10); // gets the address

									pc = atoi(buf);
									pc--;

									break;

							case 15: // Jump to address if the value in AC is zero
									
									pc++; // to get the value of the jump address
									if (ac == 0)
									{
										
										
										ss.str("");
										ss << 1;
										ss << pc;
										count = ss.str();

										write(p2c[1], (void*)count.c_str(), 10);								
										read(c2p[0], buf, 10); // gets the address

										pc = atoi(buf);
										pc--;
										
									}

									break;

							case 16: // Jump to address if the value in AC is not equal to zero
									
									pc++; // to get the value of the jump address
									
									if (ac != 0)
									{
										
										
										ss.str("");
										ss << 1;
										ss << pc;
										count = ss.str();

										write(p2c[1], (void*)count.c_str(), 10);								
										read(c2p[0], buf, 10); // gets the address

										pc = atoi(buf);

										
									}

									break;

							case 17: // Push return address on to the stack and jump on the stack.
									
									//Putting AC on stack
								{

									ss.str("");
									ss << 2;
									ss << sp; // address at which value should be copied.									
									count = ss.str();
									
									write(p2c[1], (void*)count.c_str(), 10);
									read(c2p[0], buf, 10); // waiting for memory to copy address
									
									ss.str("");
									ss << ac;//value to be copied
									count = ss.str();									
									write(p2c[1], (void*)count.c_str(), 10);
									
									sp--;

									//Putting IR on stack

									ss.str("");
									ss << 2;
									ss << sp; // address at which value should be copied.									
									count = ss.str();
									
									write(p2c[1], (void*)count.c_str(), 10);
									read(c2p[0], buf, 10); // waiting for memory to copy address
									
									ss.str("");
									ss << ir;//value to be copied
									count = ss.str();									
									write(p2c[1], (void*)count.c_str(), 10);
									
									sp--;

									//Putting X on stack

									ss.str("");
									ss << 2;
									ss << sp; // address at which value should be copied.									
									count = ss.str();
									
									write(p2c[1], (void*)count.c_str(), 10);
									read(c2p[0], buf, 10); // waiting for memory to copy address
									
									ss.str("");
									ss << x;//value to be copied
									count = ss.str();									
									write(p2c[1], (void*)count.c_str(), 10);
									
									sp--;

									//Putting Y on stack

									ss.str("");
									ss << 2;
									ss << sp; // address at which value should be copied.									
									count = ss.str();
									
									write(p2c[1], (void*)count.c_str(), 10);
									read(c2p[0], buf, 10); // waiting for memory to copy address
									
									ss.str("");
									ss << y;//value to be copied
									count = ss.str();									
									write(p2c[1], (void*)count.c_str(), 10);
									
									sp--;

									//Putting PC on stack

									//first get the jump address

									pc++; // to get the value of the jump address
										
									ss.str("");
									ss << 1;
									ss << pc;
									count = ss.str();

									write(p2c[1], (void*)count.c_str(), 10);								
									read(c2p[0], buf, 10); // gets the address

									int tempPC = atoi(buf); //storing PC temporarily 
									
									pc++; // next address should be stored on the stack

									ss.str("");
									ss << 2;
									ss << sp; // address at which value should be copied.									
									count = ss.str();
									
									write(p2c[1], (void*)count.c_str(), 10);
									read(c2p[0], buf, 10); // waiting for memory to copy address
									
									ss.str("");
									ss << pc;//value to be copied
									count = ss.str();									
									write(p2c[1], (void*)count.c_str(), 10); //pushing the value to stack
									
									sp--;

									// now it will jump to the address
									
									pc = tempPC;
									pc--; //as PC will be incremented after this switch, it is decremented here.
									//cout<<sp<<endl<<x<<endl<<y<<endl<<ac<<endl<<ir<<endl;
								
									break;
								}

							case 18: // Pops return address from the stack and jumps to the address.

									//Getting the value of PC

									sp++; //making stack pointer to point PC

									ss.str("");
									ss << 1;
									ss << sp;
									count = ss.str();

									write(p2c[1], (void*)count.c_str(), 10);								
									read(c2p[0], buf, 10); // gets the PC

									pc = atoi(buf);
									pc--;

									// Getting value of Y

									sp++;

									ss.str("");
									ss << 1;
									ss << sp;
									count = ss.str();

									write(p2c[1], (void*)count.c_str(), 10);								
									read(c2p[0], buf, 10); // gets the Y

									y = atoi(buf);

									// Getting value of X

									sp++;

									ss.str("");
									ss << 1;
									ss << sp;
									count = ss.str();

									write(p2c[1], (void*)count.c_str(), 10);								
									read(c2p[0], buf, 10); // gets the X

									x = atoi(buf);

									// Getting value of IR

									sp++;

									ss.str("");
									ss << 1;
									ss << sp;
									count = ss.str();

									write(p2c[1], (void*)count.c_str(), 10);								
									read(c2p[0], buf, 10); // gets the IR

									ir = atoi(buf);

									// Getting value of AC

									sp++;

									ss.str("");
									ss << 1;
									ss << sp;
									count = ss.str();

									write(p2c[1], (void*)count.c_str(), 10);								
									read(c2p[0], buf, 10); // gets the AC

									ac = atoi(buf);

									break;

							case 19: // increment x

									x++;

									break;

							case 20: // decrement x
									
									x--;

									break;

							case 30:
									
									//cout<<"parent exiting"<<endl;

									chpid = getpid();
									kill(chpid,0);
									_exit(1);
									break;

						}
						pc++;
					}
					waitpid(-1, NULL, 0);
	}

}
