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

  /* getters*/
  struct sockaddr_in * getAdress () { return (&adress); }
  short getErrorCode () { return (error_code); }
  bool isParsingDone () { return (is_request_parsing_done); }
  time_t getLastReadTime () { return (last_read); }
  int getClientFd () { return (client_fd); }
  string & getBody () { return (body); }
  map<string, string> & getRequestHeaders () { return (request_headers); }

  /* setters */
  void setErrorCode (short code) { error_code = code; }
  string & getResponse () { return (response); }
  
  void readRequest ();
  void buildResponse();

  void parseRequestLine();
  void parseHeaders();
  void parseBody();
  void parseSizedBody();
  void parseChunkedBody();
  void printRequest();
  void parseRequest();

  void buildError();

private:
  /* Connection */
  time_t last_read;
  int client_fd;
  struct sockaddr_in adress;
  Server *server_owner;
  short error_code;

  /* Request */
  bool is_request_parsing_done, is_chunked, is_sized;
  string raw_request, method, target_resource, version, body;
  map<string, string> request_headers;

  /* Response */
  string response;
};

#endif