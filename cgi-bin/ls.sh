#!/usr/bin/env bash

printf "HTTP/1.1 200 OK\r\n"
printf "Content-Type: text/html\r\n"
printf "\r\n"

FOLDER="/nfs/homes/almarcos"

printf "<p>List of files in <code>$FOLDER</code></p>"

files=($(ls $FOLDER))

for file in "${files[@]}"
do
    printf "<code>$file</code><br>"
done