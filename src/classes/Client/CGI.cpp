#include "Client.hpp"

bool validateExtension (const string & file_name, vector<string> & cgi_extensions);
int wait_child (pid_t pid, time_t child_born);
string get_folder (const string & script_path);
string get_script_name (string script_path);

bool
Client::isCGI()
{
    if (this->route && this->route->getCgiExt().empty())
        return (false);
  return (true);
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

bool validateExtension (const string & file_name, vector<string> & cgi_extensions)
{
  size_t dot = file_name.rfind('.');
  vector<string>::iterator start, end;
  string ext;

  if (dot == string::npos)
    return (false);
  ext = file_name.substr(dot);
  start = cgi_extensions.begin();
  end = cgi_extensions.end();
  while (start != end)
  {
    if (*start == ext)
      return (true);
    start++;
  }
  return (false);
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
    {
      script_path = route->getRoot () + target_resource;

      if (access(script_path.c_str(), F_OK) == -1)
        { setError(NOT_FOUND); return (false); }
      else
        { setError(FORBIDDEN); return (false); }
    }

  if (access(script_path.c_str(), F_OK) == -1)
    { setError(NOT_FOUND); return (false); }
  else if (access(script_path.c_str(), X_OK) == -1)
    { setError(FORBIDDEN); return (false); }
  return (true);
}

void
Client::parent ()
{
  int status;
  stringstream content;
  
  status = wait_child (pid, child_born); 
  if (!checkChildStatus (status))
    return ;
  
  ifstream file(CGI_FILE_OUT, ios::binary);
  if (file.is_open() == false)
    { setError(INTERNAL_SERVER_ERROR); return ; }
   content << file.rdbuf();
   response += content.str();
  if (response.size() == 0)
    { setError(BAD_GATEWAY); return ; }
  file.close();
  response_is_done = true;
}


void
Client::populate_env_vars()
{
  string currentHeader;

  vars.push_back(string("GATEWAY_INTERFACE=") + GATEWAY_INTERFACE);
  vars.push_back(string("SERVER_SOFTWARE=") + SERVER_SOFTWARE);
  vars.push_back(string("SERVER_PROTOCOL=") + SERVER_PROTOCOL);
  vars.push_back(string("REQUEST_METHOD=") + this->method);
  vars.push_back(string("SCRIPT_NAME=") + this->script_name);
  vars.push_back(string("SERVER_NAME=") + getHeader("host"));
  vars.push_back(string("REMOTE_ADDR=") + getIpString());
  vars.push_back(string("SERVER_PORT=") + to_string(server_owner->getPort()));

  currentHeader = getHeader("content-type");
  if(currentHeader.empty() == false)
    vars.push_back(string("CONTENT_TYPE=") + currentHeader);
 
  currentHeader = getHeader("content-length");
  if(currentHeader.empty() == false)
    vars.push_back(string("CONTENT_LENGTH=") + currentHeader);

  if (query_params.empty() == false)
    vars.push_back(string("QUERY_STRING=") + query_params);

  if (path_info.empty() == false)
    vars.push_back(string("PATH_INFO=") + path_info);

  for (size_t i = 0; i < vars.size(); i++){
    env_vars[i] = vars[i].c_str();
  }
}


void
Client::child ()
{
 if (close (client_fd) < 0) 
    FATAL_ERROR("close");

  string folder = get_folder (script_path);
  script_name = get_script_name (script_path);
  char *argv[] = {const_cast<char*>(script_name.c_str()), NULL};
  
  redirectStdin();
  redirectStdout();
  redirectStderr();
  populate_env_vars();
  if (chdir(folder.c_str()) == -1)
    FATAL_ERROR("chdir");
  if (execve(script_name.c_str(), argv, const_cast<char* const*>(env_vars)) == -1)
    FATAL_ERROR("execve");
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
Client::redirectStdin ()
{
  ofstream file(CGI_FILE_IN, ios::trunc | ios::binary);

  if (!file)
    FATAL_ERROR("open");

  file << body;
  file.close();
  if (!file)
    FATAL_ERROR("close");
  int fd_in = open (CGI_FILE_IN, O_RDONLY);
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

void
Client::redirectStderr ()
{
  int fd_err = open (CGI_FILE_ERR, O_CREAT | O_APPEND | O_RDWR, 0666);

  if (fd_err == -1)
    FATAL_ERROR("open");
  if (dup2(fd_err, STDERR_FILENO) == -1)
    FATAL_ERROR("dup2");
  else if (close (fd_err) == -1)
    FATAL_ERROR("close");
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

bool
Client::checkChildStatus (int status)
{
  if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL)
    { setError(GATEWAY_TIMEOUT); return false; }
  else if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
    { setError(BAD_GATEWAY); return false; }
  return true;
}

string
Client::getHeader (string key)
{
  if (request_headers.count (key) == 0)
    return "";
  return request_headers[key];
}