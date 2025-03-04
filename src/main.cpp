#include "ConfigParser.hpp"
#include "ServerManager.hpp"
#include "Utils.hpp"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

ServerManager Manager;
extern ServerManager Manager;

void signalHandler (int signum)
{
  (void)signum;
  cout << "\nBye!" << endl;
  exit (EXIT_SUCCESS);
}

int
main (int argc, char **argv)
{
  signal (SIGINT, signalHandler);

  if (validateConfigFile (argc, argv))
    exit (EXIT_FAILURE);

  try
    {

      string configFile = readFileAsString (argv[1]);

      ConfigParser parser (configFile);
      Manager.getServersRef () = parser.servers;
      // printServerInfo(Manager);
      configFile.~string ();
      parser.~ConfigParser ();
      Manager.initServers ();
      Manager.mainLoop ();
    }
  catch (const std::exception &e)
    {
      std::cerr << RED << e.what () << RESET << std::endl;
	    exit(EXIT_FAILURE);
    }
  return (0);
}