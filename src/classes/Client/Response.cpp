#include "Client.hpp"

void
Client::buildResponse()
{
    if (status_code)
        buildError ();
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
