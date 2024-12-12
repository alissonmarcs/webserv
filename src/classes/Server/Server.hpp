#ifndef SERVER_H
# define SERVER_H

# include "Utils.hpp"
# include "Route.hpp"

class Server
{
  public:
	Server();
	~Server();
	Server(const Server &src);
	void init();

	string host;
	string server_name;
	map<int, string> error_pages;
	size_t client_max_body_size;
	vector<Route> routes;
	int server_fd;
	string ip;
	uint16_t port;
	struct sockaddr_in adress;

	void addRoute(const Route &route);
};

#endif