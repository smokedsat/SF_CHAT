#pragma once 

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <map>
#include <vector>
#include "user.h"

#define MESSAGE_LENGTH 4096 // Max buf size
#define PORT 8080


std::string main_menu[3];
std::string user_menu[4];
std::string data_of_user[6];
std::string data_for_message[3]; // 1 - currentUser, 2 - toUser, 3 - message

ssize_t bytes = 0;


class client_interface
{
private:
	int socket_file_descriptor;
	int connection;
	struct sockaddr_in server_address;
	struct sockaddr_in client;
	char message[MESSAGE_LENGTH];
	bool is_connected_to_server = false;

private:
	void showMenu(User_interface & current_user, std::string & mess, std::string & command, std::string & username)
	{
		user_menu[0] = "/users";
		user_menu[1] = "/message";
		user_menu[2] = "/read";
		user_menu[3] = "/exit";

		std::string res;
		
		std::vector<std::string> users;
		std::vector<Message> messages;
		std::string _user;

		std::string from;
		std::string to;
		std::string date;
		std::string mess_from_user;
		Message current_message;
		std::string full_message;

		int choice = 0;
		bool compared = false;
		int k = 0;
		bzero(message, strlen(message));
		int slashes = 0;
		
		while(true)
		{
			compared = false;
			choice = 666;
			bytes = 0;

			for(auto it: data_for_message)
			{
				it.clear();
			}

			std::cout << "Welcome to chat, " << current_user.getName() << std::endl;
			mess.clear();

			while(!compared)
			{
				std::cout << "Please, choose what to do: " << std::endl
			<< "0. /users Show all available users to chat with." << std::endl
			<< "1. /message Send message to user." << std::endl
			<< "2. /read Read all messages with user. " << std::endl
			<< "3. /exit Exit." << std::endl;

				mess.clear();
				std::getline(std::cin, mess);

				for(size_t i = 0; i < 4; i++)
				{
					if(!strcmp(mess.c_str(), user_menu[i].c_str()))
					{
						choice = i;
						compared = true;
						break;
					}
				}

				if(compared == false)
				{
					std::cout << "Wrong choice. Message must be : /reg /login /exit"<< std::endl;
				}
			}
				bzero(message, strlen(message));
				strcpy(message, mess.c_str());

				//sleep(5); sleep for 5 seconds

				write(socket_file_descriptor, message, strlen(message));
				std::cout << "Successfully sent data to server: " << message << std::endl;

			switch (choice)
			{
				case 0:
					std::cout << "You picked 0. Show all users. Wait for responce from server: "<<std::endl;
					
					bytes = 0;
					while(bytes <= 0)
					{
						bzero(message, strlen(message));
						bytes = read(socket_file_descriptor, message,sizeof(message));
					}

					std::cout << "Server responce: " << message << std::endl;

					

					for(size_t it = 0; it < strlen(message); it++)
					{
						if(message[it] != ':')
						{
							_user += message[it];
							std::cout << _user << std::endl; 
						}
						if(message[it] == ':' || it == (strlen(message) -1))
						{	
							users.push_back(_user);
							std::cout << _user << std::endl;
							_user.clear();
						}						
					}

					std::cout << "There is all users of chat: " << std::endl;

					for(ssize_t it = 0; it < users.size(); it++)
					{
						std::cout << users.at(it) << std::endl;
					}

					choice = 666;
					break;

				case 1:
					k = 0;
					bytes = 0;
					username.clear();
					mess.clear();

					bzero(message, strlen(message));
					
					std::cout << "You picked 1. Send message to user. Please, write username, that must get your message: " << std::endl;
					std::getline(std::cin, username);

					std::cout << "Please write message: " << std::endl;
					std::getline(std::cin, mess);


					data_for_message[0] = current_user.getUsername();
					data_for_message[1] = username;
					data_for_message[2] = mess;

					std::cout <<"mess= " <<  data_for_message[2] << std::endl;

					for(size_t it = 0; it < 3; it++)
					{
						for(size_t jt = 0; jt < data_for_message[it].size(); jt++)
						{

							message[k++]= data_for_message[it].at(jt);
							if(jt == data_for_message[it].size() - 1 && it != 2)
							{
								message[k++] = ':';
							}
						}
					}

					
					std::cout << "Prepared data to send to server: " << message;

					write(socket_file_descriptor, message, strlen(message));
					std::cout << "Sent data to server: " << message << std::endl 
					<< "Waiting for responce from server: " << std::endl;

					bytes = 0;

					while(bytes <= 0)
					{
						bzero(message, strlen(message));
						bytes = read(socket_file_descriptor, message, sizeof(message));
					}

					std::cout << "Message from server: " << message << std::endl;

					choice = 666;

					data_for_message[0].clear();
					data_for_message[1].clear();
					data_for_message[2].clear();
					break;

				case 2:
					bytes = 0;
					bzero(message, strlen(message));
					std::cout << "You picked 2 /read . Please, write username, with who you would like to read messages. " << std::endl;
					std::getline(std::cin, username);

					
					strcpy(message, username.c_str());

					mess.clear();

					write(socket_file_descriptor, message, strlen(message));
					

					bytes = 0;

					while(bytes <= 0)
					{
						bzero(message, strlen(message));
						bytes = read(socket_file_descriptor, message, sizeof(message));
					}

					// Realise rule, that will show correct messages from users.
					std::cout << "Received message from server: " << message << std::endl;

					slashes = 0;
					messages.clear();
					full_message.clear();
					to.clear();
					from.clear();
					date.clear();
					mess_from_user.clear();
					current_message.clear();

					for(ssize_t it = 0; it < strlen(message); it++)
					{
						if(message[it] == '/')
						{
							slashes++;
							it++;
						}
						
						switch (slashes)
						{
						case 0:
							date += message[it];
							break;
						case 1:
							continue;
							break;
						case 2:
							from += message[it];
							break;
						case 3:
							continue;
							break;
						case 4:
							to+= message[it];
							break;
						case 5:
							continue;
							break;
						case 6:
							mess_from_user+= message[it];
							break;
						case 7:
							slashes = 0;
							current_message.clear();
							current_message.setMessage(from,to,mess_from_user, date);
							messages.push_back(current_message);
							mess_from_user.clear();
							date.clear();
							from.clear();
							to.clear();
							break;
						default:
							break;
						}
					}

					std::cout << "You got messages:" << std::endl;

					for(auto it : messages)
					{
						std::cout << "Date: " << it._datetime << " From: " << it._from << " To: " 
						<< it._to << "\n" << " Message: " << it._data << std::endl; 
					}

					choice = 666;
					break;

				case 3:
					choice = 666;
					return;
					break;

				default:
					choice = 666;
					break;
			}

		}
	}
public:
	client_interface()
	{
		socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

		if (socket_file_descriptor == -1)
		{
			std::cout << "ERROR: Creation of socket failed." << std::endl;
			exit(1);
		}

		// Setup server address
		server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
		
		// Setup port num
		server_address.sin_port = htons(PORT);

		// Using IPv4
		server_address.sin_family = AF_INET;

		// Setup connection to the server
		connection = connect(socket_file_descriptor, (struct sockaddr*)&server_address,
			sizeof(server_address));

		if (connection == -1)
		{
			std::cout << "ERROR: Client cannot connect to the server." << std::endl;
			exit(1);
		}
		else
		{
			is_connected_to_server = true;
		}
	}

	void start()
	{
		// Interaction with server
		if (!is_connected_to_server)
		{
			std::cout << "ERROR: Cant interact with the server, its not connected." << std::endl;
			return;
		}
		
		User_interface curr_user;
		std::string _message_fake;

		main_menu[0] = "/reg";
		main_menu[1] = "/login";
		main_menu[2] = "/exit";

		bool comp = false;
		size_t choice = 0;
		std::string mess;
		int k = 0; 

		while (true)
		{
			comp = false;
			bzero(message, strlen(message));

			while(!comp)
			{
				std::cout 	<< "Please choose from menu: " << std::endl
							<< "0. Registration: /reg" << std::endl
							<< "1. Login: /login " << std::endl
							<< "2. Exit: /exit " << std::endl;
							
				mess.clear();
				std::getline(std::cin, mess);
				strcpy(message, mess.c_str());
				choice = 666;

				for(size_t i = 0; i < 3; i++)
				{
					if(!strcmp(message, main_menu[i].c_str()))
					{
						choice = i;
						comp = true;
						break;
					}
				}
				if(!comp)
				{
					std::cout << "Wrong choice. Message must be : /reg /login /exit"<< std::endl;
				}
			}

			write(socket_file_descriptor, message, strlen(message));
			std::cout << "Sent message to server: " << message << std::endl;
			//read(socket_file_descriptor, message, sizeof(message));
			
			switch (choice)
			{
				case 0:
					bzero(message, strlen(message));
					// Read info message from server
					//read(socket_file_descriptor, message, sizeof(message));
					std::cout << "You choised Registration.\n You need to send some infermation about yourself :"<<std::endl;
					data_of_user[0].clear();
					std::cout << "\nPlease enter your name: " << std::endl;
					std::getline(std::cin, data_of_user[0]);

					data_of_user[1].clear();
					std::cout << "Please enter your surname: " << std::endl;
					std::getline(std::cin, data_of_user[1]);

					data_of_user[2].clear();
					std::cout << "Please enter your phone: " << std::endl;
					std::getline(std::cin, data_of_user[2]);
					
					data_of_user[3].clear();
					std::cout << "Please enter your password: " << std::endl;
					std::getline(std::cin, data_of_user[3]);

					data_of_user[4].clear();
					std::cout << "Please enter your username: " << std::endl;
					std::getline(std::cin, data_of_user[4]);

					data_of_user[5].clear();
					std::cout << "Please enter your email: " << std::endl;
					std::getline(std::cin, data_of_user[5]);
					
					
					for(size_t it = 0; it < 6; it++)
					{
						for(size_t jt = 0; jt < data_of_user[it].size(); jt++)
						{
							message[k++]= data_of_user[it].at(jt);
							if(jt == data_of_user[it].size() - 1)
							{
								message[k++] = ':';
							}
						}
					}
					std::cout << "data: " << message<< std::endl;

					write(socket_file_descriptor, message, strlen(message));

					std::cout << "LOG: sent data to server: " << message << std::endl;

					bytes = 0;
					while(bytes <= 0 )
					{
						bzero(message, strlen(message));
						bytes = read(socket_file_descriptor, message, sizeof(message));
					}
					std::cout << "Received message from server: " << message;
					break;

				case 1:
					
					bzero(message,strlen(message));

					std::cout << "You choosed Login. Now you need to write your username and password. " << std::endl;
					std::cout << "Please write your username, that you wrote at Registration: " << std::endl;
					data_of_user[4].clear();
					std::getline(std::cin, data_of_user[4]);
					
					std::cout << "Please write your password, that you wrote at Registration: " << std::endl;
					data_of_user[3].clear();
					std::getline(std::cin, data_of_user[3]);

					for(size_t it = 3; it < 5; it++)
					{
						for(size_t jt = 0; jt < data_of_user[it].size(); jt++)
						{
							message[k++]= data_of_user[it].at(jt);
							if(jt == data_of_user[it].size() - 1)
							{
								message[k++] = ':';
							}
						}
					}
					


					write(socket_file_descriptor, message, strlen(message));
					
					std::cout << "Sent message to server: " <<  message << std::endl;
					std::cout << "\n Please wait, while server reply: " << std::endl;

					bytes = 0;
					while(bytes <= 0)
					{
						bzero(message, strlen(message));
						bytes = read(socket_file_descriptor, message, sizeof(message));
					}
					
					std::cout << message;

					curr_user.setUsername(data_of_user[4]);
					curr_user.setPassword(data_of_user[3]);

					
					showMenu(curr_user,_message_fake, data_of_user[3], data_of_user[3]);
					
					break;

				case 2:
					return;
					break;
				
				default:

					break;
					
			}
			
		}
		// Close socket, disconnect
		close(socket_file_descriptor);
	}
};