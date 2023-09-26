#include "Channel.hpp"
#include <iostream>
#include <netdb.h>
#include "Server.hpp"

Channel::Channel(std::string _channelName) : channelName(_channelName) { userCount = 0; }
Channel::~Channel(){}

bool Channel::addUser(int userFD, std::string nickname)
{
	if (checkUser(nickname))
		return false;
	users[nickname] = userFD;
	userCount++;
	showMessage(nickname, "has joined the channel\r\n");
	return true;
}

bool Channel::checkUser(std::string nickname)
{
	std::map<std::string, int>::iterator it = users.find(nickname);
	return (it != users.end());
}

void Channel::showMessage(std::string nickname, std::string message)
{
	message = channelName + "/" + nickname + ": " + message;
	std::map<std::string, int>::iterator it;
	for (it = users.begin(); it != users.end(); it++)
		send(it->second, message.c_str(), message.size(), 0);
}

bool Channel::kickUser(std::string nickname, std::vector<std::string> tokens)
{
	if (checkUser(nickname) == false)
		return false;
	users.erase(nickname);
	userCount--;
	std::string message = "has left the channel";
	if (!tokens.empty() && tokens[2].empty() == false)
		message += ": " + getMessage(tokens);
	if (message[message.size() - 1] != '\n')
		message += "\r\n";
	showMessage(nickname, message);
	return true;
}

bool Channel::setOperator(std::string name)
{
	std::map<std::string, int>::iterator it = users.find(name);
	if (it == users.end())
		return false;
	channelOperator = name;
	return true;
}

