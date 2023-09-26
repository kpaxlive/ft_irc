#pragma once

#include <string>
#include <vector>
#include <map>

class Channel
{
	std::string channelName;
	std::map<std::string, int> users;
	std::string channelOperator;

	int userCount;
	public:
		
		Channel(std::string _channelName);
		~Channel();

		bool addUser(int userFD, std::string nickname);
		bool checkUser(std::string nickname);
		void showMessage(std::string nickname, std::string message);
		bool kickUser(std::string nickname, std::vector<std::string> tokens);
		std::string getChannelName() {return channelName;}
		int getUserCount() {return userCount;}
		std::map<std::string, int> getUsers() {return users;}
		bool setOperator(std::string name);
		std::string getOperator() {return channelOperator;}
};
