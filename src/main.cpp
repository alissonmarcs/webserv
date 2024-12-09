#include "Utils.hpp"
#include "ServerManager.hpp"

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void populateServers(ServerManager &Manager)
{
	Manager.servers.push_back(Server());
	Manager.servers[0].ip = "127.0.0.1";
	Manager.servers[0].port = 9000;
}

int main(int argc, char **argv)
{
	ServerManager Manager;	

	populateServers(Manager);
	Manager.initServers();
	Manager.mainLoop();
}