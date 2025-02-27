#!/usr/bin/env python3

import sys

def main():
    texto = sys.stdin.read()
    
    texto_upper = texto.upper()

    print("HTTP/1.1 200 OK", end='\r\n')
    print("Content-type: text/plain", end='\r\n')
    print("", end='\r\n')

    print ("Received from server: ", texto)
    print("Sending to server: ", texto_upper)

if __name__ == "__main__":
    main()