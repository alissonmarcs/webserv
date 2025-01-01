
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
	string getPath() const { return path; }
	string getRoot() const { return root; }
	bool getAutoindex() const { return autoindex; }
	std::vector<string> getAllowedMethods() const { return allowed_methods; }
	string getRedirect() const { return redirect; }
	string getDefaultFile() const { return default_file; }
	string getCgiExt() const { return cgi_ext; }
	string getUploadStore() const { return upload_store; }
	vector<string> getIndex() const { return indexFiles; }

	// Setters
	void setPath(string path) { this->path = path; }
	void setRoot(string root) { this->root = root; }
	void setAutoindex(bool autoindex) { this->autoindex = autoindex; }
	void setAllowedMethods(string method) { this->allowed_methods.push_back(method); }
	void setRedirect(string redirect) { this->redirect = redirect; }
	void setDefaultFile(string default_file) { this->default_file = default_file; }
	void setCgiExt(string cgi_ext) { this->cgi_ext = cgi_ext; }
	void setUploadStore(string upload_store) { this->upload_store = upload_store; }
	void setIndex(string index) { this->indexFiles.push_back(index); }

	// Route parsing
	int parseRouteConfig(const string &line, istringstream &stream, int &nestingLevel);
	void validDirective(const std::string &directive);
	void checkDuplicateDirectiveRoute(const string &line);

  private:
	string path;
	string root;
	bool autoindex;
	std::vector<string> allowed_methods;
	string redirect;
	string default_file;
	string cgi_ext;
	string upload_store;
	vector<string> indexFiles;
};

#endif // ROUTE_HPP
