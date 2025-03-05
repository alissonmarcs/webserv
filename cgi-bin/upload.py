#!/usr/bin/env python3

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

import cgi
import os

def print_headers(status_code, phase, content_type):
    print ("HTTP/1.1 %s %s" % (status_code, phase), end='\r\n')
    print ("Content-type: %s" % content_type, end='\r\n')
    print ("", end='\r\n')

method = os.environ.get('REQUEST_METHOD', 'GET')
if method != 'POST':
    print_headers ("405", "Method Not Allowed", "text/html")
    print ("<h1>Error: Method Not Allowed</h1>")
    exit ()

UPLOAD_DIR = 'uploads'

if not os.path.exists(UPLOAD_DIR):
    os.makedirs(UPLOAD_DIR)

form = cgi.FieldStorage()

if 'file' in form:
    file_item = form['file']
    
    if file_item.filename:
        file_path = os.path.join(UPLOAD_DIR, os.path.basename(file_item.filename))
        
        with open(file_path, 'wb') as f:
            f.write(file_item.file.read())

        print_headers ("200", "OK", "text/html") 
        print(f"<h1>File <code>'{file_item.filename}'</code> saved sucessfully!</h1>")
    else:
        print_headers ("400", "Bad Request", "text/html")
        print ("<h1>Error: no file were send</h1>")
        exit ()
else:
    print_headers ("400", "Bad Request", "text/html")
    print("<h1>Error: no file were send</h1>")