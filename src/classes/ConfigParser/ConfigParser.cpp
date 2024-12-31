#include "ConfigParser.hpp"
#include "Error.hpp"

ConfigParser::ConfigParser (const string &config)
{
  istringstream stream (config);
  string lineStream;
  int nestingLevel = 0;
  Server *activeServer = NULL;

  while (getline (stream, lineStream))
    {
      lineTreatment (lineStream);
      if (lineStream.empty ())
        continue;
	  if (nestingLevel == 0 && lineStream.find("server") == string::npos)
		throw ConfigParserException("Error: server directive missing");
      if (lineStream.find ("{") != string::npos)
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
