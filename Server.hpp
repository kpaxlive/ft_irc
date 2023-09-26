#pragma once

#include <netinet/in.h>
#include <iostream>
#include <poll.h>
#include <vector>
#include <map>
#include "Client.hpp"
#include "Channel.hpp"

#define MAX_CLIENTS 15

class Server
{
	private:
		std::string password;

		std::map<std::string, void(Server::*)(int, std::vector<std::string>)> commands;
		std::vector<pollfd> pollfds;
		std::map<int, Client *> clients;
		std::map<std::string, Channel *> channels;
		struct sockaddr_in serverAddress;
		bool quited;
		char hostname[1024];

		int serverFD;
		int acceptFD;
		int clientCount;
	
		int createSocket(std::string port);
	
		struct sockaddr_in addressFill(std::string port);
	
		void sendMessageToClient(int fd, std::string message);


		void onClientConnect();
		void onClientDisconnect(int clientFD, std::vector<std::string> tokens);


		void recieveMessage(int fd);
		void processMessage(int fd, std::string prompt);

        void quit(int fd, std::vector<std::string> tokens);
        void nick(int fd, std::vector<std::string> tokens);
        void pass(int fd, std::vector<std::string> tokens);
        void user(int fd, std::vector<std::string> tokens);
        void notice(int fd, std::vector<std::string> tokens);
        void kick(int fd, std::vector<std::string> tokens);
        void who(int fd, std::vector<std::string> tokens);
        void privmsg(int fd, std::vector<std::string> tokens);
        void join(int fd, std::vector<std::string> tokens);
        void part(int fd, std::vector<std::string> tokens);
        void list(int fd, std::vector<std::string> tokens);
		void member(int fd, std::vector<std::string> tokens);

	public:
		Server(std::string port, std::string password);
		~Server();

		void launch();
		void getClients();
};

std::vector<std::string>	split(const std::string &str, std::string delimiter);
std::string					getMessage(std::vector<std::string> tokens);
