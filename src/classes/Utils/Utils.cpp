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

string
readFileAsString (const string &path)
{
  ifstream file (path.c_str ());

  if (!file.is_open ())
    FATAL_ERROR ("Error: Could not open file");

  stringstream buffer;
  buffer << file.rdbuf ();
  file.close ();
  return (buffer.str ());
}


void
removeSemicolon (string &line)
{
  size_t pos = line.find (";");
  if (pos != string::npos)
    line = line.substr (0, pos);
}

string
trim (const string &str)
{
  size_t untilFirstChar = str.find_first_not_of (" \t");
  size_t untilLastChar = str.find_last_not_of (" \t");
  return (
      (untilFirstChar == string::npos || untilLastChar == string::npos)
          ? ""
          : str.substr (untilFirstChar, untilLastChar - untilFirstChar + 1));
}

string
removeComments (string &line)
{
  size_t pos = line.find ("#");
  if (pos != string::npos)
    line = line.substr (0, pos);
  return (line);
}

void
lineTreatment (string &line)
{
  removeSemicolon (line);
  line = trim (line);
  removeComments (line);
}

// print server info
void
printServerInfo (ServerManager &Manager)
{
  vector<Server> servers = Manager.getServersRef ();
  size_t size = servers.size ();

  for (size_t i = 0; i < size; i++)
    {
      cout << BLUE << "-------------------Server------------- " << i << RESET << ":\n";
      cout << PINK << "Host: " << GREEN << servers[i].getHost () << RESET << "\n";
      cout << PINK << "Port: " << GREEN << servers[i].getPort () << RESET << "\n";
      cout << PINK << "Server name: " << GREEN << servers[i].getServerName () << RESET << "\n";
      cout << PINK << "Client max body size: " << GREEN << servers[i].getClientMaxBodySize ()
           << RESET << "\n";
      cout << PINK << "Error pages count: " << GREEN << servers[i].error_pages.size () << RESET << "\n";
      cout << PINK << "Error pages :\n";

      map<int, string>::iterator start = servers[i].error_pages.begin ();
      map<int, string>::iterator end = servers[i].error_pages.end ();
      for (; start != end; start++)
        cout << PINK << "\tError code: " << RED << start->first << RESET << ", file: " << PURPLE << start->second
             << RESET << "\n";
	  printRoutesInfo(servers[i]);
    }
}

void
printRoutesInfo(Server &server)
{
	vector<Route> routes = server.getRoutes();
	size_t size = routes.size();

	for (size_t i = 0; i < size; i++)
	{
		cout << BLUE << "-------------------Route------------- " << i << RESET << ":\n";
		cout << PINK << "Path: " << GREEN << routes[i].getPath() << RESET << "\n";
		cout << PINK << "Root: " << GREEN << routes[i].getRoot() << RESET << "\n";
		cout << PINK << "Autoindex: " << GREEN << routes[i].getAutoindex() << RESET << "\n";
		cout << PINK << "Allowed methods count: " << GREEN << routes[i].getAllowedMethods().size() << RESET << "\n";
		cout << PINK << "Allowed methods :\n";
		vector<string> methods = routes[i].getAllowedMethods();
		size_t size = methods.size();
		for (size_t j = 0; j < size; j++)
			cout << PINK << "\tMethod: " << RED << methods[j] << RESET << "\n";
		cout << PINK << "Redirect: " << GREEN << routes[i].getRedirect() << RESET << "\n";
		cout << PINK << "Default file: " << GREEN << routes[i].getDefaultFile() << RESET << "\n";
		cout << PINK << "Cgi ext: " << GREEN << routes[i].getCgiExt() << RESET << "\n";
		cout << PINK << "Upload store: " << GREEN << routes[i].getUploadStore() << RESET << "\n";
	}
}