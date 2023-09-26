#pragma once

#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <string>

#define MAX_CLIENTS 15

class Client
{
	private:
		std::vector<std::string> channels;
		char		hostname[1024];
		int			clientFD;
		std::string	username;
		std::string	nickname;
		std::string	realName;
		std::string	displayName;

		bool registered;
		bool enteredPass;

	public:
		void setNickname(std::string _nickname)			{nickname = _nickname;}
		void setUsername(std::string _username)			{username = _username;}
		void setRealName(std::string _realName)			{realName = (_realName[0] == ':') ? _realName : ":" + _realName;}
		void setDisplayName(std::string _displayName)	{displayName = _displayName;}
		char *getHostname()								{return hostname;}
		std::string	getUsername()						{return username;}
		std::string	getNickname()						{return nickname;}
		std::string	getRealName()						{return realName;}
		std::string	getDisplayName()					{return displayName;}
		int getFD()										{return clientFD;}
		void		setRegistered()						{registered = true;}
		bool		isRegistered()						{return registered;}
		void		setEnteredPass()					{enteredPass = true;}
		bool		isEnteredPass()						{return enteredPass;}
		void eraseChannel(std::string channelName);

		Client(int _clientSocket);
		~Client();
		
		void recieveMessage(int fd);
		void addChannel(std::string channelName) { channels.push_back(channelName); }
		std::vector<std::string> getChannels() {return channels;}
};