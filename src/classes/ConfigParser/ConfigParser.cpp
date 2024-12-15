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

void ConfigParser::parseGlobalConfig(const string &line)
{
	(void)line;
}

void ConfigParser::parseServerConfig(const string &line, Server &server)
{
	if (line.find("listen") != string::npos)
	{
		istringstream iss(line);
		string directive;

		iss >> directive >> server.port;
	}
	if (line.find("host") != string::npos)
	{
		istringstream iss(line);
		string directive;

		iss >> directive >> server.host;
	}
	if (line.find("server_name") != string::npos)
	{
		istringstream iss(line);
		string directive;

		iss >> directive >> server.server_name;
	}
	if (line.find("error_page") != string::npos)
	{
		istringstream iss(line);
		string directive;
		int code;
		string path;

		iss >> directive >> code >> path;
		server.error_pages[code] = path;
	}
	if (line.find("client_max_body_size") != string::npos)
	{
		istringstream iss(line);
		string directive;

		iss >> directive >> server.client_max_body_size;
	}
}

void ConfigParser::parseRouteConfig(Server &server, std::ifstream &file)
{
	(void)server;
	(void)file;
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