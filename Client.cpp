#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <fcntl.h>
#include <arpa/inet.h>

#include "Client.hpp"

Client::Client(int _clientFD)
{
	this->clientFD = _clientFD;
	this->registered = false;
	this->enteredPass = false;
}

Client::~Client(){}

void Client::recieveMessage(int fd)
{
	std::string message;

	char buffer[100];
	bzero(buffer, 100);

	while (!std::strstr(buffer, "\n"))
	{
		bzero(buffer, 100);
		if (recv(fd, buffer, 100, 0) < 0)
		{
			std::cerr << "**** Reading Message: Failed ****" << std::endl;
			return;
		}
		message.append(buffer);
	}
	message.erase(message.find_last_not_of(" \n\r\t\v\f")+1);
}

void Client::eraseChannel(std::string channelName)
{
	std::vector<std::string>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++)
		if (*it == channelName)
		{
			channels.erase(it);
			return;
		}
}

