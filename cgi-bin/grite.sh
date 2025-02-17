#!/usr/bin/env bash

printf "HTTP/1.1 200 OK\r\n"
printf "Content-Type: text/html\r\n"
printf "\r\n"

OUTPUT=$(tr a-z A-Z)
printf "$OUTPUT\n"
