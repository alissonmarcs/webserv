#include "Client.hpp"
#include <sstream>

bool isValidMethod(string & method);
void printRequest (string & request);

void
Client::readRequest ()
{
    char buffer[BUFFER_SIZE];
    int ret;
    
    ret = recv (client_fd, buffer, BUFFER_SIZE, 0);
    if (ret < 0)
    {
        LOGGER (getClientIp(&adress).c_str(), strerror (errno));
        error_code = 500;
        return ;
    }
    buffer[ret] = '\0';
    string request (buffer, ret);
    printRequest(request);
    parseRequestLine(request);
    parseHeaders(request);
    parseBody(request);
}

void
printRequest (string & request)
{
    cout << BOLD "\nRequest:\n" RESET BROWN;
    cout << request;
    cout << RESET << endl;
}

void
Client::parseBody(string & request)
{
    if (request_headers.count("content-length"))
    {
        size_t content_length = atoll(request_headers["content-length"].c_str());
        if (content_length > 1000000 || content_length != request.size())
        {
            error_code = 400;
            return ;
        }
        body = request;
        is_request_parsing_done = true;
    }
    else if (request_headers.count("transfer-encoding"))
    {
        if (request_headers["transfer-encoding"] != "chunked")
        {
            error_code = 501;
            return ;
        }
    }
}

bool
isValidToken (string & token)
{
    static const char special_bytes[] = {'!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '^', '_', '`', '|', '~'};
    const size_t special_bytes_size = sizeof(special_bytes) / sizeof(special_bytes[0]);
    int c;

    for (size_t i = -1; i < token.size(); i++)
    {
        c = token[i];
        if (isalnum(c))
            continue;
        for (size_t j = 0; j < special_bytes_size; j++)
        {
            if (c == special_bytes[j])
                break;
            if (j == special_bytes_size - 1)
                return (false);
        } 
    }
    return (true);
} 

bool
isValidHeaderValue(string & value)
{
    for (size_t i = 0; i < value.size(); i++)
    {
        if (isprint(value[i]) == false)
            return (false);
    }
    return (true);
}

bool
isValidMethod(string & method)
{
    if (method.empty() || (method != "GET" && method != "POST" && method != "DELETE"))
        return (false);
    return (true);
}

bool
isFormatValid (string & request)
{
    size_t end_headers = request.find("\r\n\r\n");
    size_t end_request_line = request.find("\r\n"); 

    if (end_headers == string::npos)
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
    return (true);
}

void
Client::parseRequestLine(string & request)
{
    if (method.empty() == false)
        return ;
    if (isFormatValid(request) == false)
    {
        error_code = 400;
        return ;
    }
    
    size_t end_request_line = request.find("\r\n");
    istringstream extractor(request.substr(0, end_request_line));
    
    extractor >> method >> target_resource >> version;
    if (isValidMethod(method) == false || target_resource.empty() || version.empty() || version != "HTTP/1.1")
    {
        error_code = 400;
        return ;
    }
    request.erase(0, end_request_line + 2);
}

void
Client::parseHeaders(string & request)
{
    if (request_headers.size() > 0)
        return ;

    const size_t end_headers = request.find("\r\n\r\n");
    size_t end_request_line, double_dot;
    string name, value;

    for (size_t start = 0; start < end_headers; )
    {
        end_request_line = request.find("\r\n", start);
        double_dot = request.find(":", start);
        if (double_dot == string::npos || double_dot > end_request_line)
        {
            error_code = 400;
            return ;
        }
        name = request.substr(start, double_dot - start);
        double_dot += 1;
        value = request.substr(double_dot , end_request_line - double_dot);
        trim2(name);
        trim2(value);
        lowercase(name);
        lowercase(value);
        if (isValidToken(name) == false || isValidHeaderValue(value) == false)
        {
            error_code = 400;
            return ;
        }
        request_headers[name] = value;
        start = end_request_line + 2;
    }
    request.erase(0, end_headers + 4);
}
