#include "Utils.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

void error(const char *msg, const char *file, long line)
{
	std::fprintf(stderr, "Fatal error: %s in %s:%ld: %s\n", msg, file, line, std::strerror(errno));
	std::exit(EXIT_FAILURE);
}

bool isValidIp(string ip)
{
	struct sockaddr_in adress;

	memset (&adress, 0, sizeof(adress));
	if (inet_pton(AF_INET, ip.c_str(), &adress.sin_addr) == 1)
		return (true);
	return (false);
}

string getClientIp(struct sockaddr_in * client_addr)
{
    char tmp[INET_ADDRSTRLEN];
	stringstream ip;

    memset (tmp, 0, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &client_addr->sin_addr, tmp, INET_ADDRSTRLEN);
	ip << tmp << ":" << ntohs(client_addr->sin_port);
    return (ip.str());
}