#include "ConfigParser.hpp"
#include "Error.hpp"

ConfigParser::ConfigParser ()
{
}

ConfigParser::~ConfigParser ()
{
}

void
ConfigParser::init (string & config)
{
	istringstream stream (config);
	string lineStream;
	Server *activeServer = NULL;
	bool serverFound = false;
	nestingLevel = 0;

	while (getline (stream, lineStream))
	{
		lineTreatment (lineStream);
		if (lineStream.empty ())
			continue;

		istringstream lineWords (lineStream);
		string firstWord;
		lineWords >> firstWord;

		if (serverFound && firstWord == "server")
			throw ConfigParserException("Error: server directive inside server block");
		if (nestingLevel == 0 && !serverFound)
			thereIsServerDirective(lineStream, serverFound);
		if (serverFound && lineStream.find ("{") != string::npos)
			startServerBlock(activeServer);
		else if (lineStream.find ("}") != string::npos)
			endServerBlock(activeServer, serverFound);
		if (activeServer && lineStream.find("location") != string::npos)
			processLocation(activeServer, lineStream, stream);
		else if (nestingLevel == 1)
			activeServer->parseServerConfig (lineStream, *activeServer);
	}
	if (nestingLevel != 0)
		throw ConfigParserException ("Error: brackets mismatch");
}

ConfigParser &
ConfigParser::operator= (const ConfigParser &rhs)
{
	(void)rhs;
	return (*this);
}

void
ConfigParser::startServerBlock(Server*& activeServer)
{
	nestingLevel++;
	if (nestingLevel == 1)
		activeServer = new Server ();
}

void
ConfigParser::endServerBlock(Server*& activeServer, bool& serverFound)
{
	nestingLevel--;
	if (nestingLevel == 0)
	{
		activeServer->checkServerValues(*activeServer);
		servers.push_back (*activeServer);
		delete activeServer;
		activeServer = NULL;
		serverFound = false;
	}
}

void
ConfigParser::processLocation(Server*& activeServer, const string& lineStream, istringstream& stream)
{
	Route route;
	route.parseRouteConfig(lineStream, stream, nestingLevel);
	activeServer->addRoute(route);
	activeServer->routes_redirect[route.getPath()] = route.getRedirect();

	string current = route.getRedirect();
	set<string> visited;

	while (!current.empty()) {
		if (visited.find(current) != visited.end())
			throw ConfigParserException("Error: redirect loop detected");

		visited.insert(current);

		if (activeServer->routes_redirect.find(current) == activeServer->routes_redirect.end())
			break;

		current = activeServer->routes_redirect[current];
	}

	stream.clear();
	stream.seekg(0, ios::cur);
}

void
ConfigParser::thereIsServerDirective(const string& lineStream, bool& serverFound)
{
	if (lineStream.find("server") != string::npos)
				serverFound = true;
	else
		throw ConfigParserException("Error: missing server directive");
}
