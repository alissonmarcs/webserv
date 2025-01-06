#include "Client.hpp"
#include <sstream>

bool isValidMethod(string & method);
void printRequest (string & request);
bool ChunkedBodyEnded(string & request);

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
    raw_request = string (buffer, ret);
    printRequest();
    parseRequestLine();
    if (error_code == 0)
        parseHeaders();
    if (error_code == 0)
        parseBody();
}

void
Client::printRequest ()
{
    cout << BOLD "\nRequest:\n" RESET BROWN;
    cout << raw_request;
    cout << RESET << endl;
}

int
Client::isInvalidBody(string & request)
{
    bool content_length = request_headers.count("content-length");
    bool transfer_encoding = request_headers.count("transfer-encoding");
    bool body = request.size() > 0;

    if (content_length && transfer_encoding)
        return (400);
    else if (!body && (content_length || transfer_encoding))
        return (400);
    else if (body && !content_length && !transfer_encoding)
        return (400);
    else if (transfer_encoding && request_headers["transfer-encoding"] != "chunked")
        return (501);
    else if (transfer_encoding)
        is_chunked = true;
    return (0);
}

void
Client::parseBody()
{
    if ((error_code = isInvalidBody(raw_request)))
        return ;
    if (is_chunked)
        parseChunkedBody();
    else
        parseSizedBody();
}

void
Client::parseChunkedBody ()
{
}

void
Client::parseSizedBody()
{
}

void
Client::removeChunkedDelimiters ()
{
    
}

bool
haveLastChunk(string & request)
{
    size_t start_index = request.size() - 5;

    size_t result = request.find("0\r\n\r\n", start_index);
    if (result == string::npos)
        return (false);
    return (true);
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
Client::parseRequestLine()
{
    if (method.empty() == false)
        return ;
    if (isFormatValid(raw_request) == false)
    {
        error_code = 400;
        return ;
    }
    
    size_t end_request_line = raw_request.find("\r\n");
    istringstream extractor(raw_request.substr(0, end_request_line));
    
    extractor >> method >> target_resource >> version;
    if (isValidMethod(method) == false || target_resource.empty() || version.empty())
    {
        error_code = 400;
        return ;
    }
    if (version != "HTTP/1.1")
    {
        error_code = 505;
        return ;
    }
    raw_request.erase(0, end_request_line + 2);
}

void
Client::parseHeaders()
{
    if (request_headers.size() > 0)
        return ;

    const size_t end_headers = raw_request.find("\r\n\r\n");
    size_t end_request_line, double_dot;
    string name, value;

    for (size_t start = 0; start < end_headers; )
    {
        end_request_line = raw_request.find("\r\n", start);
        double_dot = raw_request.find(":", start);
        if (double_dot == string::npos || double_dot > end_request_line || raw_request[double_dot - 1] == ' ')
        {
            error_code = 400;
            return ;
        }
        name = raw_request.substr(start, double_dot - start);
        double_dot += 1;
        value = raw_request.substr(double_dot , end_request_line - double_dot);
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
    raw_request.erase(0, end_headers + 4);
}
