#include <iostream>
#include "Server.hpp"

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		std::cout << "** Usage : ./irc <port> <password>" << std::endl;
		return 1;
	}
	
	Server server(argv[1], std::string(argv[2]));
	server.launch();
}
