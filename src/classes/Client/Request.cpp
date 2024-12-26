#include "Client.hpp"
#include <sstream>

bool isValidMethod(string & method);

void
Client::readRequest ()
{
    char buffer[BUFFER_SIZE];
    string request;
    int ret;
    
    ret = recv (client_fd, buffer, BUFFER_SIZE, 0);
    if (ret < 0)
    {
        LOGGER (getClientIp(&adress).c_str(), strerror (errno));
        error_code = 500;
        return ;
    }
    buffer[ret] = '\0';
    request = buffer;
    if (parseRequestLine (request) == false)
    {
        error_code = 400;
        return ;
    }
}

bool
isValidMethod(string & method)
{
    if (method.empty() || (method != "GET" && method != "POST" && method != "DELETE"))
        return (false);
    return (true);
}

bool
Client::parseRequestLine(string & request)
{
    size_t end_headers = request.find("\r\n\r\n");
    size_t end_request_line = request.find("\r\n"); 

    if (end_headers == string::npos || request.find("Host:") == string::npos)
        return (false);
    if (request.find("\n\r") != end_headers + 1)
        return (false);
    if (isalpha(request[0]) == false)
        return (false);

    for (size_t i = 0; i < end_request_line ; i++)
    {
        if ((request[i] == '\r' && request[i + 1] == '\r') || (request[i] == '\n' && request[i + 1] == '\n') || (request[i] == ' ' && request[i + 1] == ' '))
            return (false);
    }
    
    istringstream iss(request.substr(0, end_request_line));
    iss >> method >> target_resource >> version;
    if (isValidMethod(method) == false || target_resource.empty() || version.empty() || version != "HTTP/1.1")
        return (false);
    return (true);
}