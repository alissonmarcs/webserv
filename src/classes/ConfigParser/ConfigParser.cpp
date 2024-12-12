#include "ConfigParser.hpp"

ConfigParser::ConfigParser()
{
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

void ConfigParser::parseServerConfig(std::ifstream &file)
{
	(void)file;
}

void ConfigParser::parseRouteConfig(Server &server, std::ifstream &file)
{
	(void)server;
	(void)file;
}

void ConfigParser::loadConfig(const string &path)
{
	(void)path;
}

