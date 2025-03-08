#!/bin/bash

function print_status ()
{
    printf "HTTP/1.1 %d %s\r\n" "$1" "$2"
    printf "Content-Type: text/html\r\n"
    printf "\r\n"
}

current_folder=$(pwd)

print_status 200 "OK"

printf "<h1>Folder from where the script was executed: <code>'$current_folder'</code></h1>"