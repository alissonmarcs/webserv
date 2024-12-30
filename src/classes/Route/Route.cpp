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
	index = "";
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
		index = rhs.index;
	}
	return (*this);
}

Route::Route(const Route &src)
{
	*this = src;
}


void Route::validDirective(const std::string &directive)
{
    if (directive == "root" || directive == "autoindex" || directive == "allowed_methods" || 
        directive == "redirect" || directive == "default_file" || directive == "cgi_ext" || 
        directive == "upload_store" || directive == "index" || directive == "client_max_body_size" || 
		directive == "error_page" || directive == "return")
    {
		return ;
	}
    std::cerr << "Invalid directive: " << directive << std::endl;
    throw std::invalid_argument("Invalid directive: " + directive);
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
	{
		if (getRoot().empty())
			throw ConfigParserException("Error: root directive missing in route");
  		if (getAllowedMethods().empty())
			throw ConfigParserException("Error: allowed_methods directive missing in route");
  		if (getIndex().empty())
			throw ConfigParserException("Error: index directive missing in route");
		return (-1);
	}
	lineTreatment(routeLine);
	if (routeLine.empty())
		continue;

	istringstream routeIss(routeLine);
	string routeDirective, value;
	routeIss >> routeDirective >> value;

	validDirective(routeDirective);
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
	else if (routeDirective == "index")
		setIndex(value);
  }
  return (0);
}