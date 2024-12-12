#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const string &path)
{
	istringstream stream(readFileAsString(path));

	string line;
	int bracketCount = 0;

	while (getline(stream, line))
	{
		line = trim(removeComments(line));
		if (line.empty())
			continue;
		if (line.find("{") != string::npos)
			bracketCount++;
		if (line.find("}") != string::npos)
			bracketCount--;
		if (bracketCount == 0)
			parseGlobalConfig(line);
		else if (bracketCount == 1)
			parseServerConfig(stream);
		else if (bracketCount == 2)
			parseRouteConfig(servers.back(), stream);
	}


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

void ConfigParser::parseServerConfig(istringstream &file)
{
	(void)file;
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