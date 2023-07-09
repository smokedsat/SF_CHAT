#pragma once

#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include <sstream>

#include "mysql_interface.h"

#define MESSAGE_LENGTH 4096 // Max length of data buffer
#define PORT 8080


class server_interface
{
private:
    struct sockaddr_in server_address;
    struct sockaddr_in client;
    
    socklen_t socket_length;
    int socket_file_descriptor;
    int bind_status;
    int listen_status;

    bool _isConnected = false;
    
    mysql database;

    User_interface current_user;

public:
        // TODO: Добавить возможность обрабатывать сообщения от пользователя 
        //пользователю, отправлять список зарегистрированных пользователей, 
        //отправлять все смс, что есть в личном чате, если есть

    void wait_for_user_menu(int connection)
    {
        ssize_t bytes = 0;
        char message[MESSAGE_LENGTH];
        std::cout << "Waiting for user responce. Current user is " << current_user.getUsername() << std::endl;
        size_t choice = 999;
        std::string menu[4] = {"/users", "/message", "/read", "/exit"};
        std::string mess_str;
        std::string full_message;
        std::vector<Message> messages;
        std::vector<std::string> users_vector;
        std::string data_for_message[3];
        
        std::string responce;
        Message mess;
        std::string user;
        
        int sizeOfVector = 0;

        int k = 0;
        while(choice != 3)
        {
            bzero(message, strlen(message));

            std::cout << "Waiting for choice from client:" << std::endl;

            bytes = 0;

            while(bytes <= 0)
                {
                    // попробовать изменить sizeof(message) и strlen на MESSAGE_LENGTH.
                    bzero(message, sizeof(message));

                    bytes = read(connection, message, sizeof(message));

                    ////////////////////////////////
                }
            std::cout << "Comparing user choice from user menu. " << std::endl;
            
            for(size_t i = 0; i < 4; i++)
				{
					if(!strcmp(message, menu[i].c_str()))
					{
						choice = i;
						break;
					}
				}
				
                switch (choice)
                {
                    case 0: 
                        bytes = 0;
                        std::cout << "Client choosed /users" << std::endl;
                        users_vector = database.getUsersFromDatabase();
                        
                        
                        for(auto i = 0; i < users_vector.size(); i++)
                        {
                            for(auto j = 0; j < users_vector.at(i).size(); j++)
                            {
                                sizeOfVector++;
                            }
                        }


                        bzero(message, sizeof(message));

                        for(auto i =0 ; i < sizeOfVector;)
                        {
                            if(i >= 4095)
                            {
                                break;
                            }

                            for(auto it : users_vector)
                            {
                                it.push_back(':');
                                for(auto j = 0; j < it.size(); j++)
                                {
                                    message[i++] = it.at(j);
                                }
                            }
                        }

                        for(auto user : users_vector)
                        {
                            responce += (user + ":");
                        }
                        
                        
                        if(!responce.empty())
                        {
                            responce.pop_back();
                        }
                        
                        /*std::cout << "Vector of users is : " << std::endl;
                        for(auto it : users)
                        {
                            std::cout << it << std::endl;
                        }*/
                        bzero(message, sizeof(message));
                        std::cout << "\nresponce string is: " << responce << std::endl;
                        strcpy(message, responce.c_str());

                        std::cout << "\nSent message to client: " << responce << std::endl;

                        write(connection, message, sizeof(message));
                        
                        break;

                    case 1:
                        
                        std::cout << "Client picked /message. " << std::endl;
                        std::cout << "Waiting for data from client..." << std::endl;
                        
                        bytes = 0;

                        while (bytes <= 0)
                        {
                            bzero(message, sizeof(message));
                            bytes = read(connection, message, strlen(message));
                            std::cout << "connection:" << connection << std::endl <<"message: " <<
                                 message << std:: endl << "strlen: " << strlen(message) << std::endl << "sizeof mess: " << 
                                 sizeof(message) << std::endl; 
                        }
                        
                        std::cout << "Received data from client: " << message << std::endl;

                        for(size_t jt = 0; jt < 3 ; jt++)
                                {
                                        for(size_t it = 0; it < bytes; it++)
                                        {
                                            if((message[it] == ':') &&  jt == 2)
                                            {
                                                jt++;
                                                break;
                                            }
                                            if(message[it] == ':'|| message[it] == '\0')
                                            {
                                                jt++; 
                                            }
                                            if(message[it] != ':')
                                            {
                                                data_for_message[jt] += message[it];
                                            }   
                                        }
                                }

                        bzero(message, sizeof(message));

                        mess.setMessage(data_for_message[0], data_for_message[1],data_for_message[2]);
                        
                        database.insertMessageToDatabase(mess);
                        
                        responce += "Successfully sent message to database " + data_for_message[0]+ ":" + data_for_message[1]+ ":" + data_for_message[2];
                        for(auto i = 0; i < responce.size(); i++)
                        {
                            message[i] = responce.at(i);
                        }

                        write(connection, message, strlen(message));

                        std::cout << "Sent message to client: " << message << std::endl;

                        break;


                    case 2:
                        bytes = 0;

                        while(bytes <= 0)
                        {
                            bzero(message, sizeof(message));
                            bytes = read(connection, message, sizeof(message));
                        }
                        
                        mess_str.clear();
                        for(auto i = 0; i < strlen(message);i++)
                        {
                            mess_str += message[i];
                        }

                        messages.clear();

                        messages = database.getMessagesFromUToU(current_user.getUsername(), mess_str);

                        
                        for(auto it : messages)
                        {
                            mess_str.clear();
                            for(auto i = 0; i < 5; i++)
                            {
                                mess_str += it._datetime + ":from:" + it._from + ":to:" + it._to + ":message:" + it._data;
                            }
                            
                            full_message += mess_str + ":";
                            
                        }

                        full_message.pop_back();
                        if(full_message.size() >= 4096)
                        {
                            full_message.resize(4095);
                        }
                        
                        bzero(message, sizeof(message));
                        strcpy(message, full_message.c_str());

                        write(connection, message, strlen(message));

                        std::cout << "Sent message to client: " << message << std::endl; 

                        break;

                    case 3:


                        break;

                }
        }
    }





public:
    bool isConnected() { return _isConnected; }

    server_interface()
    {
        database.connect();

        database.mydatabase.create_table("messages", "id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, username_from VARCHAR(10), username_to VARCHAR(10), data VARCHAR(1024),date DATETIME DEFAULT NOW()");
        database.mydatabase.create_table("users", "id integer primary key auto_increment, name varchar(255), surname varchar(255), phone varchar(255), password varchar(255), username varchar(10) unique, email varchar(255)");

        // Create the socket
        socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

        if (socket_file_descriptor == -1) {
            std::cout << "ERROR: SOCKET CREATION FAILED." << std::endl;
            return;
        }

        // Initialize the server address
        // bzero(&server_address, sizeof(server_address));

        // Initialize the server address
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
        server_address.sin_port = htons(PORT);

        

        // Bind the socket to the server address
        bind_status = bind(socket_file_descriptor, (struct sockaddr*)&server_address, sizeof(server_address));
        if (bind_status == -1) {
            std::cout << "ERROR: SOCKET BINDING FAILED." << std::endl;
            return;
        }

        // Start listening for incoming connections
        listen_status = listen(socket_file_descriptor, 5);
        if (listen_status == -1) {
            std::cout << "ERROR: SOCKET IS UNABLE TO LISTEN FOR NEW CONNECTIONS." << std::endl;
            return;
        }
        else {
            _isConnected = true;
            std::cout << "SERVER IS LISTENING FOR NEW CONNECTIONS: " << std::endl;
            /*
            std::cout << "SERVER_S_ADDR: " << server_address.sin_addr.s_addr << std::endl;
            std::cout << "SERVER_SIN_FAMILY: " << server_address.sin_family << std::endl;
            std::cout << "SERVER_SIN_PORT: " << server_address.sin_port << std::endl;
            */
        }
    }

    ~server_interface()
    {
        _isConnected = false;
        database.disconnectFromMySQL();
        close(socket_file_descriptor);
    }

    

    void run()
    {
        if(!database.isDbConnected)
        {
            database.connect();
        }

        while (true) {
            socket_length = sizeof(client);
            int connection = accept(socket_file_descriptor, (struct sockaddr*)&client, &socket_length);

            if (connection == -1) {         
                std::cout << "ERROR: SERVER IS UNABLE TO ACCEPT THE DATA FROM CLIENT." << std::endl;
                continue;
            }


                std::string mess;
                ssize_t bytes = 0;
                std::string menu[3];
                menu[0] = "/reg";
                menu[1] = "/login";
                menu[2] = "/exit";
                size_t choice = 0;

                char message[MESSAGE_LENGTH];

                //  
                    std::vector<std::string> users_vector;
                    int sizeOfVector = 0;
                    std::string responce;
                    //
                std::string _mess;
                std::string name{""},surname{""},phone{""}, password{""}, username{""}, email{""}; 
                std::string data[6];
                User_interface user;
                User user_menu(user);

                std::vector<std::string> users_from_db;

                bool readed = false;

                while (true) {
                    
                    bzero(message, MESSAGE_LENGTH);
                    int k = 0;
                    bytes = 0;

                    while (bytes <= 0)
                    {
                        bzero(message, MESSAGE_LENGTH);
                        bytes = read(connection, message, sizeof(message));

                          std::cout << "connection:" << connection << std::endl <<"message: " <<
                                 message << std:: endl << "strlen: " << strlen(message) << std::endl << "sizeof mess: " 
                                 << sizeof(message) << std::endl; 
                        
                        std::cout << "message readed: " << message << std::endl;
                        
                        for (size_t it = 0; it < 3; it++)
                        {
                            if (!strcmp(message, menu[it].c_str()))
                            {
                                choice = it;
                                readed = true;
                                break;
                            }
                        }
                        
                        std::cout << "readed = " << message << std::endl;
                    } 

                    std::cout << "choice = " << choice << std::endl;
                    
                    

                    while(choice != 2)
                    {
                        switch (choice)
                        {
                            case 0: // REGISTRATION
                                bytes = 0;
                                choice = 2;

                                while(bytes <= 0)
                                {
                                        bytes = read(connection, message,sizeof(message));
                                }
                                std::cout << "LOG: received data from client: \n " << message << std::endl;
                                // PARSING THE DATA FROM CLIENT
                                for(size_t jt = 0; jt < 6 ; jt++)
                                {
                                        for(size_t it = 0; it < bytes; it++)
                                        {
                                            if((message[it] == ':') &&  jt == 5)
                                            {
                                                jt++;
                                                break;
                                            }
                                            if(message[it] == ':'|| message[it] == '\0')
                                            {
                                                jt++; 
                                            }
                                            if(message[it] != ':')
                                            {
                                                data[jt] += message[it];
                                            }   
                                        }
                                }
                                bzero(message,sizeof(message));

                                std::cout << "name: " << data[0] << "\n";
                                std::cout << "surname: " << data[1] << "\n";
                                std::cout << "phone: " << data[2] << "\n";
                                std::cout << "password: " << data[3] << "\n";
                                std::cout << "username: " << data[4] << "\n";
                                std::cout << "email: " << data[5] << "\n";
                                
                                user.setName(data[0]);
                                user.setSurname(data[1]);
                                user.setPhone(data[2]);
                                user.setPassword(data[3]);
                                user.setUsername(data[4]);
                                user.setEmail(data[5]);
                   
                                if(!database.mydatabase.isDataExistsInRowFromTableWhereEqual("users","username", data[4]));
                                {
                                    database.insertUserToDatabase(user);
                                    bzero(message,sizeof(message));
                                    mess = "Successfully registered you as " + data[4] + " " + data[3];

                                    strcpy(message, mess.c_str());

                                    write(connection, message, sizeof(message));
                                }

                                break;

                            case 1: // LOGIN
                                
                                bytes = 0;
                                bzero(message,sizeof(message));

                                while(bytes <= 0)
                                {
                                    bytes = read(connection,message, sizeof(message));
                                    std::cout << "Readed: " << message << std::endl;
                                }

                                std::cout << "LOG: received data from client: \n " << message << std::endl;
                                // PARSING THE DATA FROM CLIENT
                                for(size_t jt = 3; jt < 5 ; jt++)
                                {
                                        for(size_t it = 0; it < bytes; it++)
                                        {
                                            if((message[it] == ':') &&  jt == 4)
                                            {
                                                jt++;
                                                break;
                                            }
                                            if(message[it] == ':'|| message[it] == '\0')
                                            {
                                                jt++; 
                                            }
                                            if(message[it] != ':')
                                            {
                                                data[jt] += message[it];
                                            }   
                                        }
                                }
                                std::cout << "password: " << data[3] << "\n";
                                std::cout << "username: " << data[4] << "\n";

                                user.setPassword(data[3]);
                                user.setUsername(data[4]);
                                
                                user_menu = user;
                                bzero(message,sizeof(message));

                                if(database.mydatabase.isDataExistsInRowFromTableWhereEqual("users","username", data[4]))
                                {
                                    if(database.mydatabase.isDataExistsInRowFromTableWhereEqual("users","password", data[3]))
                                    {
                                        std::cout << "Client successfully logged in as: " << data[4]<< " : " << data[3] << " via socket " << connection << std::endl;
                                        mess = "Successfully logged in.";
                                        strcpy(message,mess.c_str());

                                        write(connection, message, sizeof(message));

                                        current_user = user;
                                    }
                                    else
                                    {
                                        mess = "Incorrect password. Try later.";
                                        strcpy(message,mess.c_str());

                                        write(connection, message, sizeof(message));
                                    }
                                }
                                else
                                {
                                    mess = "Cannot find that user in Database. Try later.";
                                    strcpy(message,mess.c_str());

                                    write(connection, message, sizeof(message));
                                }

                                wait_for_user_menu(connection);

                                
                                // Space for user menu and choosing menu
                               

                                break;

                            case 2:
                                // Client sent 2. It could be Reading messages

                                break;
                            
                            case 3:
                                // Client sent 3. It could be sending messages

                                break;

                            case 4:
                                // CLient sent 4. It could be logging out
                                break;

                            case 5:
                                // Client sent 5. It could be something
                                break;
                            default:

                                break;


                        }
                    }
                    
                    

                    
                

                // Close connection
                close(connection);
                };


        }

        
    }

private:
    void registration()
    {

    }

    void login()
    {

    }


};