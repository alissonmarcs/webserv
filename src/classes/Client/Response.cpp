#include "Client.hpp"

void
Client::buildResponse()
{
    if (method == "GET")
        http_get ();
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
    Route * route = get_route ();
    string file_name = route->getRoot() + target_resource;
    struct stat file_stat;

    file_name.insert(0, ".");
    if (stat(file_name.c_str(), &file_stat) == -1)
    {
        perror ("stat");
        status_code = NOT_FOUND;
        return ;
    }

    ifstream file(file_name.c_str());
    stringstream content;
    string content_str;

    content << file.rdbuf();
    content_str = content.str();
    response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Length: " + to_string (content_str.size()) + "\r\n";
    response += "Content-tpye: text/html\r\n\r\n";
    response += content_str;
    
}

Route *
Client::get_route()
{
    vector<Route> & routes = server_owner->getRoutes();
    Route * route = NULL;
    size_t size = routes.size(), index_start = 0, index_differ = 0;

    for (size_t i = 0; i < size; i++)
    {
        if ((index_start = target_resource.find(routes[i].getPath())) != string::npos)
        {
            size_t index_differ_tmp = getDifferIndex(target_resource, routes[i].getPath());

            if (index_differ_tmp > index_differ)
            {
                index_differ = index_differ_tmp;
                route = &routes[i];
            }
        }
    }
    return (route);
}