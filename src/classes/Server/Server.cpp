#include "Server.hpp"

void parseHost(istringstream &serverStream, Server &server);
void parseLocation(istringstream &serverStream, Server &server);
void parseServerName(istringstream &serverStream, Server &server);
void parseErrorPages(istringstream &serverStream, Server &server);
void parseClientMaxBodySize(istringstream &serverStream, Server &server);
void duplicateServerDirective(Server &server, const string &directive);
void initServerDirectiveStatus(Server &server);

Server::Server () : host (""), port (0), server_name (""), client_max_body_size (0), routes (vector<Route>()), server_fd(-1), isPortSet (false)
{
  error_pages[BAD_REQUEST] = BAD_REQUEST_HTML;
  error_pages[UNAUTHORIZED] = UNAUTHORIZED_HTML;
  error_pages[FORBIDDEN] = FORBIDDEN_HTML;
  error_pages[NOT_FOUND] = NOT_FOUND_HTML;
  error_pages[METHOD_NOT_ALLOWED] = METHOD_NOT_ALLOWED_HTML;
  error_pages[REQUEST_TIMEOUT] = REQUEST_TIMEOUT_HTML;
  error_pages[PAYLOAD_TOO_LARGE] = PAYLOAD_TOO_LARGE_HTML;
  error_pages[INTERNAL_SERVER_ERROR] = INTERNAL_SERVER_ERROR_HTML;
  error_pages[NOT_IMPLEMENTED] = NOT_IMPLEMENTED_HTML;
  error_pages[HTTP_VERSION_NOT_SUPPORTED] = HTTP_VERSION_NOT_SUPPORTED_HTML;
  error_pages[CONFLIT] = REQUEST_CONFLIT_HTML;
  error_pages[UNSUPPORTED_MEDIA_TYPE] = UNSUPPORTED_MEDIA_TYPE_HTML;
  error_pages[GATEWAY_TIMEOUT] = GATEWAY_TIMEOUT_HTML;
  error_pages[BAD_GATEWAY] = BAD_GATEWAY_HTML;

  memset (&adress, 0, sizeof (adress));
  initServerDirectiveStatus(*this);
  errorPagesMap = map<int, string>();
}

Server::Server (const Server &src) 
	:	host (src.host), 
		port (src.port), 
		server_name (src.server_name),
    	client_max_body_size (src.client_max_body_size),
		server_fd(src.server_fd)
		{
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
	{
		string tmp = "Error: duplicate directive in server: " + directive;
		throw ConfigParserException(tmp.c_str());
	}
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
	string file;
	stringstream content;

	serverStream >> errorCode >> file;
	lineTreatment(file);
	duplicateErrorPageDirective(server, file, errorCode);
	ifstream fileStream(file.c_str());
	if (!fileStream.is_open())
		throw ConfigParserException("Error: file in error_page directive does not exist");
	content << fileStream.rdbuf();
	server.error_pages[errorCode] = content.str();
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
		if(remaining != "K" && remaining != "G" && remaining != "M")
			throw ConfigParserException("Error: invalid character");
		if(remaining == "K")
			directiveValue = directiveValue * 1000;
		if(remaining == "G")
			directiveValue = directiveValue *  1000000000;
		if(remaining == "M")
			directiveValue = directiveValue *  1000000;
    }
    server.setClientMaxBodySize(directiveValue);
}
