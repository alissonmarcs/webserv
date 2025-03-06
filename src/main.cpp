#include "ConfigParser.hpp"
#include "ServerManager.hpp"
#include "Utils.hpp"

ServerManager Manager;

void signalHandler (int signum)
{
  (void)signum;
  cout << endl;
  thatIsAllFolks();
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
