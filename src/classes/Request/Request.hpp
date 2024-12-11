#ifndef REQUEST_H
#define REQUEST_H

#include "Utils.hpp"

class Request
{
public:
    Request ();
    ~Request ();
    Request (const Request & src);

    string method;
    string path;
    string http_version;
    map<string, string> headers;
};

#endif