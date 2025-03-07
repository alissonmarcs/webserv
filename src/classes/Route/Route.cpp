#include "Utils.hpp"
#include "Route.hpp"

// Class constructor
Route::Route()
{
	root = "";
	autoindex = false;
	redirect = "";
	default_file = "";
	upload_store = "";
	index = "";
	initDirectiveStatus();
}

Route::~Route()
{
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
		directiveStatus = rhs.directiveStatus;
		folder_to_search_inside = rhs.folder_to_search_inside;
	}
	return (*this);
}

Route::Route(const Route &src)
{
	*this = src;
}

int
Route::parseRouteConfig(const string &line, istringstream &stream, int &nestingLevel)
{
  parseLocation(line);
  string routeLine;
  while(getline(stream, routeLine))
  {
	if (routeLine.find("location") != string::npos)
		throw ConfigParserException("Error: invalid directive in route");
	if (routeLine.find("{") != string::npos)
		nestingLevel++;
	if (routeLine.find("}") != string::npos)
	{
		checkBasicDirectiveAreSet();
		nestingLevel--;
		if (path == "/")
			folder_to_search_inside = root;
		else
  			folder_to_search_inside = root + path;
		return (1);
	}
	lineTreatment(routeLine);
	if (routeLine.empty())
		continue;

	istringstream routeIss(routeLine);
	string routeDirective, value, extraValue;
	routeIss >> routeDirective >> value;

	if (routeDirective != "allowed_methods" && routeDirective != "cgi_ext" && routeIss >> extraValue)
		throw ConfigParserException("Error: too many arguments in directive");
	if (routeDirective != "{")
		checkEmptyDirectiveValue(value);
	setDirectiveValue(routeDirective, value, routeIss);
  }
  return (0);
}
void Route::initDirectiveStatus()
{
	directiveStatus["root"] = false;
	directiveStatus["autoindex"] = false;
	directiveStatus["allowed_methods"] = false;
	directiveStatus["redirect"] = false;
	directiveStatus["default_file"] = false;
	directiveStatus["cgi_ext"] = false;
	directiveStatus["upload_store"] = false;
	directiveStatus["index"] = false;
}

void 
Route::setDirectiveValue(const string &directive, const string &value, istringstream &routeIss)
{
	validDirective(directive);
	checkDuplicateDirectiveRoute(directive);
	if (directive == "root")
		setRoot(value);
	else if (directive == "autoindex")
		setAutoindex(value);
	else if (directive == "redirect")
		setRedirect(value);
	else if (directive == "default_file")
		setDefaultFile(value);
	else if (directive == "cgi_ext")
		setCgiExt(value, routeIss);
	else if (directive == "upload_store")
		setUploadStore(value);
	else if (directive == "index")
		setIndex(value);
	else if (directive == "allowed_methods")
		setAllowedMethods(value, routeIss);
}


void Route::validDirective(const std::string &directive)
{
    if (directive == "root" || directive == "autoindex" || directive == "allowed_methods" || 
        directive == "redirect" || directive == "default_file" || directive == "cgi_ext" || 
        directive == "upload_store" || directive == "index" || directive == "error_page" || 
		directive == "}" || directive == "{")
    {
		return ;
	}
    throw std::invalid_argument("Invalid directive: " + directive);
}

void
Route::checkEmptyDirectiveValue(const string &value)
{
	if (value.empty())
		throw ConfigParserException("Error: empty value in directive");
}

void
Route::checkDuplicateDirectiveRoute(const string &directive) {
    if (getDirectiveStatus(directive)) {
		string tmp = "Error: duplicate directive in route: " + directive;
        throw ConfigParserException(tmp.c_str());
    }
    setDirectiveStatus(directive, true);
}

void
Route::parseLocation(const string &line)
{
	string directiveName, path;
	istringstream routeStream (line);

	routeStream >> directiveName >> path;
	if (directiveName != "location")
		throw ConfigParserException("Error: invalid directive in route");
	lineTreatment(path);
	trimBraces(path);
	setPath(path);
}

void
Route::checkBasicDirectiveAreSet()
{
	if (getRoot().empty())
			throw ConfigParserException("Error: root directive missing in route");
  	if (getAllowedMethods().empty())
			throw ConfigParserException("Error: allowed_methods directive missing in route");
	if (getPath().empty())
		throw ConfigParserException("Error: location directive missing in route");
}

void
Route::setUploadStore(const string &upload_store)
{
	struct stat file_info;
	string upload = upload_store;
	trim(upload);

	
	if (upload.empty() || upload[0] != '/' || upload[upload.size() - 1] != '/')
	throw ConfigParserException("Error: value of 'upload_store' must be a directory");
	
	string UploadStorePath = getRoot() + upload;

	if (stat(UploadStorePath.c_str(), &file_info) == -1)
		throw ConfigParserException("Error: upload_store directory does not exist");

	this->upload_store = upload;
}

void
Route::setCgiExt(const string &value, istringstream &routeIss)
{
	string ext;
	if (value.empty())
		throw ConfigParserException("Error: empty value in cgi_ext directive");
	if (value[0] != '.')
		throw ConfigParserException("Error: invalid value in cgi_ext directive");
	cgi_ext.push_back(value);
	while (routeIss >> ext)
	{
		if (ext[0] != '.')
			throw ConfigParserException("Error: invalid value in cgi_ext directive");
		if (find(cgi_ext.begin(), cgi_ext.end(), ext) != cgi_ext.end())
			throw ConfigParserException("Error: duplicate value in cgi_ext directive");
		cgi_ext.push_back(ext);
	}
}

void
Route::setAllowedMethods(const string &value, istringstream &routeIss)
{
	string method;
	if (value != "GET" && value != "POST" && value != "DELETE" && value != "PUT")
		throw ConfigParserException("Error: invalid method in allowed_methods directive");
	allowed_methods.push_back(value);
	while (routeIss >> method)
	{
		if (method != "GET" && method != "POST" && method != "DELETE" && method != "PUT")
			throw ConfigParserException("Error: invalid method in allowed_methods directive");
		if (find(allowed_methods.begin(), allowed_methods.end(), method) != allowed_methods.end())
			throw ConfigParserException("Error: duplicate value in allowed_methods directive");
		allowed_methods.push_back(method);
	}
}

void
Route::setAutoindex(const string &value)
{
	if (value == "on")
		autoindex = true;
	else if (value == "off")
		autoindex = false;
	else
		throw ConfigParserException("Error: invalid value in autoindex directive");
}

void
Route::setRedirect(const string &value)
{
	if (value.empty())
		throw ConfigParserException("Error: empty value in redirect directive");
	if (value == path)
		throw ConfigParserException("Error: redirect loop");


	redirect = value;
}