#include "ConfigParser.hpp"
#include "ServerManager.hpp"
#include "Utils.hpp"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void
populateServers (ServerManager &Manager)
{
  Server *server;
  Manager.addEmptyServer ();
  server = Manager.getLastServer ();
  server->setHost ("127.0.0.1");
  server->setPort (9000);
}

int
main (int argc, char **argv)
{
  ServerManager Manager;

  if (validateConfigFile (argc, argv))
    exit (EXIT_FAILURE);

  string configFile = readFileAsString (argv[1]);
  try
    {
      ConfigParser parser (configFile);
      Manager.getServersRef () = parser.servers;
    }
  catch (const std::exception &e)
    {
      std::cerr << RED << e.what () << RESET << std::endl;
    }
  printServerInfo (Manager);
}