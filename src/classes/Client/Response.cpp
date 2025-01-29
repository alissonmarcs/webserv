#include "Client.hpp"

bool isFile (const string & content);
string getFileName(const string & content);

void
Client::buildResponse()
{
    findRoute ();
    RouteValidation ();
    if (haveError())
        buildError();
    else if (method == "GET")
        http_get ();
    else if (method == "POST")
        http_post ();
}

void
Client::http_post()
{
    map<string, string>::iterator content_type = request_headers.find("content-type");

    if (content_type != request_headers.end() && content_type->second.find ("multipart/form-data") != string::npos && body.find("filename=\"") != string::npos)
        handleUpload(content_type);
}

void
Client::handleUpload(map<string, string>::iterator content_type)
{
    string boundary = getBoundary(content_type->second);
    vector<string> * parts = splitMultipart (boundary);
    vector<string>::iterator start, end;

    start = parts->begin();
    end = parts->end();
    while (start != end)
    {
        if (isFile(*start))
        {
            string file_name = getFileName(*start);

            ofstream file ((route->getRoot() + route->getUploadStore() + file_name).c_str());
            if (file.is_open() == false)
            {
                status_code = INTERNAL_SERVER_ERROR;
                return;
            }

            size_t tmp = start->find("\r\n\r\n") + 4;

            file.write(start->data() + tmp, start->size() - tmp);
            
            if (file.fail())
            {
                status_code = INTERNAL_SERVER_ERROR;
                return;
            }
            file.close();
        }
        start++;
    }
    delete parts;
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
        if (strncmp (last_boundary.data (), body.data () + end, last_boundary.size()) == 0)
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
        status_code = NOT_FOUND;
    else if (find(route->getAllowedMethods().begin(), route->getAllowedMethods().end(), method) == route->getAllowedMethods().end())
        status_code = METHOD_NOT_ALLOWED;
    else if (method == "GET")
        http_get_error_handling ();
}

void
Client::http_get_error_handling ()
{
    static_file_name = route->getRoot() + target_resource;
    file_exists_and_is_readable = (access((static_file_name.c_str()), R_OK) != -1) ? true : false;

    if (file_exists_and_is_readable == false) { status_code = NOT_FOUND; return; }
    if (stat(static_file_name.c_str(), &file_info) == -1) { status_code = INTERNAL_SERVER_ERROR; return; }

    if (S_ISDIR(file_info.st_mode) && static_file_name[static_file_name.size() - 1] != '/')
        status_code = MOVED_PERMANENTLY;
    else if (S_ISDIR(file_info.st_mode) && route->getAutoindex () == true && route->getIndex () != "")
    {
        string index = static_file_name + route->getIndex();

        if (access(index.c_str(), R_OK) != -1)
        {
            static_file_name += route->getIndex();
            index_is_valid = true;
        }
        else
            index_is_valid = false;
    }
    else if (S_ISDIR(file_info.st_mode) && route->getAutoindex () == false && route->getIndex () != "")
    {
        string index = static_file_name + route->getIndex();
        if (access(index.c_str(), R_OK) != -1)
            static_file_name += route->getIndex();
        else
            status_code = NOT_FOUND;
    }
    else if (S_ISDIR(file_info.st_mode) && route->getAutoindex () == false && route->getIndex () == "")
        status_code = FORBIDDEN;
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
Client::http_get ()
{
    if (route->getAutoindex() && S_ISDIR(file_info.st_mode) && index_is_valid == false)
        autoindex();
    else if (status_code == MOVED_PERMANENTLY)
    {
        response = "HTTP/1.1 301 Moved Permanently\r\n";
        response += "Location: " + target_resource + "/\r\n";
        response += "\r\n";
    }
    else
    {
        ifstream file(static_file_name.c_str());
        stringstream content;

        content << file.rdbuf ();
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Length: " + to_string (content.str ().size ()) + "\r\n";
        response += findContentType ();
        response += "\r\n";
        response += content.str ();
        file.close ();

        cout << BOLD "Response:\n" RESET;
        cout <<  response << endl;
    }
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
        status_code = INTERNAL_SERVER_ERROR;
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

bool
Client::targetResourceIsDir()
{
    return (target_resource[target_resource.size() - 1] == '/');
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
