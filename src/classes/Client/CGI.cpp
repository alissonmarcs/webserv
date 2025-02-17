#include "Client.hpp"

bool
Client::isCGI()
{
    if (this->route && this->route->getCgiExt().empty())
        return (false);
  return (true);
}

bool validateExtension (const string & file_name, const string & cgi_ext)
{
  size_t dot = file_name.rfind('.');
  string ext;

  if  (dot == string::npos)
    return (false);
  ext = file_name.substr(dot);
  return (ext == cgi_ext);
}

void
Client::findScriptName()
{
  if (route == NULL)
    return ;
  if (validateExtension(target_resource, route->getCgiExt()))
    script_name = route->getRoot () + target_resource;
  else if (validateExtension(route->getIndex(), route->getCgiExt()))
    script_name = route->getRoot () + route->getIndex();
  else
  {
    setError(NOT_FOUND);
    return ;
  }
  if (access(script_name.c_str(), F_OK) == -1)
    setError(NOT_FOUND); 
  else if (access(script_name.c_str(), X_OK) == -1)
    setError(FORBIDDEN);
}

void
Client::handleCGI()
{
  findScriptName ();
  if (haveError()) 
    return ;

  pid = fork();
  if (pid == -1)
  {
    setError(INTERNAL_SERVER_ERROR);
    return ;
  }
  else if (pid == 0)
  {
    fd_out = open (CGI_FILE_OUT, O_CREAT | O_TRUNC | O_RDWR, 0666);
    if (fd_out == -1)
      exit(EXIT_FAILURE);
    if (dup2(fd_out, STDOUT_FILENO) == -1)
      exit(EXIT_FAILURE);
    close (fd_out);

    if (execve(script_name.c_str(), NULL, NULL) == -1)
      exit(EXIT_FAILURE);
  }
  else
  {
    waitpid(pid, NULL, 0);

    fd_in = open (CGI_FILE_OUT, O_RDONLY);
    if (fd_in == -1)
    {
      setError(INTERNAL_SERVER_ERROR);
      return ;
    }
    char buffer[BUFFER_SIZE];
    int ret;
    while ((ret = read(fd_in, buffer, BUFFER_SIZE)) > 0)
      response.append(buffer, ret);
    close(fd_in);
    cgi_is_done = true;
  }
}