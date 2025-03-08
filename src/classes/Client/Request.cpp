#include "Client.hpp"
#include "ServerManager.hpp"
#include <sstream>

extern ServerManager Manager;

bool
Client::readRequest ()
{
    char buffer[BUFFER_SIZE] = {0};
    int ret;

    ret = recv (client_fd, buffer, BUFFER_SIZE, 0);
    if (ret <= 0)
    {
        cout << "Error when reading from client's socket: "  << strerror(errno) << '\n';
        setError(INTERNAL_SERVER_ERROR);
        return false;
    }
    raw_request += string (buffer, ret);
    last_read = time(NULL);
    parseRequest ();
    return true;
}

void
Client::setError(short status_code)
{
    if (this->status_code != 0)
        return ;
    this->status_code = status_code;
    buildError();
    is_request_parsing_done = true;
    response_is_done = true;
}

void
Client::parseRequest ()
{
    size_t end_headers = raw_request.find("\r\n\r\n");

    if (method.empty () && raw_request.size () >= 400 && end_headers == string::npos)
    {
        setError (BAD_REQUEST);
        return ;
    }
    if (method.empty() && end_headers != string::npos)
    {
        parseRequestLine();
        if (!haveError())
            parseHeaders();
        
        if (request_headers.size () == 0 || request_headers.count("host") == 0)
        {
            setError(BAD_REQUEST);
            return ;
        }
    }
    if ((is_sized || is_chunked) && !haveError())
        parseBody();
    else if (!request_headers.empty())
        is_request_parsing_done = true;
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
    size_t end;
    string tmp_body;

    body += raw_request;
    raw_request.clear();
    end = body.rfind("0\r\n\r\n");
    if (body.size() > server_owner->getClientMaxBodySize() || (body.size() > 1000 && end == string::npos))
        { setError(PAYLOAD_TOO_LARGE); return ; }

    if (end != string::npos)
    {
        while (1)
        {

            size_t size_chunk = atoi(body.data());
            if (size_chunk == 0)
                break;
            size_t start_data = body.find("\r\n") + 2;
            size_t index = 0;

            for ( ; index < size_chunk; index++)
            {
                    tmp_body += body[start_data + index];
            }
            if (body[start_data + index] != '\r' || body[start_data + index + 1] != '\n')
                { setError(BAD_REQUEST); return ; }
            body.erase(0, start_data + index + 2);
        }
        request_headers.erase("transfer-encoding");
        is_request_parsing_done = true;
        body = tmp_body;
    }
}

void
Client::parseSizedBody()
{
    const size_t len = atoi(request_headers["content-length"].c_str());
    const size_t client_max_body_size = server_owner->getClientMaxBodySize();

    if (len >= HARD_MAX_BODY_SIZE)
    {
        setError(PAYLOAD_TOO_LARGE);
        return ;
    }
    body += raw_request;
    raw_request.clear();
    if (body.size() > len)
    {
        setError (BAD_REQUEST);
        return ;
    }
    if (body.size() == len && len > client_max_body_size)
        setError (PAYLOAD_TOO_LARGE);
    else if (body.size() == len)
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
        setError(BAD_REQUEST);
        return ;
    }
    
    size_t end_request_line = raw_request.find("\r\n");
    istringstream extractor(raw_request.substr(0, end_request_line));
    
    extractor >> method >> target_resource >> version;
    if (isValidMethod(method) == false || target_resource.empty() || version.empty())
    {
        setError(BAD_REQUEST);
        return ;
    }
    if (version != "HTTP/1.1")
    {
        setError(HTTP_VERSION_NOT_SUPPORTED);
        return ;
    }

    size_t query_index = target_resource.find("?");
    if (query_index != string::npos)
    {
        query_params = target_resource.substr(query_index + 1);
        target_resource.erase(query_index);
    }
    size_t dot = target_resource.find("."); 
    size_t slash = target_resource.find("/", dot); 
    if (dot != string::npos && slash != string::npos && dot < slash)
    {
        path_info = target_resource.substr(slash);
        target_resource.erase(slash);
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
            setError(BAD_REQUEST);
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
            setError(BAD_REQUEST);
            return ;
        }
        request_headers[name] = value;
        start = end_request_line + 2;
    }
    is_sized = request_headers.count("content-length");
    is_chunked = request_headers.count("transfer-encoding");
    if (is_sized && is_chunked)
        { setError(BAD_REQUEST); return ; }
    if (request_headers.count("host"))
        chooseServer();
    else
        { setError(BAD_REQUEST); return ; }
    raw_request.erase(0, end_headers + 4);
    if (is_sized == false && is_chunked == false && raw_request.size() != 0)
        setError(BAD_REQUEST);
}

void
Client::chooseServer()
{
    string host = getHeader("host");
    size_t double_dot = host.find(":");
    vector<Server> & servers = Manager.getServersRef();
    size_t index = 0;

    if (double_dot != string::npos)
        host = host.substr(0, double_dot);
    for ( ; index < servers.size(); index++)
    {
        if (servers[index].getServerName() == host)
        {
            server_owner = &servers[index];
            return ;
        }
    }
}

bool
Client::haveError()
{
    if (status_code >= 400 && status_code <= 599)
        return (true);
    return (false);
}