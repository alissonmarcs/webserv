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

#define BAD_REQUEST_HTML "<html><head><title>400 Bad Request</title></head><body><center><h1>400 Bad Request</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define UNAUTHORIZED_HTML "<html><head><title>401 Unauthorized</title></head><body><center><h1>401 Unauthorized</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define FORBIDDEN_HTML "<html><head><title>403 Forbidden</title></head><body><center><h1>403 Forbidden</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define NOT_FOUND_HTML "<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define METHOD_NOT_ALLOWED_HTML "<html><head><title>405 Method Not Allowed</title></head><body><center><h1>405 Method Not Allowed</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define REQUEST_TIMEOUT_HTML "<html><head><title>408 Request Timeout</title></head><body><center><h1>408 Request Timeout</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define PAYLOAD_TOO_LARGE_HTML "<html><head><title>413 Payload Too Large</title></head><body><center><h1>413 Payload Too Large</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define INTERNAL_SERVER_ERROR_HTML "<html><head><title>500 Internal Server Error</title></head><body><center><h1>500 Internal Server Error</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define NOT_IMPLEMENTED_HTML "<html><head><title>501 Not Implemented</title></head><body><center><h1>501 Not Implemented</h1></center><hr><center>nginx/1.27.2</center></body></html>"
#define HTTP_VERSION_NOT_SUPPORTED_HTML "<html><head><title>505 HTTP Version Not Supported</title></head><body><center><h1>505 HTTP Version Not Supported</h1></center><hr><center>nginx/1.27.2</center></body></html>"
