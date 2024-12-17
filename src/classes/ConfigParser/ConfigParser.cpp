#include "ConfigParser.hpp"
#include "Error.hpp"

ConfigParser::ConfigParser (const string &config)
{
  istringstream stream (config);
  string line;
  int bracketCount = 0;
  Server *activeServer = NULL;

  while (getline (stream, line))
    {
      line = trim (removeComments (line));
      if (line.empty ())
        continue;
      if (line.find ("{") != string::npos)
        {
          bracketCount++;
          if (bracketCount == 1)
            activeServer = new Server ();
        }
      else if (line.find ("}") != string::npos)
        {
          bracketCount--;
          if (bracketCount == 0)
            {
              servers.push_back (*activeServer);
              delete activeServer;
              activeServer = NULL;
            }
        }
      if (bracketCount < 0)
        throw ConfigParserException ("Error: brackets mismatch");
      if (bracketCount == 1)
        activeServer->parseServerConfig (line, *activeServer);
	  else if (bracketCount == 2 && activeServer && line.find("location") != string::npos)
		{
			Route route;
			bracketCount += route.parseRouteConfig(line, stream);
			activeServer->addRoute(route);
		}
    }
  if (bracketCount != 0)
    throw ConfigParserException ("Error: brackets mismatch");
}

ConfigParser &
ConfigParser::operator= (const ConfigParser &rhs)
{
  (void)rhs;
  return (*this);
}
