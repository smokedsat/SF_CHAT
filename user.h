#pragma once
#include <string>
#include <iostream>
#include "message.h"
#include <vector>
#include <string.h>
#include <ctime>

class User_interface
{
	std::string _name;
	std::string _surname;
	std::string _phone;
	std::string _password;
	std::string _username;
	std::string _email;

	uint64_t _id;

public:
	// TODO: Check if username.size != 10;

	User_interface() : _name("default"), _surname("default"),_phone("default"),_password("default"),_username("default"),_email("default"), _id(0){	}
	
	
	User_interface(const std::string& name, const std::string& surname, const std::string& phone, const std::string& password, const std::string& username, const std::string& email )
		: _name(name),_surname(surname),_phone(phone),_password(password),_username(username),_email(email)
	{
		// TODO: add comparing as check max string size.
	}

	std::string& getUsername() { return _username; }
	std::string& getName() { return _name; }
	std::string& getSurname() { return _surname; }
	std::string& getPassword() { return _password; }
	std::string& getEmail() { return _email; }
	std::string& getPhone() { return _phone; }
	uint64_t & getId() { return _id; }

	void setUsername(const std::string &username) { _username = username; }
	void setName(const std::string& name) { _name = name; }
	void setSurname(const std::string& surname) { _surname = surname; }
	void setPassword(const std::string& password) { _password = password; }
	void setEmail(const std::string& email) { _email = email; }
	void setPhone(const std::string& phone) { _phone = phone; }
	void setId(uint64_t & id) { _id = id; }

	User_interface& operator=(User_interface& user)
	{
		this->_name = user._name;
		this->_surname = user._surname;
		this->_id = user._id;
		this->_password = user._password;
		this->_phone = user._phone;
		this->_username = user._username;
		this->_email = user._email;

		return *this;
	}
};

class User
{
public:
	User_interface currentUser;
	std::vector<struct Message> _messages;
	std::string currentTime;
public:
	User(const std::string& name, const std::string& surname, const std::string& password, const std::string& username,  const std::string& phone = "default", const std::string& email = "default")
	{
		User_interface newUser(name, surname, phone, password, username, email);
		currentUser = newUser;

	}
	User(User_interface & user) 
	{
		currentUser = user;
	}
	void sendMessage(const std::string& to, const std::string& message)
	{
		updateTime();
		Message mess;
		mess.setMessage(currentUser.getUsername(), to, message, currentTime);
		_messages.push_back(mess);
	}

	void readMessages(const std::string& with)
	{
		for (auto& it : _messages)
		{
			std::cout << "Message from: " << it._from << " to: " << it._to << " >> " << it._data << std::endl;
		}
	}

	void sendMessageToGeneral(const std::string& data)
	{
		updateTime();
		Message mess;
		mess.setMessage(currentUser.getUsername(), "all", data, currentTime);
		_messages.push_back(mess);
	}

	void readGeneralMessages()
	{
		for (auto& it : _messages)
		{
			if(!strcmp(it._to.c_str(),"all"))
			{
				std::cout << "Message from: " << it._from << " to: all >> " << it._data << std::endl;
			}
		}
	}


	User& operator=(User_interface& user)
	{
		this->currentUser.setName(user.getName());
		this->currentUser.setSurname(user.getSurname());
		this->currentUser.setId(user.getId());
		this->currentUser.setPassword(user.getPassword());
		this->currentUser.setPhone(user.getPhone());
		this->currentUser.setUsername(user.getUsername());
		this->currentUser.setEmail(user.getEmail());
		return *this;
	}

private:
	void updateTime()
	{
		std::time_t now = std::time(nullptr);
    	currentTime = std::ctime(&now);
	}
};
