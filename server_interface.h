#pragma once
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>

#include "mysql_interface.h"
#include <ctime>

#define MESSAGE_LENGTH 4096 // Max length of data buffer
#define PORT 8080

// Volatile for user_menu



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

    User_interface UI_CURRENT_USER;

    std::string CURRENT_TIME;

public:
        // TODO: Добавить возможность обрабатывать сообщения от пользователя 
        //пользователю, отправлять список зарегистрированных пользователей, 
        //отправлять все смс, что есть в личном чате, если есть

    void updateTime()
    {
        std::time_t now = std::time(nullptr);
    	CURRENT_TIME = std::ctime(&now);
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

        std::vector<std::thread> threads;

        while (true) {
            socket_length = sizeof(client);
            int connection = accept(socket_file_descriptor, (struct sockaddr*)&client, &socket_length);

            if (connection == -1) {         
                std::cout << "ERROR: SERVER IS UNABLE TO ACCEPT THE DATA FROM CLIENT." << std::endl;
                continue;
            }

            // Create a new thread to handle the new client
            threads.emplace_back([connection, this]() {

                char CH_MESSAGE[MESSAGE_LENGTH];

                std::string STR_SERVER_RESPONCE;
                // FOR MAIN MENU
                std::string STR_NAME{""},STR_SURNAME{""},STR_PHONE{""}, STR_PASSWORD{""}, STR_USERNAME{""}, STR_EMAIL{""}; 
                std::string data[6];
                User_interface UI_USER;
                User USER_USER(UI_USER);

                std::vector<std::string> VEC_STR_USERS_FROM_DB;

                bool readed = false;

                size_t ULONG_CHOICE = 999;

                // FOR USER MENU
                std::string STR_USER_MENU[4] = {"/users", "/message", "/read", "/exit"};
        
                std::string STR_FULL_MESSAGE;
                std::vector<Message> VEC_MESSAGE_MESSAGES;
                std::vector<std::string> VEC_STR_USERS;
                std::string data_for_message[3];
                Message MESSAGE_MESSAGE;
                std::string STR_USER;
                std::string STR_MESSAGE;
                ssize_t LONG_BYTES = 0;
                std::string STR_LOGIN_MENU[3] = {"/reg", "/login", "/exit"};
                
                int sizeOfVector = 0;

                while (true) {
                    
                    bzero(CH_MESSAGE, strlen(CH_MESSAGE));
                    int k = 0;
                    LONG_BYTES = 0;

                    while (LONG_BYTES <= 0)
                    {
                        bzero(CH_MESSAGE, strlen(CH_MESSAGE));
                        LONG_BYTES = read(connection, CH_MESSAGE, sizeof(CH_MESSAGE));
                        
                        for (size_t it = 0; it < 3; it++)
                        {
                            if (!strcmp(CH_MESSAGE, STR_LOGIN_MENU[it].c_str()))
                            {
                                ULONG_CHOICE = it;
                                readed = true;
                                break;
                            }
                        }
                        
                        std::cout << "readed = " << CH_MESSAGE << std::endl;
                    } 

                    std::cout << "ULONG_CHOICE = " << ULONG_CHOICE << std::endl;
                    
                    

                    while(ULONG_CHOICE != 2)
                    {
                        LONG_BYTES = 0;

                        switch (ULONG_CHOICE)
                        {
                            case 0: // REGISTRATION
                                LONG_BYTES = 0;
                                ULONG_CHOICE = 2;

                                while(LONG_BYTES <= 0)
                                {
                                        LONG_BYTES = read(connection, CH_MESSAGE,sizeof(CH_MESSAGE));
                                }
                                std::cout << "LOG: received data from client: \n " << CH_MESSAGE << std::endl;
                                // PARSING THE DATA FROM CLIENT
                                for(size_t jt = 0; jt < 6 ; jt++)
                                {
                                        for(size_t it = 0; it < LONG_BYTES; it++)
                                        {
                                            if((CH_MESSAGE[it] == ':') &&  jt == 5)
                                            {
                                                jt++;
                                                break;
                                            }
                                            if(CH_MESSAGE[it] == ':'|| CH_MESSAGE[it] == '\0')
                                            {
                                                jt++; 
                                            }
                                            if(CH_MESSAGE[it] != ':')
                                            {
                                                data[jt] += CH_MESSAGE[it];
                                            }   
                                        }
                                }
                                bzero(CH_MESSAGE,strlen(CH_MESSAGE));

                                std::cout << "name: " << data[0] << "\n";
                                std::cout << "surname: " << data[1] << "\n";
                                std::cout << "phone: " << data[2] << "\n";
                                std::cout << "password: " << data[3] << "\n";
                                std::cout << "username: " << data[4] << "\n";
                                std::cout << "email: " << data[5] << "\n";
                                
                                UI_USER.setName(data[0]);
                                UI_USER.setSurname(data[1]);
                                UI_USER.setPhone(data[2]);
                                UI_USER.setPassword(data[3]);
                                UI_USER.setUsername(data[4]);
                                UI_USER.setEmail(data[5]);
                   
                                if(!database.mydatabase.isDataExistsInRowFromTableWhereEqual("users","username", data[4]));
                                {
                                    database.insertUserToDatabase(UI_USER);
                                    bzero(CH_MESSAGE,strlen(CH_MESSAGE));
                                    STR_SERVER_RESPONCE = "Successfully registered you as " + data[4] + " " + data[3];

                                    strcpy(CH_MESSAGE, STR_SERVER_RESPONCE.c_str());

                                    write(connection, CH_MESSAGE, strlen(CH_MESSAGE));
                                }

                                break;

                            case 1: // LOGIN
                                
                                LONG_BYTES = 0;
                                bzero(CH_MESSAGE,strlen(CH_MESSAGE));

                                while(LONG_BYTES <= 0)
                                {
                                    LONG_BYTES = read(connection,CH_MESSAGE, sizeof(CH_MESSAGE));
                                    std::cout << "Readed: " << CH_MESSAGE << std::endl;
                                }

                                std::cout << "LOG: received data from client: \n " << CH_MESSAGE << std::endl;
                                // PARSING THE DATA FROM CLIENT
                                for(size_t jt = 3; jt < 5 ; jt++)
                                {
                                        for(size_t it = 0; it < LONG_BYTES; it++)
                                        {
                                            if((CH_MESSAGE[it] == ':') &&  jt == 4)
                                            {
                                                jt++;
                                                break;
                                            }
                                            if(CH_MESSAGE[it] == ':'|| CH_MESSAGE[it] == '\0')
                                            {
                                                jt++; 
                                            }
                                            if(CH_MESSAGE[it] != ':')
                                            {
                                                data[jt] += CH_MESSAGE[it];
                                            }   
                                        }
                                }
                                std::cout << "password: " << data[3] << "\n";
                                std::cout << "username: " << data[4] << "\n";

                                UI_USER.setPassword(data[3]);
                                UI_USER.setUsername(data[4]);
                                
                                USER_USER = UI_USER;
                                bzero(CH_MESSAGE,strlen(CH_MESSAGE));

                                if(database.mydatabase.isDataExistsInRowFromTableWhereEqual("users","username", data[4]))
                                {
                                    if(database.mydatabase.isDataExistsInRowFromTableWhereEqual("users","password", data[3]))
                                    {
                                        std::cout << "Client successfully logged in as: " << data[4]<< " : " << data[3] << " via socket " << connection << std::endl;
                                        STR_SERVER_RESPONCE = "Successfully logged in.";
                                        strcpy(CH_MESSAGE,STR_SERVER_RESPONCE.c_str());

                                        write(connection, CH_MESSAGE, strlen(CH_MESSAGE));

                                        UI_CURRENT_USER = UI_USER;
                                    }
                                    else
                                    {
                                        STR_SERVER_RESPONCE = "Incorrect password. Try later.";
                                        strcpy(CH_MESSAGE,STR_SERVER_RESPONCE.c_str());

                                        write(connection, CH_MESSAGE, strlen(CH_MESSAGE));
                                    }
                                }
                                else
                                {
                                    STR_SERVER_RESPONCE = "Cannot find that user in Database. Try later.";
                                    strcpy(CH_MESSAGE,STR_SERVER_RESPONCE.c_str());

                                    write(connection, CH_MESSAGE, strlen(CH_MESSAGE));
                                }

                                STR_SERVER_RESPONCE.clear();
                                //wait_for_user_menu(connection);

                                // TESTING////////////////////////////////////////
                                LONG_BYTES = 0;
                                
                                std::cout << "Waiting for user responce. Current user is " << UI_CURRENT_USER.getUsername() << std::endl;
                                

                                k = 0;

                                while(ULONG_CHOICE != 3)
                                {
                                    ULONG_CHOICE = 666;
                                    bzero(CH_MESSAGE, strlen(CH_MESSAGE));

                                    std::cout << "Waiting for ulong_choice from client:" << std::endl;

                                    LONG_BYTES = 0;

                                    while(LONG_BYTES <= 0)
                                        {
                                            // попробовать изменить sizeof(ch_message) и strlen на MESSAGE_LENGTH.
                                            bzero(CH_MESSAGE, strlen(CH_MESSAGE));

                                            LONG_BYTES = read(connection, CH_MESSAGE, sizeof(CH_MESSAGE));

                                            ////////////////////////////////
                                        }
                                    std::cout << "Comparing user ulong_choice from user menu. " << std::endl;
                                    
                                    for(size_t i = 0; i < 4; i++)
                                        {
                                            if(!strcmp(CH_MESSAGE, STR_USER_MENU[i].c_str()))
                                            {
                                                ULONG_CHOICE = i;
                                                break;
                                            }
                                        }
                                        
                                        switch (ULONG_CHOICE)
                                        {
                                            case 0: 
                                                sizeOfVector = 0;
                                                LONG_BYTES = 0;
                                                std::cout << "Client choosed /users" << std::endl;
                                                VEC_STR_USERS = database.getUsersFromDatabase();
                                                
                                                
                                                for(auto i = 0; i < VEC_STR_USERS.size(); i++)
                                                {
                                                    for(auto j = 0; j < VEC_STR_USERS.at(i).size(); j++)
                                                    {
                                                        sizeOfVector++;
                                                    }
                                                }


                                                bzero(CH_MESSAGE, strlen(CH_MESSAGE));

                                                for(auto i =0 ; i < sizeOfVector;)
                                                {
                                                    if(i >= 4095)
                                                    {
                                                        break;
                                                    }

                                                    for(auto it : VEC_STR_USERS)
                                                    {
                                                        it.push_back(':');
                                                        sizeOfVector++;
                                                        for(auto j = 0; j < it.size(); j++)
                                                        {
                                                            CH_MESSAGE[i++] = it.at(j);
                                                        }
                                                    }
                                                }

                                                STR_SERVER_RESPONCE.clear();
                                                
                                                for(auto user : VEC_STR_USERS)
                                                {
                                                    STR_SERVER_RESPONCE += (user + ":");
                                                    std::cout << "USER IN VECTOR : " << user << std::endl;
                                                }
                                                
                                                STR_SERVER_RESPONCE.resize(sizeOfVector);

                                                
                                                if(!STR_SERVER_RESPONCE.empty())
                                                {
                                                    STR_SERVER_RESPONCE.pop_back();
                                                }
                                                
                                                bzero(CH_MESSAGE, strlen(CH_MESSAGE));
                                                std::cout << "\nresponce string is: " << STR_SERVER_RESPONCE << std::endl;
                                                strcpy(CH_MESSAGE, STR_SERVER_RESPONCE.c_str());

                                                std::cout << "\nSent ch_message to client: " << STR_SERVER_RESPONCE << std::endl;

                                                write(connection, CH_MESSAGE, strlen(CH_MESSAGE));
                                                
                                                break;

                                            case 1:
                                                data_for_message[0].clear();
                                                data_for_message[1].clear();
                                                data_for_message[2].clear();

                                                std::cout << "Client picked /message. " << std::endl;
                                                std::cout << "Waiting for data from client..." << std::endl;
                                                
                                                LONG_BYTES = 0;

                                                while (LONG_BYTES <= 0)
                                                {
                                                    bzero(CH_MESSAGE, strlen(CH_MESSAGE));
                                                    LONG_BYTES = read(connection, CH_MESSAGE, sizeof(CH_MESSAGE));
                                                }
                                                
                                                std::cout << "Received data from client: " << CH_MESSAGE << std::endl;

                                                // ERROR IS HERE. 
                                                for(size_t jt = 0; jt < 3 ; jt++)
                                                {
                                                    for(size_t it = 0; it < LONG_BYTES; it++)
                                                    {
                                                        //std::cout <<"MAX_SIZE:" << LONG_BYTES  << "  symb:" <<  CH_MESSAGE[it] << " it:" << it << "  jt:" << jt;
                                                        if((CH_MESSAGE[it] == ':') &&  jt == 2)
                                                        {
                                                            break;
                                                        }
                                                        if(CH_MESSAGE[it] == ':')
                                                        {
                                                            jt++; 
                                                        }
                                                        if(CH_MESSAGE[it] != ':')
                                                        {
                                                            data_for_message[jt] += CH_MESSAGE[it];
                                                        }   
                                                    }
                                                }

                                                bzero(CH_MESSAGE, strlen(CH_MESSAGE));

                                                MESSAGE_MESSAGE.setMessage(data_for_message[0], data_for_message[1],data_for_message[2], CURRENT_TIME);
                                                
                                                database.insertMessageToDatabase(MESSAGE_MESSAGE);
                                                
                                                STR_SERVER_RESPONCE += "Successfully sent ch_message to database " + data_for_message[0]+ ":" + data_for_message[1]+ ":" + data_for_message[2];
                                                for(auto i = 0; i < STR_SERVER_RESPONCE.size(); i++)
                                                {
                                                    CH_MESSAGE[i] = STR_SERVER_RESPONCE.at(i);
                                                }

                                                write(connection, CH_MESSAGE, strlen(CH_MESSAGE));

                                                std::cout << "Sent ch_message to client: " << CH_MESSAGE << std::endl;

                                                break;


                                            case 2:
                                                LONG_BYTES = 0;

                                                while(LONG_BYTES <= 0)
                                                {
                                                    bzero(CH_MESSAGE, strlen(CH_MESSAGE));
                                                    LONG_BYTES = read(connection, CH_MESSAGE, sizeof(CH_MESSAGE));
                                                }
                                                
                                                STR_MESSAGE.clear();

                                                for(auto i = 0; i < strlen(CH_MESSAGE);i++)
                                                {
                                                    STR_MESSAGE += CH_MESSAGE[i];
                                                }

                                                VEC_MESSAGE_MESSAGES.clear();

                                                VEC_MESSAGE_MESSAGES = database.getMessagesFromUToU(UI_CURRENT_USER.getUsername(), STR_MESSAGE);

                                                
                                                for(auto it : VEC_MESSAGE_MESSAGES)
                                                {
                                                    STR_MESSAGE.clear();
                                                    for(auto i = 0; i < 5; i++)
                                                    {
                                                        STR_MESSAGE += it._datetime + "/from/" + it._from + "/to/" + it._to + "/message/" + it._data + "/";
                                                    }
                                                    
                                                    STR_FULL_MESSAGE += STR_MESSAGE + "/";
                                                    
                                                }

                                                
                                                if(STR_FULL_MESSAGE.size() >= 4096)
                                                {
                                                    STR_FULL_MESSAGE.resize(4095);
                                                }
                                                
                                                bzero(CH_MESSAGE, strlen(CH_MESSAGE));
                                                strcpy(CH_MESSAGE, STR_FULL_MESSAGE.c_str());

                                                write(connection, CH_MESSAGE, strlen(CH_MESSAGE));

                                                std::cout << "Sent ch_message to client: " << CH_MESSAGE << std::endl; 

                                                break;

                                            case 3:


                                                break;

                                        }
                                }
                                // Space for user menu and choosing menu
                                //
                                // /////////////////////////////////////////////////////

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
                    
                    

                    
                }

                // Close connection
                close(connection);
                });


        }

        // Wait for all threads to finish before exiting
        for (auto& thread : threads) {
            thread.join();
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