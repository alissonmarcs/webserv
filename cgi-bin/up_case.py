#!/usr/bin/env python3

import os
import sys
from urllib.parse import unquote

def print_headers(status_code, phase, content_type):
    print ("HTTP/1.1 %s %s" % (status_code, phase), end='\r\n')
    print ("Content-type: %s" % content_type, end='\r\n')
    print ("", end='\r\n')

def main():
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    method = os.environ.get('REQUEST_METHOD', '')
    encoded_string = sys.stdin.read(content_length)

    if (method != 'POST'):
        print_headers("405", "Method Not Allowed", "text/html")
        print ("<h1>Method Not Allowed</h1>")
        return
    elif (content_length == 0):
        print_headers("400", "Bad Request", "text/html")
        print ("<h1>Please give some body in request</h1>")
        return
    decoded_string = unquote(encoded_string)
    print_headers("200", "OK", "text/html")
    print ("<h1>Typed string in upper case:</h1>")
    if '=' in decoded_string:
        key, value = decoded_string.split('=', 1)
        if key == 'content':
            value = value.replace('+', ' ')
            paragraphs = value.split('\n')
            for paragraph in paragraphs:
                print ("<p>")
                print(paragraph.upper())
                print ("</p>")

if __name__ == "__main__":
    main()