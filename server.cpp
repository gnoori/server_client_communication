/*Guzel Noori - CS 470
Lab 4 - Server <-> Client Burger Communication
Date: 3/3/2021*/

//libraries
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <cstring>
#include <thread>
#include <chrono>
#include <vector>
#include <ctime>
#include <pthread.h>

using namespace std;

vector<thread> chefs;
struct sockaddr_in serv_addr;

//default port
const int port = 1488;
// in seconds
const int mTime = 60;
const int mxTime = 100;
const int midTime = 130;
const int clients = 30;

//socket message
const char* message = "Serving";

//default number of chefs
int numChefs = 2;

int chefPointer = 0;
int temp = 0;
int customer = -1;

//default max number of burgers
int maxBurgers = 25;
int burgersEaten = 0;
int totalB = 0;
int rem = maxBurgers;
int opt = 1;

//socket
int sockfd, clientSocket[clients];

bool burgLine = true;
bool burgLineLimit = false;

bool have();
void maxB(int burgers);
bool addB();
bool eatB();
void nChef();

void maxB(int burgers)
{
	rem = burgers;
	maxBurgers = burgers;
}

bool have()
{
	return rem || !burgLine;
}

bool addB()
{
	if (!rem)
	{
		return false;
	}
	while (burgLineLimit)
	{
		this_thread::sleep_for(chrono::seconds(1));
	}
	
	chefPointer = (chefPointer + 1) % maxBurgers;
	rem--;
	cout << rem << " more burgers should be prepared." <<  endl;
	if (chefPointer == customer)
	{
		burgLineLimit = true;
	}
	else
	{
		burgLineLimit = false;
	}
	burgLine = false;
	return true;
}

bool eatB()
{
	while (burgLine)
	{
		if (!rem)
		{
			return false;
		}
		
		this_thread::sleep_for(chrono::seconds(1));
	}
	
	customer = (customer + 1) % maxBurgers;
	if (customer == chefPointer - 1)
	{
		burgLine = true;
	}
	else
	{
		burgLine = false;
	}

	burgLineLimit = false;
	burgersEaten++;

	cout <<"Burger # " << burgersEaten << "." << endl;

	return true;
}

void Chef(int chefN)
{
	while (addB())
	{
		srand(time(NULL));
		int seconds = mTime + (rand() % (mxTime - mTime));
		this_thread::sleep_for(chrono::seconds(1));
		cout << "Chef " << (chefN+1) << " prepared #" << ++totalB << " burger in " << seconds << " seconds \n" << endl;
	}
}
void nChef(int inNumChefs)
{
	for (int i = 0; i < inNumChefs; ++i)
	{
		chefs.push_back(thread(&Chef, i));
		this_thread::sleep_for(chrono::seconds(1));
	}
}

int main(int argc, char* argv[])
{

	if (argc==1){
		cout << "Using default Port Number -- > 1488"<<endl;
		cout << "Using default Max Burgers --> 25\n"<<endl;
	}
	
	//https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
	int command;
	while ((command = getopt (argc, argv, "b:c:")) != -1)
	{
		switch (command)
		{
			case 'b':
				temp = atoi(optarg);
				if (temp != 0)
				{
					maxBurgers = temp;
					cout << "Max Burgers: " << maxBurgers << endl;
				}
				else
				{
					cout << "Incorrect # of burgers" << endl;
					cout << "Usage: ./server -b <MaxNumberOfBurgers> -c <NumberOfChefs>" << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case 'c':
				temp = atoi(optarg);
				if (temp != 0)
				{
					numChefs = temp;
					cout << "Number of Chefs: " << numChefs << endl;
				}
				else
				{
					cout << "Incorrect # of chefs" << endl;
					cout << "Usage: ./server -b <MaxNumberOfBurgers> -c <NumberOfChefs>" << endl;
					exit(EXIT_FAILURE);
				}
				break;
		}
	}
	
	maxB(maxBurgers);
	nChef(numChefs);
	
	char buffer[1024] = {0};

	for (int i = 0; i < clients; i++)
	{
		clientSocket[i] = 0;
	}
	sockfd = socket(AF_INET , SOCK_STREAM , 0);
	if (sockfd == 0 || setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		cout << "\nError : Could not create socket" << endl;
		exit(EXIT_FAILURE);
	}

	cout << "\nSocket Connection is established." << endl;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	cout << "\nLooking for clients. \n"<<endl;

	if ((bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) ||(listen(sockfd, 3) < 0))
	{
		cout << "Can not bind sockets" << endl;;
		exit(EXIT_FAILURE);
	}

	cout << "Socket binding is successful. \n"<<endl;

	int sizelen = sizeof(serv_addr);
	
	while (have())
	{
		fd_set readfds;
		FD_ZERO(&readfds);
		
		FD_SET(sockfd, &readfds);
		int max_sd = sockfd;
		
		for (int i = 0 ; i < clients ; i++)
		{
			if(clientSocket[i] > 0)
			{
				FD_SET(clientSocket[i] , &readfds);
				if(clientSocket[i] > max_sd)
				{
					max_sd = clientSocket[i];
				}
			}
		}
		
		int activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);
		if ((activity < 0) && (errno != EINTR))
		{
			cout << "Can not select" << endl;
		}
		
		if (FD_ISSET(sockfd, &readfds))
		{
			int newSocket;
			if ((newSocket = accept(sockfd, (struct sockaddr *)&serv_addr, (socklen_t*)&sizelen)) < 0)
			{
				cout << "Can not accept a client" << endl;
				exit(EXIT_FAILURE);
			}

			cout << "\nConnection is successful. "<< endl;
			cout << "Press Ctr+C to end the connection. \n"<< endl;
			
			for (int i = 0; i < clients; i++)
			{
				if(clientSocket[i] == 0)
				{
					clientSocket[i] = newSocket;
					break;
				}
			}
		}

		for (int i = 0; i < clients; i++)
		{
			if (FD_ISSET(clientSocket[i] , &readfds))
			{
				int numCharsReceived;
				if ((numCharsReceived = (int)read(clientSocket[i] , buffer, 1024)) == 0)
				{
					close(clientSocket[i]);
					clientSocket[i] = 0;
				}
				else
				{
					buffer[numCharsReceived] = '\0';
					if(eatB())
					{
						cout << "Served..." << endl;
						send(clientSocket[i] , message, strlen(message), 0);
                        cout << "Eaten..." << endl;
					}
				}
			}
		}
	}
	return 0;
}
