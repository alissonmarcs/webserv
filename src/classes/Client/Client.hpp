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
  struct sockaddr_in  getAdress () { return (adress); }
  Server * getServerOwner () { return (server_owner); }
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
  void setError(short status_code);

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

  void validateBodyPost (string & boundary);
  void removeAllButBody (string & boundary);

  void populate_env_vars();

  void handleFolder ();
  void loadStaticFile ();
  void redirectToFolderWithSlash ();
  void locationRedirect ();

  void redirectStdin ();
  void redirectStdout ();
  bool isCGI ();
  void handleCGI ();
  bool findScriptPath ();
  bool response_is_done;

  string getHeader (string key);

  void child ();
  void parent ();
  bool checkChildStatus (int status);

  void chooseServer ();

  void redirectStderr ();


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

  /* CGI */
  pid_t pid;
  string path_info;
  const char *env_vars[30];
  vector<string> vars;
  time_t child_born;
  string script_path, script_name;
};

#endif