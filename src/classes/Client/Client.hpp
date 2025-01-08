#ifndef CLIENT_H
#define CLIENT_H

#include "Server.hpp"
#include "Utils.hpp"

class Server;

class Client
{
public:
  Client ();
  Client (int fd, struct sockaddr_in addr, Server *owner);
  ~Client ();
  Client (const Client &src);

  struct sockaddr_in * getAdress () { return (&adress); }
  short getErrorCode () { return (error_code); }
  bool getIsParsingDone () { return (is_request_parsing_done); }
  
  void readRequest ();
  void parseRequestLine();
  void parseHeaders();
  void parseBody();
  void parseSizedBody();
  void parseChunkedBody();
  void printRequest();
  int isInvalidBody (string & request);
  void removeChunkedDelimiters();

private:
  /* Connection */
  int client_fd;
  struct sockaddr_in adress;
  Server *server_owner;
  short error_code;

  /* Request */
  bool is_request_parsing_done, is_chunked;
  string raw_request, method, target_resource, version, body;
  map<string, string> request_headers;

};

#endif