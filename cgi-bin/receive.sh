#!/usr/bin/env bash

function print_status ()
{
    printf "HTTP/1.1 %d %s\r\n" "$1" "$2"
    printf "Content-Type: text/html\r\n"
    printf "\r\n"
}

print_status 200 "OK"
printf "<>File received sucefully!!<\p>"
cat - > any.jpeg
exit 0