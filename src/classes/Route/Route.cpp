#include "Utils.hpp"
#include "Route.hpp"

// Class constructor
Route::Route()
{
	root = "";
	autoindex = true;
	redirect = "";
	default_file = "";
	cgi_ext = "";
	upload_store = "";
}

// Class assignment operator
Route &Route::operator=(const Route &rhs)
{
	if (this != &rhs)
	{
		path = rhs.path;
		root = rhs.root;
		autoindex = rhs.autoindex;
		allowed_methods = rhs.allowed_methods;
		redirect = rhs.redirect;
		default_file = rhs.default_file;
		cgi_ext = rhs.cgi_ext;
		upload_store = rhs.upload_store;
	}
	return (*this);
}

Route::Route(const Route &src)
{
	*this = src;
}

int
Route::parseRouteConfig(const string &line, istringstream &stream)
{
  string directiveName, path;
  istringstream routeStream (line);

  routeStream >> directiveName >> path;
  if (directiveName != "location")
	throw ConfigParserException("Error: invalid directive in route");
  lineTreatment(path);
  setPath(path);
  string routeLine;
  while(getline(stream, routeLine))
  {
	if (routeLine.find("location") != string::npos)
		throw ConfigParserException("Error: invalid directive in route");
	if (routeLine.find("}") != string::npos)
		return (-1);
	lineTreatment(routeLine);
	if (routeLine.empty())
		continue;

	istringstream routeIss(routeLine);
	string routeDirective, value;
	routeIss >> routeDirective >> value;

	if (routeDirective == "root")
		setRoot(value);
	else if (routeDirective == "autoindex")
	{
		lineTreatment(value);
		if (value == "on")
			setAutoindex(true);
		else if (value == "off")
			setAutoindex(false);
	}
	else if (routeDirective == "allowed_methods")
	{
		lineTreatment(value);
		setAllowedMethods(value);
		while(routeIss >> value)
		{
			if (value != "GET" && value != "POST" && value != "DELETE")
				throw ConfigParserException("Error: invalid method in allowed_methods");
			if (value == ";")
				break;
			lineTreatment(value);
			setAllowedMethods(value);
		}
 	}
	else if (routeDirective == "redirect")
		setRedirect(value);
	else if (routeDirective == "default_file")
		setDefaultFile(value);
	else if (routeDirective == "cgi_ext")
		setCgiExt(value);
	else if (routeDirective == "upload_store")
		setUploadStore(value);
  }
  return (0);
}