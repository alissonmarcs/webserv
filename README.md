<div align="center">
	<h1>Webserv</h1>
	<img src="https://raw.githubusercontent.com/ayogun/42-project-badges/refs/heads/main/badges/webservm.png" alt="Webserver project badge of 42"/>
	<p align="center">A 42 project where we have to create our own HTTP/1.1 web server from scratch, using only C++ 98 Standard Library</p>
</div>

<div align="center">
	<h2>Final score</h2>
	<img src="https://i.imgur.com/6tYI7iy.png" alt="Project scored with 110/100">
</div>

## How to compile

```
git clone https://github.com/alissonmarcs/webserv.git
cd webserv
make
```

## How to use

```
./webserv configs/some_config.conf
```

## Config file format

```nginx
# configs/cgi.conf
server
{
    host 127.0.0.1;                                 # Ip to attach on.
    listen 8000;                                    # Port to listen.

    client_max_body_size 10M;                       # When request has body, limit its size. When no prefixes are used, the integer is interpreted as bytes. You can use M and G prefixes.

    error_page 504 error_pages/504.html;            # Define custom error pages. When none are defined, the default is used.

    location /configs/
    {
        root ./;

        autoindex on;                               # Enable/disable the listing of an folder when the request asks for an folder.
        allowed_methods GET;                        # Listing of allowed methods
    }
}
```

## How work `location` ?

```nginx
location /car
{
    root www;
    index other_index.html;
    allowed_methods GET;
}
```

The location path, `/car`, is for select what location will process the request.

```nginx
location /forms
{
    #...
}

location /cgi-bin/
{
    #...
}
```

`GET /forms/is_prime.html` will be processed by `location /forms`.

`GET /cgi-bin/upload.py` will be processed by `location /cgi-bin`.

`GET /demo/some_file.html` match with no location, so the server will response `404 Not Found`.

## How work `root` ?

`root` tells in what folder the requested file will be searched for.

```nginx
location /tech
{
    root  www;
}

location /forms
{
    root ./;
}
```

`GET /tech/index.html` will return `./www/tech/index.html`, or `404 Not Found` if the file is not found.

`GET /forms/up_case.html` will return `./forms/up_case.html`.

The root value can be set as relative or absolute.

##  How `autoindex` and `index` work ?

### `autoindex`

When an folder were requested, you can enable the listing of folder.

```nginx
location /tech
{
    root  www;
    autoindex on;
    allowed_methods GET;
}
```

`GET /tech` will return the listing of `./www/tech/`

### `index`

When an folder were requested, you can set an default file to be served.

```nginx
location /tech
{
    root  www;
    index services.html
    allowed_methods GET;
}
```

`GET /tech` will return `./www/tech/services.html`.

### both `index` and `autoindex`

```nginx
location /tech
{
    root  www;
    index null.html
    autoindex on;
    allowed_methods GET;
}
```

When both `index` and `autoindex` are enabled, if the file specified in `index` really exists, it will be returned, if not, the listing of folder will be returned.

`GET /tech` will return the listing of `./www/tech/`.

## How work `client_max_body_size` ?

You can limit the size of body of `POST` requests.
Use no prefix for bytes, M for megabytes, or G for gigabytes.




