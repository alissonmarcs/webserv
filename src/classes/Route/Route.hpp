
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
	string getRoot() const { return root; }
	bool getAutoindex() const { return autoindex; }
	std::vector<string> & getAllowedMethods() { return allowed_methods; }
	string getRedirect() const { return redirect; }
	string getDefaultFile() const { return default_file; }
	string getCgiExt() const { return cgi_ext; }
	string getUploadStore() const { return upload_store; }
	string getIndex() const { return index; }
	bool getDirectiveStatus(string directive) { return directiveStatus[directive]; }
	string getFolderToSearchInside() { return folder_to_search_inside; }

	// Setters
	void setPath(string path) { this->path = path; }
	void setRoot(string root) { this->root = root; }
	void setAutoindex(bool autoindex) { this->autoindex = autoindex; }
	void setAllowedMethods(string method) { this->allowed_methods.push_back(method); }
	void setRedirect(string redirect) { this->redirect = redirect; }
	void setDefaultFile(string default_file) { this->default_file = default_file; }
	void setCgiExt(string cgi_ext) { this->cgi_ext = cgi_ext; }
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
	std::vector<string> allowed_methods;
	string redirect;
	string default_file;
	string cgi_ext;
	string upload_store;
	string index;
	map<string, bool> directiveStatus;

	void initDirectiveStatus();
};

#endif // ROUTE_HPP
