
#ifndef ROUTE_HPP_
# define ROUTE_HPP_

# include "Utils.hpp"
# include "Server.hpp"

class Server;

// Class declaration
class Route
{
  public:
	Route();
	Route(const Route &src);
	Route &operator=(const Route &rhs);

	// Getters
	string & getPath() { return path; }
	string	getRoot() const { return root; }
	bool getAutoindex() const { return autoindex; }
	vector<string> & getAllowedMethods() { return allowed_methods; }
	string getRedirect() const { return redirect; }
	string getDefaultFile() const { return default_file; }
	vector<string> & getCgiExt() { return cgi_ext; }
	string getUploadStore() const { return upload_store; }
	string getIndex() const { return index; }
	bool getDirectiveStatus(string directive) { return directiveStatus[directive]; }
	string getFolderToSearchInside() { return folder_to_search_inside; }

	// Setters
	void setPath(string path) { this->path = path; }
	void setRoot(string root) { this->root = root; }
	void setAutoindex(const string &value);
	void setRedirect(const string &value);
	void setDefaultFile(string default_file) { this->default_file = default_file; }
	void setAllowedMethods(const string &value, istringstream &routeIss);
	void setCgiExt(const string &value, istringstream &routeIss);
	void setUploadStore(const string &upload_store);
	void setIndex(string index) { this->index = index; }
	void setDirectiveStatus(string directive, bool status) { directiveStatus[directive] = status; }
	void setFolderToSearchInside(string folder) { folder_to_search_inside = folder; }

	// Route parsing
	int parseRouteConfig(const string &line, istringstream &stream, int &nestingLevel);
	void validDirective(const std::string &directive);
	void checkDuplicateDirectiveRoute(const string &line);
	void checkEmptyDirectiveValue(const string &value);
	void setDirectiveValue(const string &directive, const string &value, istringstream &routeIss);
	void parseLocation(const string &line);
	void checkBasicDirectiveAreSet();

  private:
	string folder_to_search_inside;
	string path;
	string root;
	bool autoindex;
	vector<string> allowed_methods;
	string redirect;
	string default_file;
	vector<string> cgi_ext;
	string upload_store;
	string index;
	map<string, bool> directiveStatus;

	void initDirectiveStatus();
};

#endif // ROUTE_HPP
