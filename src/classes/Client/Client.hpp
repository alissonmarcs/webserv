#ifndef CLIENT_H
#define CLIENT_H

#include "Server.hpp"
#include "Utils.hpp"

class Client
{
public:
  Client ();
  Client (int fd, struct sockaddr_in addr, Server *owner);
  ~Client ();
  Client (const Client &src);

  /* getters*/
  struct sockaddr_in * getAdress () { return (&adress); }
  short getStatusCode () { return (status_code); }
  bool isParsingDone () { return (is_request_parsing_done); }
  time_t getLastReadTime () { return (last_read); }
  int getClientFd () { return (client_fd); }
  string & getBody () { return (body); }
  map<string, string> & getRequestHeaders () { return (request_headers); }
  const char * getIpString () { return inet_ntoa(adress.sin_addr); }

  /* setters */
  void setStatusCode (short code) { status_code = code; }
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
  bool haveError();

  void findRoute ();
  void http_get ();
  string findContentType ();
  void RouteValidation ();

  void http_post ();
  void handleUpload (map<string, string>::iterator content_type);
  string getBoundary (string content_type);

  void http_delete ();

  void autoindex();
  vector <string> * splitMultipart(string boundary);

  void handleFolder ();
  void loadStaticFile ();
  void redirectToFolderWithSlash ();

private:
  /* Connection */
  time_t last_read;
  int client_fd;
  struct sockaddr_in adress;
  Server *server_owner;
  short status_code;

  /* Request */
  bool is_request_parsing_done, is_chunked, is_sized;
  string raw_request, method, target_resource, query_params, version, body;
  map<string, string> request_headers;

  /* Response */
  Route * route;
  string response, static_file_name;
  struct stat file_info;
};

#endif