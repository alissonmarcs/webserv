#include "Client.hpp"

bool isFile (const string & content);
string getFileName(const string & content);

void
Client::buildResponse()
{
    if (haveError())
    {
        buildError();
        return ;
    }
    findRoute ();
    RouteValidation ();

    if (haveError())
        buildError();
    
    else if (method == "GET")
        http_get ();
    else if (method == "POST")
        http_post ();
    else if (method == "DELETE")
        http_delete ();

    if (haveError())
        buildError();
    response_is_done = true;
}

void
Client::http_delete()
{
    string file = route->getRoot() + target_resource;

    if (target_resource.find("..") != string::npos)
    {
        setError(FORBIDDEN);
        return;
    }
    if (access (file.c_str(), F_OK) == -1)
        setError(NOT_FOUND);
    else if (unlink (file.c_str()) == -1)
        setError(INTERNAL_SERVER_ERROR);
    else
        response = "HTTP/1.1 204 No Content\r\n\r\n";
}

void
Client::http_post()
{
    map<string, string>::iterator content_type = request_headers.find("content-type");

    if (content_type == request_headers.end() || content_type->second.find ("multipart/form-data") == string::npos)
        setError(UNSUPPORTED_MEDIA_TYPE);
    else if (body.find("filename=\"") != string::npos)
        handleUpload(content_type);
}

void
Client::validadeBodyPost (string & boundary)
{
    size_t start, end;

    start = body.find(boundary);
    end = body.find(boundary, start + boundary.size());
    if (start == string::npos || end == string::npos)
    {
        setError(BAD_REQUEST);
        return;
    }
    // our webserv allow only one file upload per request
    if (body.find (boundary, end + boundary.size()) != string::npos)
    {
        setError(BAD_REQUEST);
        return;
    }
    body = body.substr(start, end - start);
}

void
Client::handleUpload(map<string, string>::iterator content_type)
{
    string boundary = getBoundary(content_type->second);
    string file_name = getFileName(body);
    string complete_path = route->getRoot() + route->getUploadStore() + file_name;
    size_t end_headers = body.find("\r\n\r\n"); 
    
    validadeBodyPost (boundary);
    if (haveError())
        return ;
    if (end_headers == string::npos || file_name.empty())
    {
        setError(BAD_REQUEST);
        return ;
    }
    body.erase(0, end_headers + 4);
    ofstream file(complete_path.c_str(), ios::binary);
    if (file.is_open() == false)
    {
        setError(FORBIDDEN);
        return ;
    }
    file << body;
    response = "HTTP/1.1 204 No Content\r\n";
    response += "Server: any\r\n";
    response += "\r\n";
    file.close();
    response_is_done = true;
}

bool isFile (const string & content)
{
    if (content.find("filename=\"") == string::npos)
        return (false);
    return (true);
}

vector<string> *
Client::splitMultipart(string boundary)
{
    string last_boundary = boundary + "--";
    vector<string> * parts = new vector<string>;
    size_t start = 0, end = 0;

    while (1)
    {
        start = body.find(boundary, end);
        start += boundary.size();
        end = body.find(boundary, start);

        if (start == string::npos || end == string::npos)
            break;
        if (memcmp (last_boundary.data (), body.data () + end, last_boundary.size()) == 0)
            end -= 2;
        parts->push_back(body.substr(start, end - start));
    }
    return parts;
}

string
Client::getBoundary (string content_type)
{
    size_t start = content_type.find("boundary=");

    if (start == string::npos)
        return ("");
    return ("--" + content_type.substr(start + 9));
}

string
getFileName(const string & content)
{
    size_t start = content.find("filename=\"") + 10;
    size_t end = content.find("\"", start);

    return (content.substr(start, end - start));
}

void
Client::RouteValidation ()
{
    if (route == NULL)
        setError(NOT_FOUND);
    else if (find(route->getAllowedMethods().begin(), route->getAllowedMethods().end(), method) == route->getAllowedMethods().end())
        setError (METHOD_NOT_ALLOWED);
}


void
Client::redirectToFolderWithSlash()
{
    response = "HTTP/1.1 301 Moved Permanently\r\n";
    response += "Location: " + target_resource + "/\r\n";
    response += "\r\n";
}

void
Client::handleFolder ()
{
    if (S_ISDIR (file_info.st_mode) && target_resource[target_resource.size() - 1] != '/')
    {
        redirectToFolderWithSlash ();
        return;
    }
    if (route->getIndex() != "")
    {
        static_file_name += route->getIndex();
        memset (&file_info, 0, sizeof (file_info));
        stat (static_file_name.c_str(), &file_info);
    }
    if (S_ISREG (file_info.st_mode))
        loadStaticFile();
    else if (S_ISREG (file_info.st_mode) == false && route->getAutoindex() == true)
        autoindex();
    else if (S_ISDIR (file_info.st_mode))
    {
        if (route->getAutoindex() == true)
            autoindex();
        else
            setError(FORBIDDEN);
    }
    else
        setError(NOT_FOUND);
}

void
Client::loadStaticFile ()
{
    ifstream file(static_file_name.c_str());
    stringstream content;

    if (file.is_open() == false)
    {
        if (access(static_file_name.c_str(), F_OK) == -1)
            setError(NOT_FOUND);
        else
            setError(FORBIDDEN);
        return;
    }
    content << file.rdbuf ();
    response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Length: " + to_string (content.str ().size ()) + "\r\n";
    response += findContentType ();
    response += "\r\n";
    response += content.str ();
    file.close ();
}

void
Client::buildError()
{
    string * body = &server_owner->error_pages[status_code];

    response = "HTTP/1.1 " + to_string(status_code) + " " + getStatusText(status_code) + "\r\n";
    response += "Content-Length: " + to_string(body->size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += *body;
}

void
Client::locationRedirect()
{
    response = "HTTP/1.1 301 Moved Permanently\r\n";
    response += "Location: " + route->getRedirect() + "\r\n";
    response += "\r\n";
}

void
Client::http_get ()
{
    if (route->getRedirect() != "")
    {
        locationRedirect();
        return;
    }

    static_file_name = route->getRoot() + target_resource;
    stat (static_file_name.c_str(), &file_info);

    if (S_ISDIR(file_info.st_mode))
        handleFolder();
    else
        loadStaticFile();
}

void
Client::autoindex()
{
    string folder = route->getRoot() + target_resource;
    response = "HTTP/1.1 200 OK\r\n";
    DIR * dir = opendir (folder.c_str());
    struct dirent * entry = NULL;

    if (dir == NULL)
    {
        perror("opendir");
        setError(INTERNAL_SERVER_ERROR);
        return;
    }
    body = "<html>\n";
    body += "<head>\n";
    body += "<title>Index of " + target_resource + "</title>\n";
    body += "</head>\n";
    body += "<body>\n";
    body += "<h1>Index of " + target_resource + "</h1>\n";
    body += "<hr>\n";
    body += "<pre>\n";
    entry = readdir (dir);
    while (entry != NULL)
    {
        if (entry->d_type == DT_DIR)
            body += "<a href=\"" + string(entry->d_name) + "/\">" + entry->d_name + "/</a>\n";
        else
            body += "<a href=\"" + string(entry->d_name) + "\">" + entry->d_name + "</a>\n";
        entry = readdir (dir);
    }
    body += "</pre>\n";
    body += "<hr>\n";
    body += "</body>\n";
    body += "</html>\n";
    closedir (dir);
    response += "Content-Length: " + to_string(body.size()) + "\r\n";
    response += "Content-type: text/html\r\n";
    response += "\r\n";
    response += body;
}

void
Client::findRoute ()
{
    if (route != NULL)
        return ;

    vector<Route> & routes = server_owner->getRoutes();
    size_t size = routes.size(), diff_index = 0;

    for (size_t i = 0; i < size; i++)
    {
        if (target_resource.find(routes[i].getPath()) == 0) 
        {
            size_t diff_index_tmp = getDifferIndex(target_resource, routes[i].getPath());

            if (diff_index_tmp > diff_index)
            {
                diff_index = diff_index_tmp;
                route = &routes[i];
            }
        }
    }
}

string
Client::findContentType()
{
    size_t dot = static_file_name.rfind('.');
    string ext;

    if (dot == string::npos)
        return "Content-type: text/plain\r\n";
    ext = static_file_name.substr(dot + 1);
    if (ext == "html" || ext == "htm")
        return "Content-type: text/html\r\n";
    if (ext == "css")
        return "Content-type: text/css\r\n";
    if (ext == "js")
        return "Content-type: text/javascript\r\n";
    if (ext == "jpg" || ext == "jpeg")
        return "Content-type: image/jpeg\r\n";
    if (ext == "png")
        return "Content-type: image/png\r\n";
    if (ext == "gif")
        return "Content-type: image/gif\r\n";
    if (ext == "ico")
        return "Content-type: image/x-icon\r\n";
    if (ext == "mp3")
        return "Content-type: audio/mpeg\r\n";
    if (ext == "mp4")
        return "Content-type: video/mp4\r\n";
    return "Content-type: text/plain\r\n";
}
