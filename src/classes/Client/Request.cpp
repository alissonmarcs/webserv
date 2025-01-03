#include "Client.hpp"
#include <sstream>

bool isValidMethod(string & method);
void printRequest (string & request);
void printRequestBytes (string & request);

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
    parseRequestLine(request);
    parseHeaders(request);
    parseBody(request);
}

void
printRequest (string & request)
{
    cout << "\t\t\n\n--- REQUEST ---\n\n";
    cout << request;
    cout << "\t\t\n--- END REQUEST ---\n" << endl;
}

void
printRequestBytes (string & request)
{
    cout << "\t\t\n\n--- REQUEST BYTES ---\n\n";
    for (size_t i = 0; i < request.size(); i++)
    {
        int tmp = request[i];
        cout << tmp << " ";
    }
    cout << "\t\t\n--- END REQUEST ---\n" << endl;
}

void
Client::printBody()
{
    cout << "Body size: " << body.size() << endl;
    cout << "\t\t\n\n--- BODY ---\n\n";
    for (size_t i = 0; i < body.size(); i++)
    {
        int tmp = body[i];
        cout << tmp << " ";
    }
    cout << "\t\t\n--- END BODY ---\n" << endl;
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
Client::isChunked ()
{
    bool have_transfer_encoding = request_headers.count("transfer-encoding");
    bool is_chunked = (have_transfer_encoding && request_headers["transfer-encoding"] == "chunked");

    if (have_transfer_encoding && is_chunked)
        return (true);
    if (have_transfer_encoding && !is_chunked)
    {
        error_code = 501;
        return (false);
    }
    return (false);
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

void
Client::printHeaders()
{
    cout << "\t\t\n\n--- HEADERS ---\n\n";
    map<string, string>::iterator it = request_headers.begin();
    for (; it != request_headers.end(); it++)
        cout << it->first << ": " << it->second << endl;
    cout << "\t\t\n--- END HEADERS ---\n\n";

}