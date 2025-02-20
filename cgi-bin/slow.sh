#!/bin/sh
# Slowly read and write back the request

printf "HTTP/1.1 200 OK\r\n"
printf "Content-Type: text/plaint\r\n"
printf "\r\n"

env | while read line; do echo $line; sleep 1; done