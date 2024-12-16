#include "Utils.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

void
error (const char *msg, const char *file, long line)
{
  std::fprintf (stderr, "Fatal error: %s in %s:%ld: %s\n", msg, file, line,
                std::strerror (errno));
  std::exit (EXIT_FAILURE);
}

bool
isValidIp (string ip)
{
  struct sockaddr_in adress;

  memset (&adress, 0, sizeof (adress));
  if (inet_pton (AF_INET, ip.c_str (), &adress.sin_addr) == 1)
    return (true);
  return (false);
}

string
getClientIp (struct sockaddr_in *client_addr)
{
  char tmp[INET_ADDRSTRLEN];
  stringstream ip;

  memset (tmp, 0, INET_ADDRSTRLEN);
  inet_ntop (AF_INET, &client_addr->sin_addr, tmp, INET_ADDRSTRLEN);
  ip << tmp << ":" << ntohs (client_addr->sin_port);
  return (ip.str ());
}

string readFileAsString(const string &path)
{
	ifstream file(path.c_str());

	if (!file.is_open())
		FATAL_ERROR("Error: Could not open file");
	
	stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return (buffer.str());
}

//print server info
void printServerInfo(ServerManager &Manager)
{
  vector<Server> servers = Manager.getServersRef();
  size_t size = servers.size();

	for (size_t i = 0; i < size; i++)
	{
		cout << "Server " << i << ":\n";
		cout << "Host: " << servers[i].getHost() << "\n";
		cout << "Port: " << servers[i].getPort() << "\n";
		cout << "Server name: " << servers[i].getServerName() << "\n";
		cout << "Client max body size: " << servers[i].getClientMaxBodySize() << "\n";
    cout << "Error pages count: " << servers[i].error_pages.size() << "\n";
    cout << "Error pages:\n";

    map<int, string>::iterator start = servers[i].error_pages.begin();
    map<int, string>::iterator end = servers[i].error_pages.end();
    for (; start != end; start++)
      cout << "\tError code: " << start->first << ", file: " << start->second << "\n";
	}
}