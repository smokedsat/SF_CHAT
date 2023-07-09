#pragma once

#include "user.h"
#include "message.h"
#include "mysql_interface.h"
#include "server_interface.h"

using namespace std;

static bool online = false;

class Chat
{
private:
	std::vector<User> users;
	
	bool isServerRunning = false;
	bool isConnectedToNetwork = false;
	bool isConnectedToDatabase = false;

	mysql database;
	server_interface server;
	
public:


	void Register()
	{
		if (!isConnectedToDatabase)
		{
			database.connect();
			
		}
	}
	void Register()
	{

	}

	void Login()
	{

	}

	void connectToNetwork()
	{
		if (server.isConnected())
		{
			std::cout << "Server is already connected to network." << std::endl;

		}
		server.run();
		
	}

private:
	std::vector<Message> getMessagesFromDB(const std::string& from, const std::string& to)
	{

	}
};


std::vector<Message> Chat::getMessagesFromDB(const std::string& from, const std::string& to)
{

}


void Chat::Register(const std::string &login, const std::string &password, const std::string &username)
{
	
}

void Chat::Register()
{
	cout << "����������, ������� ��� �����: " << endl;
	string login, password, username;
	cin >> login;
	base[_currentUser].setLogin(login);
	cout << "����������, ������� ��� ������: " << endl;
	cin >> password;
	base[_currentUser].setPassword(password);
	cout << "����������, ������� ��� �������: " << endl;
	cin >> username;
	base[_currentUser].setUsername(username);
	cout << "�� ������� ���������������� ���: " << endl
		<< username << endl;;
	_currentUser++;
}

void Chat::login()
{
	string login, password;
	bool checker = false;
	while (checker != true)
	{
		cout << endl << "������� ��� �����:	" << endl;
		cin >> login;

		for (auto i = 0; i < usersCount; i++)
		{
			if (login == base[i].getLogin())
			{
				cout << "������� ��� ������: " << endl;
				cin >> password;
				if (password == base[i].getPassword())
				{
					cout << "������, " << base[i].getUsername();
					_currentUser = i;
					base[_currentUser].showMenu();
					checker = true;
					break;
				}
			}
			if ((i = usersCount) && (login != base[i].getLogin()))
			{
				cout << endl << "������������ �� ������, ��������� �������. " << endl;
				break;
			}
		}
	}
}



void startChat()
{
	online = true;

	cout << "����� ���������� � ���������� ���. �� ������ ��������� ��������� ��������: " << endl
		<< "1. ������������������ " << endl
		<< "2. ����� � ������� " << endl
		<< "0. ������� ���������� " << endl;

	Server a;

	char choice = '5';

	while (choice != 0)
	{
		cin >> choice;
		switch (choice)
		{
		case '1':
			a.Register();

			break;
		case '2':
			a.login();

			break;
		case '0':
			return;
			break;

		default:
			cout << "�������� 1, 2 ��� 0" << endl;

			break;
		}
	}

}

void stopChat()
{
	online = false;
}