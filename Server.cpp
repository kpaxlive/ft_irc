#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

#include "Server.hpp"

#include <sstream>

std::vector<std::string> split(const std::string &str, char delimiter)
{
	std::vector<std::string> strings;
    std::istringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter))
        strings.push_back(token);
	return strings;
}

Server::Server(std::string port, std::string _password)
{
	quited = false;
	password = _password;
	clientCount = 0;
	serverFD = createSocket(port);
	pollfd server = {serverFD, POLLIN, 0};
	pollfds.push_back(server);

	commands["NOTICE"]	=	&Server::notice;
	commands["KICK"]	=	&Server::kick;
	commands["WHO"]		=	&Server::who;
	commands["MEMBER"]	=	&Server::member;
	commands["LIST"]	=	&Server::list;
	commands["QUIT"] 	=	&Server::quit;
	commands["NICK"] 	=	&Server::nick;
	commands["PASS"] 	=	&Server::pass;
	commands["USER"]	=	&Server::user;
	commands["PRIVMSG"]	=	&Server::privmsg;
	commands["JOIN"]	=	&Server::join;
	commands["PART"]	=	&Server::part;
	
	std::cout << "Listening..." << std::endl;
}

Server::~Server() {}

struct sockaddr_in Server::addressFill(std::string port)
{
	struct sockaddr_in address = {};
	bzero((char *) &address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(std::stoi(port)); // converting short;
	address.sin_addr.s_addr = INADDR_ANY;
	memset(address.sin_zero, '\0', sizeof(address.sin_zero));  // sin_zero[8] is a char * so we get rid of trash data.
	return address;
}

void Server::sendMessageToClient(int fd, std::string message)
{
	message = "Server: " + message + "\r\n";
	send(fd, message.c_str(), message.size(), 0);
}

int Server::createSocket(std::string port)
{
	// Socket procceses
	int newSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (newSocket < 0)
	{
		std::cerr << "**** Client Creating Socket: Failed ****" << std::endl;
		exit(1);
	}
	int val = 1;
	int socketOptions = setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)); // If socket closed, this function makes it reusable again faster.
	if (socketOptions)
	{
		std::cerr << "**** Client Setting Socket Options: Failed ****" << std::endl;
		exit(1);
	}
	if (fcntl(newSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "**** Client Setting Socket NON-BLOCKING: Failed ****" << std::endl;
		exit(1);
	}

	serverAddress = addressFill(port);
	if(bind(newSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
	{
		std::cerr << "**** Client Binding Socket: Failed ****" << std::endl;
		exit(1);
	}
	if(listen(newSocket, MAX_CLIENTS) < 0)
	{
		std::cerr << "**** Client Listening Socket: Failed ****" << std::endl;
		exit(1);
	}
	return(newSocket);
}

void ft_log(const std::string &message)
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	std::string str(buffer);
	(void)message;
	std::cout << "[" << str << "] " << message << std::endl;
}


void Server::processMessage(int fd, std::string prompt)
{
	std::vector<std::string> tokens;
	tokens = split(prompt, ' ');

	if (commands.find(tokens[0]) == commands.end())
		return (sendMessageToClient(fd, "No command found"));
	(this->*commands[tokens[0]])(fd, tokens);
}

void Server::recieveMessage(int fd)
{
	std::string message;

	char buffer[100];
	bzero(buffer, 100);

	while (!std::strstr(buffer, "\n"))
	{
		bzero(buffer, 100);
		if (recv(fd, buffer, 100, 0) < 0)
		{
			std::cerr << "**** Reading Message From Client: Failed ****" << std::endl;
			return;
		} 
		message.append(buffer);
	}
	int i =0;
	while((message[i] == ' ' || message[i] == '\r' || message[i] == '\t' || message[i] == '\v' || message[i] == '\f') && message[i] != '\n')
		i++;
	message = message.substr(i);
	if (message.size() == 1 || (message.size() - i) == 1)
		return;
	message.erase(message.find_last_not_of(" \n\r\t\v\f")+1);
	ft_log(clients[fd]->getNickname() + " " + message);
	processMessage(fd, message);
}

void Server::onClientConnect()
{
	int fd;
	sockaddr_in s_address = {};
	socklen_t s_size = sizeof(s_address);

	if ((fd = accept(serverFD, (sockaddr *) &s_address, &s_size)) < 0)
		{std::cerr << "**** Accepting New Client: Failed ****" << std::endl; return;}

	pollfd pollfd = {fd, POLLIN, 0};
	pollfds.push_back(pollfd);

	Client *newClient = new Client(fd);
	clients[fd] = newClient;
	if (getnameinfo((struct sockaddr *) &s_address, sizeof(s_address), clients[fd]->getHostname(), 1024, NULL, 0, NI_NUMERICSERV) != 0)
		{std::cerr << "Error: failed to get client hostname!\n"; return ;}

	std::cout << "Client #" << ++clientCount << " connected" << std::endl;
}

void Server::onClientDisconnect(int clientFD, std::vector<std::string> tokens)
{
	std::vector<std::string> userChannels = clients[clientFD]->getChannels();
	for (size_t i = 0; i < userChannels.size(); i++)
	{
		std::vector<std::string> newTokens;
		newTokens.push_back("QUIT");
		newTokens.push_back(userChannels[i]);
		if (tokens.empty() == false)
			newTokens.insert(newTokens.end(), tokens.begin() + 1, tokens.end());

		part(clientFD, newTokens);
	}
	for (std::vector<pollfd>::iterator it = pollfds.begin(); it != pollfds.end(); it++)
	{
		if (it->fd == clientFD)
		{
			std::string user = std::to_string(it->fd - 3) + "(" + std::string(hostname) + ")";
			std::cout << "Client " << user << " has left the server" << std::endl;
			close(it->fd);
			pollfds.erase(it);
			break;
		}
	}
	delete clients[clientFD];
	clients.erase(clientFD);
	quited = true;
}

void Server::launch()
{
	while (1)
	{
		if (poll(pollfds.begin().base(), pollfds.size(), -1) < 0)
		{
			std::cerr << "**** Polling: Failed ****" << std::endl;
			return;
		}
		std::vector<pollfd>::iterator it;
		for(it = pollfds.begin(); it != pollfds.end(); it++)
		{
			if ((it->revents & POLLIN) == 0)
				continue;
			if ((it->revents & POLLHUP) == POLLHUP)
			{
				onClientDisconnect(it->fd, std::vector<std::string>());
				break;
			}
			if ((it->revents & POLLIN) == POLLIN)
			{
				if (it->fd == serverFD)
				{
					onClientConnect();
					break;
				}
				recieveMessage(it->fd);
				if (quited)
					break;
			}
		}
	}
}
