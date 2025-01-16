#include "Utils.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

void
fatal_error (const char *msg, const char *file, long line)
{
  std::fprintf (stderr, "Fatal error: %s in %s:%ld: %s\n", msg, file, line,
                std::strerror (errno));
  std::exit (EXIT_FAILURE);
}

void
non_fatal_error (const char *msg, const char *file, long line)
{
  std::fprintf (stderr, "Non fatal error: %s in %s:%ld: %s\n", msg, file, line,
                std::strerror (errno));
}

void
logger (const char *client, const char * msg, const char *file, long line)
{
  cout << "[" << file << ":" << line << "] [Client " << client << "]: " << msg << endl;
}

bool
isValidIp (string ip)
{
  struct sockaddr_in adress;

  memset (&adress, 0, sizeof (adress));
  return (inet_pton (AF_INET, ip.c_str (), &adress.sin_addr) != 0);
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

void
lowercase (string &str)
{
  for (size_t i = 0; i < str.size (); i++)
    str[i] = tolower (str[i]);
}

void
trim (std::string &str)
{
  const char *whiteSpace = " \t\n\r\f\v";
  str.erase (str.find_last_not_of (whiteSpace) + 1);
  str.erase (0, str.find_first_not_of (whiteSpace));
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
  trim (line);
  removeComments (line);
}

void
trimBraces (string &line)
{
  size_t pos = line.find ("{");
  if (pos != string::npos)
	line = line.substr (0, pos);
}

bool
containsLetter(const string &str)
{
  for (size_t i = 0; i < str.size(); i++)
	if (isalpha(str[i]))
	  return (true);
  return (false);
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
		cout << PINK << "Index files : " << routes[i].getIndex() << "\n";
    cout << PINK << "Folder to search inside : " << routes[i].getFolderToSearchInside() << "\n";
	}
}

long	get_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

const char* getIpString(const struct sockaddr_in * addr)
{
    return inet_ntoa(addr->sin_addr);
}

string to_string (short status_code)
{
  stringstream ss;
  ss << status_code;
  return ss.str ();
}

string to_string (size_t status_code)
{
  stringstream ss;
  ss << status_code;
  return ss.str ();
}

const char * getStatusText(short status_code)
{
  switch (status_code)
    {
    case 200:
      return "OK";
    case 201:
      return "Created";
    case 202:
      return "Accepted";
    case 204:
      return "No Content";
    case 301:
      return "Moved Permanently";
    case 302:
      return "Found";
    case 303:
      return "See Other";
    case 304:
      return "Not Modified";
    case 400:
      return "Bad Request";
    case 401:
      return "Unauthorized";
    case 403:
      return "Forbidden";
    case 404:
      return "Not Found";
    case 405:
      return "Method Not Allowed";
    case 408:
      return "Request Timeout";
    case 413:
      return "Payload Too Large";
    case 500:
      return "Internal Server Error";
    case 501:
      return "Not Implemented";
    case 505:
      return "HTTP Version Not Supported";
    default:
      return "Unknown";
    }
}

const char * getCurrentHttpDate()
{
  static char buf[1000] = {0};

  time_t now = time(0);
  struct tm tm = *gmtime(&now);
  strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
  return buf;
}

size_t getDifferIndex(string &one, string &two)
{
  size_t i = 0;
  size_t size = one.size() < two.size() ? one.size() : two.size();

  for (; i < size; i++)
    if (one[i] != two[i])
      return (i);
  return (i);
}