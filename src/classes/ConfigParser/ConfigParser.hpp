
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
  void loadConfig (const string &config);

  void thereIsServerDirective(const string& lineStream, bool& serverFound);
  void startServerBlock(Server*& activeServer);
  void endServerBlock(Server*& activeServer, bool& serverFound);
  void processLocation(Server*& activeServer, const string& lineStream, istringstream& stream);

private:
	int nestingLevel;
};

#endif
