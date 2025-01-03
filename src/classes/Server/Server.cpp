#include "Server.hpp"

void parseHost(istringstream &serverStream, Server &server);
void parseLocation(istringstream &serverStream, Server &server);
void parseServerName(istringstream &serverStream, Server &server);
void parseErrorPages(istringstream &serverStream, Server &server);
void parseClientMaxBodySize(istringstream &serverStream, Server &server);
void duplicateServerDirective(Server &server, const string &directive);
void initServerDirectiveStatus(Server &server);

Server::Server () : host (""), port (0), server_name (""), client_max_body_size (0), routes (vector<Route>()), isPortSet (false)
{
  error_pages = map<int, string>();
  memset (&adress, 0, sizeof (adress));
  initServerDirectiveStatus(*this);
  errorPagesMap = map<int, string>();
}

Server::Server (const Server &src) 
	:	host (src.host), 
		port (src.port), 
		server_name (src.server_name),
    	client_max_body_size (src.client_max_body_size) {
			  error_pages = src.error_pages;
			  routes = src.routes;
		}

Server::~Server () {}

void
Server::parseServerConfig (const string &line, Server &server)
{
  istringstream serverStream (line);
  string directiveName;
  serverStream >> directiveName;

  lineTreatment(directiveName);

  validServerDirective(directiveName);
  duplicateServerDirective(server, directiveName);
  if (directiveName == "host")
	parseHost(serverStream, server);
  else if (directiveName == "listen")
    parseLocation(serverStream, server);
  else if (directiveName == "server_name")
    parseServerName(serverStream, server);
  else if (directiveName == "error_page")
    parseErrorPages(serverStream, server);
  else if (directiveName == "client_max_body_size")
    parseClientMaxBodySize(serverStream, server);
}

void
Server::init ()
{
  int directiveValue = 1;

  server_fd = socket (AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
    FATAL_ERROR ("Error creating server's socket");
  setsockopt (server_fd, SOL_SOCKET, SO_REUSEADDR, &directiveValue, sizeof (int));
  memset (&adress, 0, sizeof (adress));
  adress.sin_family = AF_INET;
  adress.sin_addr.s_addr = inet_addr (host.c_str ());
  if (adress.sin_addr.s_addr == (in_addr_t)-1)
    FATAL_ERROR ("Error converting ip address");
  adress.sin_port = htons (port);
  if (bind (server_fd, (struct sockaddr *)&adress, sizeof (adress)) < 0)
    FATAL_ERROR ("Error binding server's socket");
  if (listen (server_fd, QUEUED_CONNECTIONS) < 0)
    FATAL_ERROR ("Error listening on server's socket");
}

void
Server::validServerDirective (const string &directive)
{
	static const char *validDirectives[] = {
		"host", "listen", "server_name", "error_page",
		"client_max_body_size", "location", "}",
		"{", "return", "server"
		};

	for (size_t i = 0; i < sizeof(validDirectives) / sizeof(validDirectives[0]); i++)
		if (directive == validDirectives[i])
			return;
	throw ConfigParserException("Error: invalid directive in server");
}

void
Server::checkServerValues(Server &server)
{
	if (server.getHost().empty())
		throw ConfigParserException("Error: missing host directive");
	if (server.getIsPortSet() == false)
		throw ConfigParserException("Error: missing listen directive");
	if (server.getServerName().empty())
		throw ConfigParserException("Error: missing server_name directive");
	if (server.routes.empty())
		throw ConfigParserException("Error: missing location directive");
	if (server.getClientMaxBodySize() == 0)
		throw ConfigParserException("Error: missing client_max_body_size directive");
}
void
Server::validatePort (int directiveValue)
{
  if (directiveValue < 0 || directiveValue > 65535)
		throw ConfigParserException("Error: invalid port number");
}

void
initServerDirectiveStatus(Server &server)
{
	static const char *validDirectives[] = {
		"host", "listen", "server_name",
		"client_max_body_size", "location", "}",
		"{", "server"
		};

	for (size_t i = 0; i < sizeof(validDirectives) / sizeof(validDirectives[0]); i++)
		server.directiveStatus[validDirectives[i]] = false;
}

void
duplicateErrorPageDirective(Server &server, const string &errorFile, int errorCode)
{
	for (map<int, string>::iterator it = server.errorPagesMap.begin(); it != server.errorPagesMap.end(); it++)
		if (it->first == errorCode)
			throw ConfigParserException("Error: duplicate error_page directive");
	server.errorPagesMap[errorCode] = errorFile;
}

void
duplicateServerDirective(Server &server, const string &directive)
{
	if (directive == "error_page")
		return ;
	if (server.directiveStatus[directive])
		throw ConfigParserException("Error: duplicate directive in server: " + directive);
	server.directiveStatus[directive] = true;
}


void
Server::addRoute (Route route)
{
	for (vector<Route>::iterator it = routes.begin(); it != routes.end(); it++)
		if (it->getPath() == route.getPath())
			throw ConfigParserException("Error: duplicate location directive");
  routes.push_back (route);
}

void
parseLocation(istringstream &serverStream, Server &server)
{
	int directiveValue;

	if (!(serverStream >> directiveValue))
		throw ConfigParserException("Error: invalid numeric value for listen");
	server.validatePort (directiveValue);
	server.setIsPortSet (true);
    server.setPort (static_cast<uint16_t>(directiveValue));
}

void
parseHost(istringstream &serverStream, Server &server)
{
	string host;

	serverStream >> host;
	lineTreatment(host);
	server.setHost(host);
}

void
parseServerName(istringstream &serverStream, Server &server)
{
	string server_name;

	serverStream >> server_name;
	lineTreatment(server_name);
	server.setServerName(server_name);
}

void
parseErrorPages(istringstream &serverStream, Server &server)
{
	int errorCode;
	string errorFile;

	serverStream >> errorCode >> errorFile;
	lineTreatment(errorFile);
	duplicateErrorPageDirective(server, errorFile, errorCode);
	server.error_pages[errorCode] = errorFile;
}

void
parseClientMaxBodySize(istringstream &serverStream, Server &server)
{
    size_t directiveValue;
    string remaining;

    if (!(serverStream >> directiveValue)) {
        throw ConfigParserException("Error: invalid numeric value for client_max_body_size");
    }
    if (serverStream >> remaining) {
        throw ConfigParserException("Error: unexpected characters in client_max_body_size directive");
    }
    server.setClientMaxBodySize(directiveValue);
}
