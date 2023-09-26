#include "Server.hpp"

std::string getMessage(std::vector<std::string> tokens)
{
	std::string message;
	for (size_t i = 2; i < tokens.size(); i++)
	{
		message += tokens[i];
		if (!tokens[i + 1].empty())
			message += " ";
	}
	message += "\r\n";
	return message;
}

void Server::quit(int fd, std::vector<std::string> tokens)
{
	onClientDisconnect(fd, tokens);
}

void Server::nick(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() != 2)
		return (sendMessageToClient(fd, "You must pass 1 argument"));
	if (tokens[1][0] == '#')
		return (sendMessageToClient(fd, "Nicknames can't start with #"));
	if (tokens[1][0] == '@')
		return (sendMessageToClient(fd, "Nicknames can't start with @"));
	if (tokens[1].find("ADMIN") != std::string::npos)
		return (sendMessageToClient(fd, "Nicknames can't contains ADMIN"));

	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); it++)
		if (it->second->getNickname() == tokens[1])
			return (sendMessageToClient(fd, "This nickname is already in use"));
	clients[fd]->setNickname(tokens[1]);
	sendMessageToClient(fd, "Nickname has setted");
}

void Server::pass(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() != 2)
		return(sendMessageToClient(fd, "You must pass 1 argument"));
	if (tokens[1] != password)
		return(sendMessageToClient(fd, "Password incorrect"));
	clients[fd]->setEnteredPass();
	sendMessageToClient(fd, "Password accepted");
}

void Server::user(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() != 5)
		return (sendMessageToClient(fd, "You must pass 4 argument: USER <username> <unused> <unused> <realname>"));
	if (clients[fd]->getNickname().empty())
		return (sendMessageToClient(fd, "You must set a Nickname first"));
	if (clients[fd]->isEnteredPass() == false)
		return (sendMessageToClient(fd, "You must enter server password first"));

	clients[fd]->setUsername(tokens[1]);
	clients[fd]->setRealName(tokens[4]);
	clients[fd]->setDisplayName(clients[fd]->getNickname() + "!" + clients[fd]->getUsername() + "@" + clients[fd]->getHostname());
	clients[fd]->setRegistered();
	sendMessageToClient(fd, "Welcome " + clients[fd]->getDisplayName());
}

void Server::privmsg(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() < 3)
		return (sendMessageToClient(fd, "You must pass 2 argument at least"));
	if (clients[fd]->isRegistered() == false)
		return (sendMessageToClient(fd, "You must register first"));

	std::string targetName = tokens[1];
	if (targetName[0] == '#') // CHANNEL
	{
		std::map<std::string, Channel *>::iterator it;
		it = channels.find(targetName);
		if (it == channels.end())
			return (sendMessageToClient(fd, "There is not a channel with that name"));
		if (channels[tokens[1]]->checkUser(clients[fd]->getNickname()) == false)
			return (sendMessageToClient(fd, "You are not member of the channel"));
		it->second->showMessage(clients[fd]->getNickname(), getMessage(tokens));
	}
	else // USER
	{
		int recieverFD = -1;
		std::map<int, Client *>::iterator it;
		for (it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->second->getNickname() != targetName)
				continue;
			recieverFD = it->second->getFD();
		}
		if (recieverFD == -1)
			return (sendMessageToClient(fd, "There is not an user with that name"));

		std::string message = getMessage(tokens);
		message = clients[fd]->getNickname() + ": " + message;
		send(recieverFD, message.c_str(), message.size(), 0);
	}
}

void Server::join(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() != 2)
		return (sendMessageToClient(fd, "You must pass 1 argument"));
	if (clients[fd]->isRegistered() == false)
		return (sendMessageToClient(fd, "You must register first"));
	if (tokens[1][0] != '#')
		return (sendMessageToClient(fd, "Channel names must be start with #"));

	std::string channelName = tokens[1];
	std::string nickname = clients[fd]->getNickname();

	std::map<std::string, Channel *>::iterator it;
	it = channels.find(channelName);
	if (it == channels.end())
	{
		channels[channelName] = new Channel(channelName);
		channels[channelName]->addUser(fd, nickname);
		clients[fd]->addChannel(channelName);
		channels[channelName]->setOperator(clients[fd]->getNickname());
		sendMessageToClient(fd, "You have been appointed as a new operator");
		return;
	}
	if (channels[channelName]->addUser(fd, nickname) == false)
		return (sendMessageToClient(fd, "User is already in the channel"));
	clients[fd]->addChannel(channelName);
}

void Server::part(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() < 2)
		return (sendMessageToClient(fd, "You must pass 1 argument at least"));

	std::map<std::string, Channel *>::iterator it = channels.find(tokens[1]);
	if (it == channels.end())
		return (sendMessageToClient(fd, "There is not a channel with that name"));

	if (it->second->kickUser(clients[fd]->getNickname(), tokens) == false)
		return (sendMessageToClient(fd, "You are not member of the channel"));
	
	clients[fd]->eraseChannel(it->first);
	
	if (channels[it->second->getChannelName()]->getUserCount() == 0)
	{
		std::string channelName = it->second->getChannelName();
		delete it->second;
		channels.erase(channelName);
	}
	else if (it->second->getOperator() == clients[fd]->getNickname())
	{
		int newOperatorFD = it->second->getUsers().begin()->second;
		it->second->setOperator(clients[newOperatorFD]->getNickname());
		sendMessageToClient(newOperatorFD, "You have been appointed as a new operator on " + it->second->getChannelName());
	}
}

void Server::list(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() != 1)
		return (sendMessageToClient(fd, "You can't pass argument"));
	if (clients[fd]->isRegistered() == false)
		return (sendMessageToClient(fd, "You must register first"));
	
	std::map<std::string, Channel *>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++)
	{
		if (it->second->getOperator() == clients[fd]->getNickname())
			sendMessageToClient(fd, it->second->getChannelName() + " - ADMIN");
		else if (it->second->checkUser(clients[fd]->getNickname()))
			sendMessageToClient(fd, it->second->getChannelName() + " - MEMBER");
		else
			sendMessageToClient(fd, it->second->getChannelName());
	}
}

void Server::member(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() != 1)
		return (sendMessageToClient(fd, "You can't pass argument"));
	if (clients[fd]->isRegistered() == false)
		return (sendMessageToClient(fd, "You must register first"));

	std::vector<std::string> userChannels = clients[fd]->getChannels();
	for (size_t i = 0; i < userChannels.size(); i++)
	{
		if (channels[userChannels[i]]->getOperator() == clients[fd]->getNickname())
			sendMessageToClient(fd, userChannels[i] + " - ADMIN");
		else
			sendMessageToClient(fd, userChannels[i]);
	}
}

void Server::who(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() != 2)
		return (sendMessageToClient(fd, "You must pass 1 argument"));
	if (clients[fd]->isRegistered() == false)
		return (sendMessageToClient(fd, "You must register first"));
	if (channels.find(tokens[1]) == channels.end())
		return (sendMessageToClient(fd, "There is not a channel with that name"));
	if (channels[tokens[1]]->checkUser(clients[fd]->getNickname()) == false)
		return (sendMessageToClient(fd, "You are not member of the channel"));
	
	sendMessageToClient(fd, "The channel has " + std::to_string(channels[tokens[1]]->getUserCount()) + " member");
	std::map<std::string, int> users = channels[tokens[1]]->getUsers();
	std::map<std::string, int>::iterator it;
	for (it = users.begin(); it != users.end(); it++)
	{
		if (it->first == channels[tokens[1]]->getOperator())
			sendMessageToClient(fd, it->first + " - ADMIN");
		else
			sendMessageToClient(fd, it->first);
	}
}

void Server::kick(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() < 3)
		return (sendMessageToClient(fd, "You must pass 3 arguments at least"));
	if (channels.find(tokens[1]) == channels.end())
		return (sendMessageToClient(fd, "There is not a channel with that name"));
	if (clients[fd]->getNickname() != channels[tokens[1]]->getOperator())
		return (sendMessageToClient(fd, "You are not authorized to kick"));	
	if (channels[tokens[1]]->checkUser(tokens[2]) == false)
		return (sendMessageToClient(fd, "There is not an user with that name in the channel"));	
	
	std::map<std::string, int> users = channels[tokens[1]]->getUsers();
	int targetFD = channels[tokens[1]]->getUsers().find(tokens[2])->second;
	part(targetFD, tokens);
	sendMessageToClient(targetFD, "You have been kicked from " + channels[tokens[1]]->getChannelName());
}

void Server::notice(int fd, std::vector<std::string> tokens)
{
	if (tokens.size() < 3)
		return (sendMessageToClient(fd, "You must pass 2 argument at least"));
	if (clients[fd]->isRegistered() == false)
		return (sendMessageToClient(fd, "You must register first"));

	std::string targetChannel = tokens[1];
	if (targetChannel[0] == '#')
	{
		std::map<std::string, Channel *>::iterator it;
		it = channels.find(targetChannel);
		if (it == channels.end())
			return (sendMessageToClient(fd, "There is not a channel with that name"));
		if (clients[fd]->getNickname() != channels[tokens[1]]->getOperator())
			return (sendMessageToClient(fd, "You are not authorized to use this command"));	
		it->second->showMessage(clients[fd]->getNickname(), "[NOTICE] " + getMessage(tokens));
	}
	else
		return (sendMessageToClient(fd, "This command can only be used for channels"));
}
