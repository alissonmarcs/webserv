#include "Client.hpp"

bool
Client::isCGI()
{
    if (this->route->getCgiExt().empty())
        return (false);
  return (true);
}

bool validateExtension (const string & file_name, const string & cgi_ext)
{
  size_t dot = file_name.rfind('.');
  string ext;

  if  (dot == string::npos)
    return (false);
  ext = file_name.substr(dot + 1);
  return (ext == cgi_ext);
}

void
Client::findScriptName()
{
  if (validateExtension(target_resource, route->getCgiExt()))
    script_name = target_resource;
  else if (validateExtension(route->getIndex(), route->getCgiExt()))
    script_name = route->getIndex();
  else
    setError(NOT_FOUND);
}

void
Client::handleCGI()
{
  findScriptName ();
  if (response_is_done)
    return ;
  
}