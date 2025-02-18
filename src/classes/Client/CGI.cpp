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
Client::redirectStdin ()
{
  fd_in = open (CGI_FILE_IN, O_CREAT | O_TRUNC | O_RDWR, 0666);
  if (fd_in == -1)
    exit(EXIT_FAILURE);
  if (write (fd_in, body.data(), body.size()) == -1)
    exit(EXIT_FAILURE);
  if (close (fd_in) == -1)
    exit(EXIT_FAILURE);
  fd_in = open (CGI_FILE_IN, O_RDONLY);
  if (fd_in == -1)
    exit(EXIT_FAILURE);
  if (dup2(fd_in, STDIN_FILENO) == -1)
    exit(EXIT_FAILURE);
  close (fd_in);
}

void
Client::redirectStdout ()
{
  fd_out = open (CGI_FILE_OUT, O_CREAT | O_TRUNC | O_RDWR, 0666);
  if (fd_out == -1)
    exit(EXIT_FAILURE);
  if (dup2(fd_out, STDOUT_FILENO) == -1)
    exit(EXIT_FAILURE);
  close (fd_out);
}

void
Client::handleCGI()
{

  findScriptName ();
  if (haveError()) 
    return ;
  start_time_cgi_process = time(NULL);
  pid = fork();
  if (pid == -1)
  {
    setError(INTERNAL_SERVER_ERROR);
    return ;
  }
  else if (pid == 0)
  {
    if (method == "POST")
      redirectStdin();
    redirectStdout();

    char *argv[] = {const_cast<char*>(script_name.c_str()), NULL};
    if (execve(script_name.c_str(), argv, NULL) == -1)
      exit(EXIT_FAILURE);
  }
  else
  {
    int child_status = 0;

    while (waitpid(pid, &child_status, WNOHANG) == 0)
    {
      if (time(NULL) - start_time_cgi_process > 3)
      {
        if (kill(pid, SIGKILL) == -1)
        {
          perror ("kill");
          exit(EXIT_FAILURE);
        }
        break ;
      }
    }

    while (waitpid(pid, &child_status, WNOHANG) == 0)
     ;

    if (WIFSIGNALED(child_status) && WTERMSIG(child_status) == SIGKILL)
    {
      setError(GATEWAY_TIMEOUT);
      return ;
    }
    else if (WIFEXITED(child_status) && WEXITSTATUS(child_status) != 0)
    {
      setError(BAD_GATEWAY);
      return ;
    }

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
    response_is_done = true;
  }
}