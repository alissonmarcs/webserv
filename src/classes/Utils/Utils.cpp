#include "Utils.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"

bool isValidIp(string ip)
{
	struct sockaddr_in adress;

	bzero(&adress, sizeof(adress));
	if (inet_pton(AF_INET, ip.c_str(), &adress.sin_addr) == 1)
		return (true);
	return (false);
}

void fatalError(string msg)
{
	perror(msg.c_str());
	exit(EXIT_FAILURE);
}

string getClientIp(struct sockaddr_in * client_addr)
{
    char tmp[INET_ADDRSTRLEN];
	stringstream ip;

    bzero(tmp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &client_addr->sin_addr, tmp, INET_ADDRSTRLEN);
	ip << tmp << ":" << ntohs(client_addr->sin_port);
    return (ip.str());
}

string readFileAsString(const string &path)
{
	ifstream file(path.c_str());

	if (!file.is_open())
		fatalError("Error: Could not open file");
	
	stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return (buffer.str());
}

//print server info
void printServerInfo(ServerManager &Manager)
{
	for (size_t i = 0; i < Manager.servers.size(); i++)
	{
		cout << "Server " << i << ":\n";
		cout << "Host: " << Manager.servers[i].host << "\n";
		cout << "Port: " << Manager.servers[i].port << "\n";
		cout << "Server name: " << Manager.servers[i].server_name << "\n";
		cout << "Client max body size: " << Manager.servers[i].client_max_body_size << "\n";
	}
}