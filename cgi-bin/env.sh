#!  /usr/bin/env bash

printf 'HTTP/1.1 200 OK\r\n'
printf "Content-type: text/html\r\n"
printf "\r\n"

printf "<h3>Environment Variables:</h3>\n\n"

vars=($(env))

for var in ${vars[*]}
do
    printf "<code>$var</code><br>\n"
done