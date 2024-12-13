
#ifndef CONFIGPARSER_HPP_
# define CONFIGPARSER_HPP_

# include "Server.hpp"
# include "Utils.hpp"

// Class declaration
class ConfigParser
{
  public:
	ConfigParser(const string &config);
	ConfigParser &operator=(const ConfigParser &rhs);
	
	vector<Server> servers;
	void parseGlobalConfig(const string &line);
	void parseServerConfig(const string &line, Server &server);
	void parseRouteConfig(Server &server, std::ifstream &file);

	void loadConfig(const string &path);

	class ConfigParserException : public std::exception
	{
	  public:
		ConfigParserException(const string &msg) : _msg(msg) {}
		virtual const char *what() const throw() { return _msg.c_str(); }
		~ConfigParserException() throw() {}

	  private:
		string _msg;
	};
};

#endif // CONFIGPARSER_HPP_
