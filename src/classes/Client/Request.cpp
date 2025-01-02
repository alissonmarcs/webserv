#include "Client.hpp"
#include <sstream>

bool isValidMethod(string & method);
void printRequest (string & request);

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
    if (parseRequestLine (request) == false || parseHeaders (request) == false)
    {
        error_code = 400;
        return ;
    }
    // if (request_headers.count("content-length") || isChunked())
    // {
    //     parseBody(request);
    //     printBody();
    // }
    // parseBody(request);
    printHeaders();
    printRequest(request);
    // printBody();

}

void
printRequest (string & request)
{
    cout << "\t\t\n\n--- REQUEST ---\n\n";
    cout << request;
    cout << "\t\t\n--- END REQUEST ---\n" << endl;
}

void
Client::printBody()
{
    cout << "\t\t\n\n--- BODY ---\n\n";
    for (size_t i = 0; i < body.size(); i++)
    {
        char tmp = body[i];
        cout << tmp;
    }
    cout << "\t\t\n--- END BODY ---\n" << endl;
}

void
Client::parseBody(string & request)
{
    if (request_headers.count("content-length"))
    {
        int content_length = atoi(request_headers["content-length"].c_str());
        if (content_length < 0)
        {
            error_code = 400;
            return ;
        }
        else
        {
            for (int i = 0; i < content_length; i++)
            {
                char c = request[i];
                body.push_back(c);
            }
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
Client::parseRequestLine(string & request)
{
    if (!method.empty())
        return (true);

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
    request.erase(0, end_request_line + 2);
    return (true);
}

bool
Client::parseHeaders(string & request)
{
    if (request_headers.size() > 0)
        return (true);

    const size_t end_headers = request.find("\r\n\r\n");
    size_t end_request_line, double_dot;
    string name, value;

    for (size_t start = 0; start < end_headers; )
    {
        end_request_line = request.find("\r\n", start);
        double_dot = request.find(":", start);
        if (double_dot == string::npos || double_dot > end_request_line)
            return (false);
        name = request.substr(start, double_dot - start);
        double_dot += 1;
        value = request.substr(double_dot , end_request_line - double_dot);
        trim2(name);
        trim2(value);
        lowercase(name);
        lowercase(value);
        if (isValidToken(name) == false || isValidHeaderValue(value) == false)
            return (false);
        request_headers[name] = value;
        start = end_request_line + 2;
    }
    request.erase(0, end_headers + 4);
    return (true);
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