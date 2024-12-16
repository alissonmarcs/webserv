#include "ConfigParser.hpp"
#include "Error.hpp"

string removeComments(string &line);
string trim(const string &str);

ConfigParser::ConfigParser(const string &config)
{
	istringstream stream(config);
	string line;
	int bracketCount = 0;
	Server *activeServer = NULL;
	
	while (getline(stream, line))
	{
		line = trim(removeComments(line));
		if (line.empty())
			continue;
		if (line.find("{") != string::npos)
		{
			bracketCount++;
			if (bracketCount == 1)
				activeServer = new Server();
		}
		else if (line.find("}") != string::npos)
		{
			bracketCount--;
			if (bracketCount == 0)
			{
				servers.push_back(*activeServer);
				delete activeServer;
				activeServer = NULL;
			}
		}
		if (bracketCount < 0)
			throw ConfigParserException("Error: brackets mismatch");
		if (bracketCount == 1)
			parseServerConfig(line, *activeServer);
	}
	if (bracketCount != 0)
		throw ConfigParserException("Error: brackets mismatch");
}

ConfigParser &ConfigParser::operator=(const ConfigParser &rhs)
{
	(void)rhs;
	return (*this);
}

void ConfigParser::parseServerConfig(const string &line, Server &server)
{
	istringstream iss(line);
	string directive, value;
	iss >> directive;

	if (directive == "host")
	{
		iss >> value;
		removeSemicolon(value);
		server.setHost(value);
	}
	else if (directive == "listen")
	{
		u_int16_t value;	

		iss >> value;
		server.setPort(value);
	}
	else if (directive == "server_name")
	{
		iss >> value;
		removeSemicolon(value);
		server.setServerName(value);
	}
	else if (directive == "error_page")
	{
		int code;
		string path;

		iss >> code >> path;
		removeSemicolon(path);
		server.error_pages[code] = path;
	}
	else if (directive == "client_max_body_size")
	{
		size_t value;

		iss >> value;
		server.setClientMaxBodySize(value);	
	}
}

void ConfigParser::parseRouteConfig(Server &server, const string &line)
{
	(void)server;
	(void)line;
}

string removeComments(string &line)
{
	size_t pos = line.find("#");
	if (pos != string::npos)
		line = line.substr(0, pos);
	return (line);
}

string trim(const string &str)
{
	size_t untilFirstChar = str.find_first_not_of(" \t");
	size_t untilLastChar = str.find_last_not_of(" \t");
	return ((untilFirstChar == string::npos || untilLastChar == string::npos) ? "" : str.substr(untilFirstChar, untilLastChar - untilFirstChar + 1));
}

void ConfigParser::removeSemicolon(string &line)
{
	size_t pos = line.find(";");
	if (pos != string::npos)
		line = line.substr(0, pos);
}