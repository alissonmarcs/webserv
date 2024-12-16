#ifndef SERVER_H
# define SERVER_H

# include "Route.hpp"
# include "Utils.hpp"

class Server
{
  public:
	Server();
	~Server();
	Server(const Server &other);
	Server &operator=(const Server &other)
	{
		if (this != &other)
		{
			host = other.host;
			port = other.port;
			server_name = other.server_name;
			error_pages = other.error_pages;
			client_max_body_size = other.client_max_body_size;
		}
		return (*this);
	}
	
	void init();

	string host;
	uint16_t port;
	string server_name;
	map<int, string> error_pages;
	size_t client_max_body_size;
	vector<Route> routes;
	int server_fd;
	string ip;
	struct sockaddr_in adress;

	void addRoute(const Route &route);
};

#endif