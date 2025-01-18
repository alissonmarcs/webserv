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
    string file_name = route->getRoot() + target_resource;
    struct stat file_stat = {};

    if (stat(file_name.c_str(), &file_stat) == -1)
    {
        cout << "file not found: " << file_name << endl;
        perror ("stat");
        status_code = NOT_FOUND;
        return ;
    }

    ifstream file(file_name.c_str());
    stringstream content;

    content << file.rdbuf ();
    response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Length: " + to_string (content.str ().size ()) + "\r\n";
    response += "Content-tpye: text/html\r\n\r\n";
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