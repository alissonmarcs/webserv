#ifndef UTILS_HPP
#define UTILS_HPP

#define ORANGE "\033[38;5;208m"
#define PINK "\033[38;5;213m"
#define PURPLE "\033[38;5;135m"
#define GREY "\033[38;5;246m"
#define BROWN "\033[38;5;130m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define CYAN "\033[1;36m"
#define YELLOW "\033[1;33m"
#define MAGENTA "\033[1;95m"
#define BLUE "\033[1;34m"
#define BOLD "\033[1m"
#define RESET "\033[0m"
#define INVERSE "\033[7m"

#define BUFFER_SIZE 6096
#define QUEUED_CONNECTIONS 300
#define MAX_EPOLL_EVENTS 300
#define FATAL_ERROR(msg) fatal_error (msg, __FILE__, __LINE__)
#define NON_FATAL_ERROR(msg) non_fatal_error (msg, __FILE__, __LINE__)
#define LOGGER(client, msg) logger(client, msg, __FILE__, __LINE__)

using namespace std;

enum file_status
{
  FileNoPermission,
  DirectoryNoPermission,
  FileNotFound,
  DirectoryNotFound,
  FileWithPermissions,
  DirectoryWithPermissions
};

#include <arpa/inet.h>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <wchar.h>

using namespace std;

class ServerManager;
class Server;

int processInput (string path);
bool validateConfigFile (int argc, char **argv);
bool isValidIp (string ip);
string getClientIp (struct sockaddr_in *client_addr);
string readFileAsString (const string &path);
void printServerInfo (ServerManager &Manager);
void fatal_error (const char *msg, const char *file, long line);
void non_fatal_error (const char *msg, const char *file, long line);
void printRoutesInfo(Server &server);
void logger (const char *client, const char * msg, const char *file, long line);


int processInput (std::string path);
bool validateConfigFile (int argc, char **argv);
bool isValidIp (string ip);
string getClientIp (struct sockaddr_in *client_addr);

void removeSemicolon (string &line);
string trim (const string &str);
void trim2 (string &str);
string removeComments (string &line);


#endif