#include "ConfigParser.hpp"
#include "Error.hpp"

ConfigParser::ConfigParser (const string &config)
{
	istringstream stream (config);
	string lineStream;
	int nestingLevel = 0;
	Server *activeServer = NULL;
	bool serverFound = false;

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
		{
			if (lineStream.find("server") != string::npos)
				serverFound = true;
			else
				throw ConfigParserException("Error: missing server directive");
		}
		if (serverFound && lineStream.find ("{") != string::npos)
		{
			nestingLevel++;
			if (nestingLevel == 1)
				activeServer = new Server ();
		}
		else if (lineStream.find ("}") != string::npos)
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
		if (nestingLevel < 0)
			throw ConfigParserException ("Error: brackets mismatch");
		if (nestingLevel == 1)
			activeServer->parseServerConfig (lineStream, *activeServer);
		else if (nestingLevel == 2 && activeServer && lineStream.find("location") != string::npos)
		{
			Route route;
			nestingLevel += route.parseRouteConfig(lineStream, stream);
			activeServer->addRoute(route);
		}
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
