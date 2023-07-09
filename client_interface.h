#pragma once

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MESSAGE_LENGTH 1024 // Max buf size
#define PORT 8080

class client_interface
{
private:
	int socket_file_descriptor;
	int connection;
	struct sockaddr_in server_address;
	struct sockaddr_in client;
	char message[MESSAGE_LENGTH];
	bool is_connected_to_server = false;

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
			/*//delete when debugged to ***
			std::cout <<"CLIENT_S_ADDR: " <<  server_address.sin_addr.s_addr << std::endl;
			std::cout <<"CLIENT_SIN_PORT: " << server_address.sin_port << std::endl;
			std::cout << "CLIENT_SIN_FAMILY: " << server_address.sin_family << std::endl;
			//***
			*/

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

		while (true)
		{
			bzero(message, sizeof(message));
			std::cout << "Enter the message you want to send to the server: " << std::endl;
			std::cin >> message;
			
			if ((strncmp(message, "end", 3)) == 0)
			{
				write(socket_file_descriptor, message, sizeof(message));
				std::cout << "Client Exited. " << std::endl;
				break;
			}

			ssize_t bytes = write(socket_file_descriptor, message, sizeof(message));

			// if bytes >= 0 then success sending
			if (bytes >= 0)
			{
				std::cout << "Data send to the server successfully. " << std::endl;
			}

			bzero(message, sizeof(message));

			// Waiting for responce from server
			read(socket_file_descriptor, message, sizeof(message));
			
			std::cout << "Data from server: " << message << std::endl;
		}
		// Close socket, disconnect
		close(socket_file_descriptor);
	}
};


