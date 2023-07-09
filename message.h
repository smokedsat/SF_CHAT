#pragma once
#include <iostream>
#include <string>
#include <vector>


struct Message
{
	std::string _from;
	std::string _to;
	std::string _data;
	std::string _datetime;
	uint64_t _messageID;
public:
	void setMessage(const std::string& from, const std::string& to, const std::string& data, std::string& datetime,  uint64_t mess_id  = 0)
	{
		_from = from;
		_to = to;
		_data = data;
		_messageID = mess_id;
		_datetime = datetime;
	}

	void clear()
	{
		_from.clear();
		_to.clear();
		_data.clear();
		_datetime.clear();
		_messageID = 0;
	}
};