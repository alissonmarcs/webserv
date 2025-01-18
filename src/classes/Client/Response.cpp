#include "Client.hpp"

void
Client::buildResponse()
{
    findRoute ();

    if (route == NULL)
        status_code = NOT_FOUND;
    else if (method == "GET")
        http_get ();
    
    if (haveError())
        buildError();
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
    static_file_name = route->getRoot() + target_resource;
    struct stat file_stat = {};

    if (stat(static_file_name.c_str(), &file_stat) == -1)
    {
        status_code = NOT_FOUND;
        return ;
    }

    ifstream file(static_file_name.c_str());
    stringstream content;

    content << file.rdbuf ();
    response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Length: " + to_string (content.str ().size ()) + "\r\n";
    response += findContentType ();
    response += "\r\n";
    response += content.str ();
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