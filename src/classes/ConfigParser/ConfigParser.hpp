
#ifndef CONFIGPARSER_HPP_
#define CONFIGPARSER_HPP_

#include "Server.hpp"
#include "Utils.hpp"

// Class declaration
class ConfigParser
{
public:
  ConfigParser (const string &config);
  ConfigParser &operator= (const ConfigParser &rhs);

  vector<Server> servers;
  int parseRouteConfig (Server &server, const string &line, istringstream &stream);
  void loadConfig (const string &config);

};

#endif // CONFIGPARSER_HPP_
