#!/usr/bin/env python3

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)
import cgi
import os

def print_headers(status_code, phase, content_type):
    print ("HTTP/1.1 %s %s" % (status_code, phase), end='\r\n')
    print ("Content-type: %s" % content_type, end='\r\n')
    print ("", end='\r\n')

def is_prime(n):
    if n <= 1:
        return False
    for i in range(2, int(n**0.5) + 1):
        if n % i == 0:
            return False
    return True

content_length = int(os.environ.get('CONTENT_LENGTH', 0))
if content_length == 0:
    print_headers("400", "Bad Request", "text/html")
    print ("<h1>Please give some body in request</h1>")
    exit()

print_headers("200", "OK", "text/html")

form = cgi.FieldStorage()
number = int(form.getvalue('number', 0))

print(f"<h1>{number} {'is prime!' if is_prime(number) else 'is not prime.'}</h1>")