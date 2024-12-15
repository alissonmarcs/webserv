
#ifndef ROUTE_HPP_
# define ROUTE_HPP_

# include "Utils.hpp"

// Class declaration
class Route
{
  public:
	Route();
	//Route &operator=(const Route &rhs);

	string path;
	string root;
	bool autoindex;
	std::vector<string> allowed_methods;
	string redirect;
	string default_file;
	string cgi_ext;
	string upload_store;
};

#endif // ROUTE_HPP_
