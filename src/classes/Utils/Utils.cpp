#include "Utils.hpp"

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

char * getClientIp(struct sockaddr_in * client_addr)
{
    static char ip[INET_ADDRSTRLEN];

    bzero(ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &client_addr->sin_addr, ip, INET_ADDRSTRLEN);
    return (ip);
}