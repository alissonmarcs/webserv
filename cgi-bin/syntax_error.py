#!/usr/bin/env python3

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)
import cgi
import os

def print_headers(status_code, phase, content_type):
    print ("HTTP/1.1 %s %s" % (status_code, phase), end='\r\n')
    print ("Content-type: %s" % content_type, end='\r\n')
    print ("", end='\r\n')

print_headers("200", "OK", "text/html")
print "<h1>There is a syntax error in this script</h1>")