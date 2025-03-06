#!/usr/bin/env python3

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

import cgi
import random
import string

def print_headers(status_code, phase, content_type):
    print ("HTTP/1.1 %s %s" % (status_code, phase), end='\r\n')
    print ("Content-type: %s" % content_type, end='\r\n')
    print ("", end='\r\n')

def generate_password(length, upper_case, lower_case, numbers, symbols):
    caracteres = ""

    if upper_case:
        caracteres += string.ascii_uppercase
    if numbers:
        caracteres += string.digits
    if symbols:
        caracteres += string.punctuation
    if lower_case:
        caracteres += string.ascii_lowercase

    senha = ''.join(random.choice(caracteres) for _ in range(length))
    return senha

form = cgi.FieldStorage()

length = int(form.getvalue('length', 8))  # Valor padrão é 8
upper_case = form.getvalue('uppercase') == '1'
lower_case = form.getvalue('lowercase') == '1'
numbers = form.getvalue('numbers') == '1'
symbols = form.getvalue('symbols') == '1'

if (not upper_case and not lower_case and not numbers and not symbols):
    print_headers ("200", "OK", "text/html")
    print ("<h1>At least one option must be selected!</h1>")
    exit ()

password = generate_password(length, upper_case, lower_case, numbers, symbols)

print_headers ("200", "OK", "text/html")

print(f"<h1>Your password was generated!</h1>")
print(f"<p style='font-size: 24px; font-weight: bold;'>{password}</p>")