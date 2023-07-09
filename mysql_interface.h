#pragma once
#include <mysql/mysql.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <vector>

#include "user.h"

const std::string DEFAULT_DATABASE_NAME = "my_database";
const std::string DEFAULT_PASSWORD_TO_DATABASE = "412313";
const std::string DEFAULT_USERNAME_FOR_DATABASE = "root";
const std::string DEFAULT_HOSTNAME = "localhost";

/*
    Tables, that are required to work int mysql with that code:

    CREATE TABLE users (id integer primary key auto_increment, name varchar(255), surname varchar(255),phone varchar(255), password varchar(255), username varchar(10), email varchar(255));
    CREATE TABLE messages ( username_from varchar(10), username_to varchar(10), data varchar(1024), date datetime, id integer auto_increment)



    INSERT INTO users (name, surname, phone, password, username, email) values ("Semen", "Trotsenko", "89873729856", "smokedsats", "semik16156@mail.ru");

    INSERT INTO messages (username_from, username_to, data, date) values ("smokedsats", "qwertgfdsa", "hello, my message is first", GETDATE())


*/

class mysql_interface
{
    // ������������� ���������� � �� MySQL
public: // objects with vars

    MYSQL connection_mysql; // obj MYSQL for interface
    MYSQL_RES* result_from_mysql = nullptr;
    MYSQL_ROW row_from_mysql = nullptr;

    bool is_db_connected = false; // Needed to check connected db or not to send queries

public: // functions and methods
    bool isConnected() { return is_db_connected; }

    mysql_interface()
    {
        result_from_mysql = nullptr;
        row_from_mysql = nullptr;
    }

    // Constuctor to use without func "connect_to_mysql"
    mysql_interface(const std::string& hostname_or_ipaddress, const std::string& username, const std::string& pass, const std::string& dbname)
    {
        /*connection_mysql = mysql_init(NULL);
        if (connection_mysql == NULL) {
            std::cout << "ERROR in MYSQL: " << mysql_error(&connection_mysql) << std::endl;
            return;
        }*/
        mysql_init(&connection_mysql);

        if (mysql_real_connect(&connection_mysql, hostname_or_ipaddress.c_str(), username.c_str(), pass.c_str(), dbname.c_str(), 0, NULL, 0) == NULL) {
            std::cout << "ERROR in MYSQL: " << mysql_error(&connection_mysql) << std::endl;
            mysql_close(&connection_mysql);
            return;
        }
        else
        {
            std::cout << "Successfully connected to db: " << dbname << " as " << username << " host is " << hostname_or_ipaddress << std::endl;
            is_db_connected = true;
        }

        
    }


    //done
    void connect_to_mysql(const std::string& hostname_or_ipaddress, const std::string& username, const std::string& pass, const std::string& dbname)
    {
        mysql_init(&connection_mysql);
        //mysql_real_connect(&connection_mysql, hostname_or_ipaddress.c_str(), username.c_str(), pass.c_str(), "", 0, NULL, 0);

        

        if (mysql_real_connect(&connection_mysql, hostname_or_ipaddress.c_str(), username.c_str(), pass.c_str(), "", 0, NULL, 0) == NULL) {
            std::cout << "ERROR in MYSQL: " << mysql_error(&connection_mysql) << std::endl;
            mysql_close(&connection_mysql);
            return;
        }
        else
        {
            std::cout << "Successfully connected to db: " << dbname << " as " << username << " host is " << hostname_or_ipaddress << std::endl;
            is_db_connected = true;
        }

        create_database(dbname);

        std::string query = "USE " + dbname;

        mysql_query(&connection_mysql, query.c_str());

        return;
    }

    // done 
    bool create_table(const std::string& table_name, const std::string& typesandnamesofrows)
    {
        
        std::string query = " CREATE TABLE IF NOT EXISTS " + table_name + " ( " + typesandnamesofrows + " )";

        mysql_query(&connection_mysql, query.c_str());
        return true;
    }

    // done
    bool checkIfDatabaseExists(const std::string& dbname)
    {
        std::vector<std::string> dbnames;

        if (mysql_query(&connection_mysql, "SHOW DATABASES")) {
            std::cout << "Error listing databases: " << mysql_error(&connection_mysql) << std::endl;
            return false;
        }

        result_from_mysql = mysql_use_result(&connection_mysql);

        std::cout << "List of databases:" << std::endl;
        while ((row_from_mysql = mysql_fetch_row(result_from_mysql)) != NULL) {
            dbnames.push_back(row_from_mysql[0]);
            std::cout << row_from_mysql[0] << std::endl;
        }

        std::string query = "USE " + dbname;

        for (auto& res : dbnames)
        {
            if (!strcmp(res.c_str(), dbname.c_str()))
            {
                mysql_query(&connection_mysql, query.c_str());
                mysql_free_result(result_from_mysql);
                return true;
            }
        }

        mysql_free_result(result_from_mysql);
        return false;
    }

    // done
    bool create_database(const std::string& dbname)
    {
        std::string query = "CREATE DATABASE IF NOT EXISTS "; // query string
        std::string real_query = query + dbname; // concat strings 
        if (mysql_query(&connection_mysql, real_query.c_str())) {
            std::cout << "Failed to create database: " << mysql_error(&connection_mysql) << std::endl;
            return false;
        }
        // Turn using database with name "&dbname"
        query = "USE " + dbname; 
        mysql_query(&connection_mysql, query.c_str());
        return true;
    }
    // done
    bool send_query_to_db(const std::string& query)
    {
        if (is_db_connected)
        {
            // debug output
            //std::cout << "send_query_to_db query = " << query << std::endl;

            mysql_query(&connection_mysql, query.c_str());
            std::cout << "Successfully sent query to db." << std::endl;
            return true;
        }
        else
        {
            std::cout << "ERROR MYSQL: can't send a query. Database is not connected. Try to connect. " << std::endl;
            return false;
        }
    }

    bool insert_into_table(const std::string& table, const std::string& types, const std::string& data)
    {
        std::string query = "INSERT INTO " + table + " ( " + types + " ) " + " VALUES " + " ( " + data + " ) ";

        if (this->isConnected())
        {
            send_query_to_db(query);
            return true;
        }
        else
        {
            std::cout << "MYSQL_INTERFACE ERROR: " << "DATABASE IS NOT CONNECTED " << std::endl;
            return false;
        }
    }

    bool isDataExistsInRowFromTableWhereEqual(const std::string& table, const std::string & row , const std::string& data)
    {
        std::string query = "SELECT * FROM " + table + " WHERE " + row + " = " + "\'" + data + "\'";
        std::vector<std::string> results;

        if(this->isConnected())
        {
            
            send_query_to_db(query);
            if (result_from_mysql = mysql_store_result(&connection_mysql))
            {
                while (row_from_mysql = mysql_fetch_row(result_from_mysql))
                {
                    for (int i = 0; i < mysql_num_fields(result_from_mysql); i++)
                    {
                        results.push_back(row_from_mysql[i]);
                    }
                }
                
                for (auto& it : results)
                {
                    if (!strcmp(it.c_str(), data.c_str()))
                    {
                        mysql_free_result(result_from_mysql);
                        return true;
                    }
                }
                mysql_free_result(result_from_mysql);
                return false;
            }
        }
        mysql_free_result(result_from_mysql);
        return false;
    }

};

struct mysql
{
    mysql_interface mydatabase;
    bool isDbConnected = false;
    /*void connect()
    {
        // ������������� ���������� � MySQL
        mydatabase.connect_to_mysql(DEFAULT_HOSTNAME, DEFAULT_USERNAME_FOR_DATABASE,
                        DEFAULT_PASSWORD_TO_DATABASE, "");

        if (!mydatabase.checkIfDatabaseExists(DEFAULT_DATABASE_NAME))
        {
            // ������� ����� ���� ������
            mydatabase.create_database(DEFAULT_DATABASE_NAME);
        }
        // ���������� ������ �� �������� �������
        std::string create_table_query = "CREATE TABLE my_query (id INT, name VARCHAR(50), age INT)";
        mydatabase.send_query_to_db(create_table_query);

    }*/
    // OK
    void connect(const std::string & dbname = DEFAULT_DATABASE_NAME, const std::string & hostname =DEFAULT_HOSTNAME,const std::string & username =DEFAULT_USERNAME_FOR_DATABASE, const std::string & password = DEFAULT_PASSWORD_TO_DATABASE)
    {
        // ������������� ���������� � MySQL
        /*mydatabase.connect_to_mysql(hostname, username, password,"");

        if (!mydatabase.checkIfDatabaseExists(dbname))
        {
            // ������� ����� ���� ������
            mydatabase.create_database(dbname);
        }*/

        mydatabase.connect_to_mysql(hostname, username, password, dbname);

        // ���������� ������ �� �������� �������
        /*std::string create_table_query = "CREATE TABLE my_query (id INT, name VARCHAR(50), age INT)";
        mydatabase.send_query_to_db(create_table_query);
        */
    }
    // OK
    void insertUserToDatabase(User & user)
    {
        std::string name = user.currentUser.getName();
        std::string surname = user.currentUser.getSurname();
        std::string phone = user.currentUser.getPhone();
        std::string password = user.currentUser.getPassword();
        std::string username = user.currentUser.getUsername();
        std::string email = user.currentUser.getEmail();
        uint64_t id = user.currentUser.getId();
        std::string id_string = std::to_string(id);

        std::string data = "\'" + name + "\'" + ", " + "\'" + surname + "\'" + ", " +"\'"+ phone + "\'" + ", "
         + "\'"+  password+ "\'" + ", " + "\'" + username + "\'" + ", "+ "\'" + email + "\'";
        std::string types = "name, surname, phone, password, username, email";//  VARCHAR(255), VARCHAR(255), VARCHAR(255), VARCHAR(255), VARCHAR(10), VARCHAR(255)
        std::string table = "users";

        if(mydatabase.isConnected())
        {
            mydatabase.insert_into_table(table, types, data);
        }
    }
    void insertUserToDatabase(User_interface & user)
    {
        std::string name = user.getName();
        std::string surname = user.getSurname();
        std::string phone = user.getPhone();
        std::string password = user.getPassword();
        std::string username = user.getUsername();
        std::string email = user.getEmail();
        uint64_t id = user.getId();
        std::string id_string = std::to_string(id);

        std::string data = "\'" + name + "\'" + ", " + "\'" + surname + "\'" + ", " +"\'"+ phone + "\'" + ", "
         + "\'"+  password+ "\'" + ", " + "\'" + username + "\'" + ", "+ "\'" + email + "\'";
        std::string types = "name, surname, phone, password, username, email";//  VARCHAR(255), VARCHAR(255), VARCHAR(255), VARCHAR(255), VARCHAR(10), VARCHAR(255)
        std::string table = "users";

        if(mydatabase.isConnected())
        {
            mydatabase.insert_into_table(table, types, data);
        }
    }
    // OK
    void insertMessageToDatabase(Message& message)
    {
        std::string from = message._from;
        std::string to = message._to;
        std::string data = message._data;

        std::string querydata = "\'" + from +"\'" + ", " + "\'"+  to + "\'" +  ", " + "\'" + data + "\'";

        std::string table = "messages";
        std::string types = "username_from, username_to, data";//"VARCHAR(10), VARCHAR(10), VARCHAR(1024)" datetime.size == 20;
        
        if(mydatabase.isConnected())
        {
             mydatabase.insert_into_table(table, types, querydata);
        }
    }
    // OK
    bool isPasswordMatchInDatabase(const std::string& password, const std::string& username)
    {
        // TODO check if password match with database
        
        std::string query = "SELECT password from users where ( username = \'" + username + "\' )";
        mydatabase.send_query_to_db(query);

        std::string password_from_mysql;

        if (mydatabase.result_from_mysql = mysql_store_result(&mydatabase.connection_mysql))
        {
            while (mydatabase.row_from_mysql = mysql_fetch_row(mydatabase.result_from_mysql))
            {
                for (int i = 0; i < mysql_num_fields(mydatabase.result_from_mysql); i++)
                {
                    password_from_mysql = mydatabase.row_from_mysql[i];
                }
            }

                if (!strcmp(password_from_mysql.c_str(), password.c_str()))
                {
                    mysql_free_result(mydatabase.result_from_mysql);
                    return true;
                }
            mysql_free_result(mydatabase.result_from_mysql);
            return false;
        }
        mysql_free_result(mydatabase.result_from_mysql);
        return false;
    }

    void disconnectFromMySQL() 
    {
        mysql_close(&mydatabase.connection_mysql);
        isDbConnected = false;
    }

    // OK
    std::vector<std::string> getUsersFromDatabase()
    {
        std::vector<std::string> users;
        std::string query = "select * from users;";
        std::string currentUsername;
        
        if(!mydatabase.send_query_to_db(query))
        {
            std::cout << "ERROR IN getUsersFromDatabase: " << mysql_error(&mydatabase.connection_mysql) << std::endl;
            return users;
        }

        mydatabase.result_from_mysql = mysql_use_result(&mydatabase.connection_mysql);

        if(mydatabase.result_from_mysql)
        {
            while((mydatabase.row_from_mysql = mysql_fetch_row(mydatabase.result_from_mysql)) != NULL)
            {
                currentUsername.clear();
                currentUsername = mydatabase.row_from_mysql[5]; 
               // std::cout << "Current username: " << currentUsername;
                users.push_back(currentUsername);
            }
        }
         mysql_free_result(mydatabase.result_from_mysql);
        
         return users;
    }

// TODO:::: ///////////////////////////////////// !!!!!!!!!!!!!!!!!!!!!!!
    // Исправить чтение сообщений от пользователя к дргому полльзователю. Сейчас выводятся все сообщения пользователей.
    
    std::vector<Message> getMessagesFromUToU(const std::string& username_1, const std::string& username_2)
    {
        std::vector<Message> messages;
        std::string message;

        Message currentMessage;

        std::string query = "SELECT * FROM messages WHERE ( (username_from = \'" + username_1+ "\' AND username_to =  \'" + username_2 + "\' ) or ( username_from = \'" + username_2 + "\' AND username_to = \'" + username_1 + "\'))  order by id asc";
        if(!mydatabase.send_query_to_db(query))
        {
            std::cout << "ERROR IN getMessagesFromUToU: " << mysql_error(&mydatabase.connection_mysql) << std::endl;
            return messages;
        }

        mydatabase.result_from_mysql = mysql_use_result(&mydatabase.connection_mysql);

        if(mydatabase.result_from_mysql)
        {
            while((mydatabase.row_from_mysql = mysql_fetch_row(mydatabase.result_from_mysql)) != NULL)
            {
                currentMessage._messageID = atoi(mydatabase.row_from_mysql[0]); 
                currentMessage._from = mydatabase.row_from_mysql[1];
                currentMessage._to = mydatabase.row_from_mysql[2];
                currentMessage._data = mydatabase.row_from_mysql[3];
                currentMessage._datetime = mydatabase.row_from_mysql[4];

                messages.push_back(currentMessage);
                currentMessage.clear();
            }
        }

        mysql_free_result(mydatabase.result_from_mysql);
        
    
        return messages;
    }

    
};