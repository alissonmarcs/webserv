enum HTTP_STATUS_CODES
{
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    REQUEST_TIMEOUT = 408,
    PAYLOAD_TOO_LARGE = 413,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    HTTP_VERSION_NOT_SUPPORTED = 505
};

# define BODY_400 = "<title>400 Bad Request</title></head> <body> <center><h1>400 Bad Request</h1></center><hr><center>nginx/1.27.2</center></body></html>"
# define BODY_404 = "<title>404 Not Found</title></head> <body> <center><h1>404 Not Found</h1></center><hr><center>nginx/1.27.2</center></body></html>"
