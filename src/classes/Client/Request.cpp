#include "Client.hpp"
#include <sstream>

void
Client::readRequest ()
{
    char buffer[BUFFER_SIZE] = {0};
    int ret;

    ret = recv (client_fd, buffer, BUFFER_SIZE, 0);
    if (ret < 0)
    {
        LOGGER (getClientIp(&adress).c_str(), strerror (errno));
        error_code = 500;
        return ;
    }
    raw_request += string (buffer, ret);
    last_read = time(NULL);
    parseRequest ();
}

void
Client::parseRequest ()
{
    if (method.empty() && raw_request.find("\r\n\r\n") != string::npos)
    {
        printRequest();
        parseRequestLine();
        if (error_code == 0)
            parseHeaders();
    }
    if ((is_sized || is_chunked) && error_code == 0)
        parseBody();
}

void
Client::printRequest ()
{
    cout << BOLD "\nRequest:\n" RESET BROWN;
    cout << raw_request;
    cout << RESET;
}

void
Client::parseBody()
{
    if (is_chunked)
        parseChunkedBody();
    else
        parseSizedBody();
}

void
Client::parseChunkedBody ()
{
    body += raw_request;
    raw_request.clear();
    size_t start, end, size, size_index = 0;

    end = body.find("0\r\n\r\n");
    if (end != string::npos)
    {
        cout << "body before:\n" << body << endl;
        while (1) 
        {
            start = body.find("\r\n", size_index);
            stringstream ss(body.substr(size_index, start - size_index));

            ss << hex;
            ss >> size;

            if (size == 0)
            {
                body.erase(start);
                is_request_parsing_done = true;
                break;
            }

            body.erase(size_index, (start - size_index) + 2);
            if (body[size] != '\r' || body[size + 1] != '\n')
            {
                error_code = 400;
                return ;
            }
            body.erase(size, 2);
            size_index = size;
        }
        cout << "body after: " << body << endl;
    }
}

void
Client::parseSizedBody()
{
    const size_t len = atoi(request_headers["content-length"].c_str());

    body += raw_request;
    raw_request.clear();
    if (body.size() > len)
    {
        error_code = 400;
        return ;
    }
    if (body.size() == len)
        is_request_parsing_done = true;
}

bool
isValidHeaderName (string & token)
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

    size_t query_index = target_resource.find("?");
    if (query_index != string::npos)
    {
        query_params = target_resource.substr(query_index + 1);
        target_resource.erase(query_index);
    }

    raw_request.erase(0, end_request_line + 2);
}

void
Client::parseHeaders()
{
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
        trim(name);
        trim(value);
        lowercase(name);
        lowercase(value);
        if (isValidHeaderName(name) == false || isValidHeaderValue(value) == false)
        {
            error_code = 400;
            return ;
        }
        request_headers[name] = value;
        start = end_request_line + 2;
    }
    is_sized = request_headers.count("content-length");
    is_chunked = request_headers.count("transfer-encoding");
    if (is_sized && is_chunked)
    {
        error_code = 400;
        return ;
    }
    raw_request.erase(0, end_headers + 4);
}
