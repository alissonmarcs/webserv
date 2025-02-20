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

bool
Client::findScriptPath()
{
  if (route == NULL)
    { setError(NOT_FOUND); return (false); }

  if (validateExtension(target_resource, route->getCgiExt()))
    script_path = route->getRoot () + target_resource;
  else if (target_resource[target_resource.size() - 1] == '/')
  {
    if (route->getIndex().empty() && route->getAutoindex() == false)
      { setError(NOT_FOUND); return (false); }
    if (route->getIndex().empty() == false)
      script_path = route->getRoot () + target_resource + route->getIndex();
    else if (route->getAutoindex() == true)
     {
      autoindex();
      return (false);
     } 
  }
  else
    { setError(NOT_FOUND); return (false); }

  if (access(script_path.c_str(), F_OK) == -1)
    { setError(NOT_FOUND); return (false); }
  else if (access(script_path.c_str(), X_OK) == -1)
    { setError(FORBIDDEN); return (false); }
  return (true);
}

void
Client::redirectStdin ()
{
  int fd_in = open (CGI_FILE_IN, O_CREAT | O_TRUNC | O_RDWR, 0666);

  if (fd_in == -1)
    FATAL_ERROR("open");
  if (write (fd_in, body.data(), body.size()) == -1)
    FATAL_ERROR("write");
  if (close (fd_in) == -1)
    FATAL_ERROR("close");
  fd_in = open (CGI_FILE_IN, O_RDONLY);
  if (fd_in == -1)
    FATAL_ERROR("open");
  if (dup2(fd_in, STDIN_FILENO) == -1)
    FATAL_ERROR("dup2");
  if (close (fd_in) == -1)
    FATAL_ERROR("close");
}

void
Client::redirectStdout ()
{
  int fd_out = open (CGI_FILE_OUT, O_CREAT | O_TRUNC | O_RDWR, 0666);

  if (fd_out == -1)
    FATAL_ERROR("open");
  if (dup2(fd_out, STDOUT_FILENO) == -1)
    FATAL_ERROR("dup2");
  else if (close (fd_out) == -1)
    FATAL_ERROR("close");
}

string get_folder (const string & script_path)
{
  size_t last_slash = script_path.rfind('/');
  if (last_slash == string::npos)
    return ("");
  return (script_path.substr(0, last_slash + 1));
}

string get_script_name (string script_path)
{
  size_t last_slash = script_path.rfind('/');

  if (last_slash == string::npos)
    return ("");
  return (script_path.substr(last_slash + 1));
}

void
Client::child ()
{
 if (close (client_fd) < 0) 
    FATAL_ERROR("close");

  string folder = get_folder (script_path);
  string script_name = get_script_name (script_path);
  char *argv[] = {const_cast<char*>(script_name.c_str()), NULL};
  
  redirectStdin();
  redirectStdout();
  if (chdir(folder.c_str()) == -1)
    FATAL_ERROR("chdir");
  if (execve(script_name.c_str(), argv, NULL) == -1)
    FATAL_ERROR("execve");
}

int
wait_child (pid_t pid, time_t child_born)
{
  int child_status = 0;

  while (waitpid(pid, &child_status, WNOHANG) == 0)
  {
    if (time(NULL) - child_born > SECONDS_TIME_OUT_CGI_PROCESS)
    {
      if (kill(pid, SIGKILL) == -1)
        FATAL_ERROR("kill");
      break ;
    }
  }

  while (waitpid(pid, &child_status, WNOHANG) == 0)
    ;
  return (child_status);
}

void
Client::parent ()
{
  int status, output_fd, bytes;
  char buffer[BUFFER_SIZE] = {0};
  
  status = wait_child (pid, child_born); 
  if (!checkChildStatus (status))
    return ;
  
  output_fd = open (CGI_FILE_OUT, O_RDONLY);
  if (output_fd == -1)
    { setError(INTERNAL_SERVER_ERROR); return ; }

  while ((bytes = read(output_fd, buffer, BUFFER_SIZE)) > 0)
    response.append(buffer, bytes);
  
  if (bytes == -1)
    { setError(INTERNAL_SERVER_ERROR); return ; }
  else if (response.size() == 0)
    { setError(BAD_GATEWAY); return ; }
  close(output_fd);
  response_is_done = true;
}

bool
Client::checkChildStatus (int status)
{
  if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL)
    { setError(GATEWAY_TIMEOUT); return false; }
  else if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
    { setError(BAD_GATEWAY); return false; }
  return true;
}

void
Client::handleCGI ()
{
  if (!findScriptPath())
    return ;
  child_born = time (NULL);
  pid = fork ();
  if (pid == -1)
    { setError(INTERNAL_SERVER_ERROR); return ; }
  else if (pid == 0)
    child ();
  else
    parent ();
}