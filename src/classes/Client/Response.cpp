#include "Client.hpp"

void
Client::buildResponse()
{
    if (error_code)
        buildError ();
}

void
Client::buildError()
{
    string * body = &server_owner->error_pages[error_code];

    response = "HTTP/1.1 " + to_string(error_code) + " " + getStatusText(error_code) + "\r\n";
    response += "Content-Length: " + to_string(body->size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += *body;
}
