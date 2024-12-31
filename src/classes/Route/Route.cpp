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
	indexFiles 	= vector<string>();
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
		indexFiles = rhs.indexFiles;
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

void
Route::checkDuplicateDirectiveRoute(const string &line)
{
	if (line.find("root") != string::npos && !getRoot().empty())
		throw ConfigParserException("Error: duplicate directive in route");
	else if (line.find("autoindex") != string::npos && getAutoindex() != true)
		throw ConfigParserException("Error: duplicate directive in route");
	else if (line.find("allowed_methods") != string::npos && !getAllowedMethods().empty())
		throw ConfigParserException("Error: duplicate directive in route");
	else if (line.find("redirect") != string::npos && !getRedirect().empty())
		throw ConfigParserException("Error: duplicate directive in route");
	else if (line.find("default_file") != string::npos && !getDefaultFile().empty())
		throw ConfigParserException("Error: duplicate directive in route");
	else if (line.find("cgi_ext") != string::npos && !getCgiExt().empty())
		throw ConfigParserException("Error: duplicate directive in route");
	else if (line.find("upload_store") != string::npos && !getUploadStore().empty())
		throw ConfigParserException("Error: duplicate directive in route");
	else if (line.find("index") != string::npos && !getIndex().empty())
		throw ConfigParserException("Error: duplicate directive in route");
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
	checkDuplicateDirectiveRoute(routeLine);
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
	else if (routeDirective == "index")
	{
		lineTreatment(value);
		setIndex(value);
		while(routeIss >> value)
		{
			if (value == ";")
				break;
			lineTreatment(value);
			setIndex(value);
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