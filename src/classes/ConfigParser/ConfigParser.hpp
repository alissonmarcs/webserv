
#ifndef CONFIGPARSER_HPP_
# define CONFIGPARSER_HPP_

# include "Server.hpp"
# include "Utils.hpp"

// Class declaration
class ConfigParser
{
  public:
	ConfigParser();
	ConfigParser &operator=(const ConfigParser &rhs);
	
	vector<Server> servers;
	void parseGlobalConfig(const string &line);
	void parseServerConfig(std::ifstream &file);
	void parseRouteConfig(Server &server, std::ifstream &file);

	void loadConfig(const string &path);
};

#endif // CONFIGPARSER_HPP_
