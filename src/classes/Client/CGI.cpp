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
    return (false);
  if (validateExtension(target_resource, route->getCgiExt()))
    script_path = route->getRoot () + target_resource;
  else if (validateExtension(route->getIndex(), route->getCgiExt()))
    script_path = route->getRoot () + route->getIndex();
  else
  {
    setError(NOT_FOUND);
    return (false);
  }
  if (access(script_path.c_str(), F_OK) == -1)
    setError(NOT_FOUND); 
  else if (access(script_path.c_str(), X_OK) == -1)
    setError(FORBIDDEN);
  return (true);
}

void
Client::redirectStdin ()
{
  fd_in = open (CGI_FILE_IN, O_CREAT | O_TRUNC | O_RDWR, 0666);

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
  fd_out = open (CGI_FILE_OUT, O_CREAT | O_TRUNC | O_RDWR, 0666);

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
Client::handleCGI()
{
  if (!findScriptPath())
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

    string folder = get_folder (script_path);
    string script_name = get_script_name (script_path);

    if (chdir(folder.c_str()) == -1)
      FATAL_ERROR("chdir");

    char *argv[] = {const_cast<char*>(script_name.c_str()), NULL};
    if (execve(script_name.c_str(), argv, NULL) == -1)
      FATAL_ERROR("execve");
  }
  else
  {
    int child_status = 0;

    while (waitpid(pid, &child_status, WNOHANG) == 0)
    {
      if (time(NULL) - start_time_cgi_process > SECONDS_TIME_OUT_CGI_PROCESS)
      {
        if (kill(pid, SIGKILL) == -1)
          FATAL_ERROR("kill");
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