#ifndef UTILS_HPP
#define UTILS_HPP

enum HTTP_STATUS_CODES
{
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202, NO_CONTENT = 204,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    REQUEST_TIMEOUT = 408,
    CONFLIT = 409,
    PAYLOAD_TOO_LARGE = 413,
    UNSUPPORTED_MEDIA_TYPE = 415,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    GATEWAY_TIMEOUT = 504,
    HTTP_VERSION_NOT_SUPPORTED = 505
};

#define BAD_REQUEST_HTML "<html><head><title>400 Bad Request</title></head><body><center><h1>400 Bad Request</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define UNAUTHORIZED_HTML "<html><head><title>401 Unauthorized</title></head><body><center><h1>401 Unauthorized</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define FORBIDDEN_HTML "<html><head><title>403 Forbidden</title></head><body><center><h1>403 Forbidden</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define NOT_FOUND_HTML "<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define METHOD_NOT_ALLOWED_HTML "<html><head><title>405 Method Not Allowed</title></head><body><center><h1>405 Method Not Allowed</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define REQUEST_TIMEOUT_HTML "<html><head><title>408 Request Timeout</title></head><body><center><h1>408 Request Timeout</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define REQUEST_CONFLIT_HTML "<html><head><title>409 Conflit</title></head><body><center><h1>409 Conflit</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define PAYLOAD_TOO_LARGE_HTML "<html><head><title>413 Payload Too Large</title></head><body><center><h1>413 Payload Too Large</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define INTERNAL_SERVER_ERROR_HTML "<html><head><title>500 Internal Server Error</title></head><body><center><h1>500 Internal Server Error</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define NOT_IMPLEMENTED_HTML "<html><head><title>501 Not Implemented</title></head><body><center><h1>501 Not Implemented</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define HTTP_VERSION_NOT_SUPPORTED_HTML "<html><head><title>505 HTTP Version Not Supported</title></head><body><center><h1>505 HTTP Version Not Supported</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define UNSUPPORTED_MEDIA_TYPE_HTML "<html><head><title>415 HTTP Unsupported Media Type</title></head><body><center><h1>HTTP Unsupported Media Type</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define GATEWAY_TIMEOUT_HTML "<html><head><title>505 Gateway Timeout</title></head><body><center><h1>Gateway Timeout</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define BAD_GATEWAY_HTML "<html><head><title>502 Bad Gateway</title></head><body><center><h1>Bad Gateway</h1></center><hr><center>nginx/1.27.2</center></body></html>"

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

#define CGI_FILE_IN "/tmp/webserv_cgi_file_in"
#define CGI_FILE_OUT "/tmp/webserv_cgi_file_out"
#define CGI_FILE_ERR "./errors_cgi.log"
#define SECONDS_TIME_OUT_CGI_PROCESS 3

#define HARD_MAX_BODY_SIZE 838860800 // 800MB
#define BUFFER_SIZE 65000
#define QUEUED_CONNECTIONS 300
#define MAX_EPOLL_EVENTS 300
#define FATAL_ERROR(msg) fatal_error (msg, __FILE__, __LINE__)
#define NON_FATAL_ERROR(msg) non_fatal_error (msg, __FILE__, __LINE__)
#define LOGGER(client, msg) logger(client, msg, __FILE__, __LINE__)
#define GATEWAY_INTERFACE "CGI/1.1"
#define SERVER_SOFTWARE "webserv/1.0"
#define SERVER_PROTOCOL "HTTP/1.1"


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
#include <algorithm>
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
#include <sys/time.h>
#include <unistd.h>
#include <vector>
#include <wchar.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

#include "Error.hpp"

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
void trim (string &str);
string removeComments (string &line);
void lowercase (string &str);

void trimBraces (string &line);
void lineTreatment (string &line);
bool containsLetter(const string &str);
long	get_time(void);
string to_string (short status_code);
const char * getStatusText (short status_code);
const char * getCurrentHttpDate ();
string to_string (size_t status_code);
string to_string (uint16_t status_code);
size_t getDifferIndex (string &one, string &two);

#endif