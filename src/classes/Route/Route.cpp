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
