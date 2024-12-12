#include "Utils.hpp"
#include <cstdio>
#include <cstdlib>

void error(const char *msg, const char *file, long line)
{
	std::fprintf(stderr, "Fatal error: %s in %s:%ld: %s\n", msg, file, line, strerror(errno));
	std::exit(EXIT_FAILURE);
}

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