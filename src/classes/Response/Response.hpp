#ifndef RESPONSE_H
#define RESPONSE_H

#include "Utils.hpp"
#include "Request.hpp"

class Response
{
public:
    Response ();
    ~Response ();
    Response (const Response & src);

    map<string, string> headers;
    Request * request;
};

#endif