#!/usr/bin/env bash

OUTPUT=$(tr a-z A-Z)
LEN=${#OUTPUT}

function print_status ()
{
    printf "HTTP/1.1 %d %s\r\n" "$1" "$2"
    printf "Content-Type: text/html\r\n"
    printf "\r\n"
}

if [ $LEN == 0 ]; then
    print_status 200 "OK"
    printf "<h1>Please provide some text</h1>"
    exit 0
fi

print_status 200 "OK"
printf "<p>$OUTPUT</p>"
exit 0