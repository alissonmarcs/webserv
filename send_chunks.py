#!/usr/bin/env python3

import http.client

conn = http.client.HTTPConnection("0.0.0.0:9000")
conn.putrequest("POST", "/")
conn.putheader("Transfer-Encoding", "chunked")
conn.endheaders()

def generate_chunks():
    yield b"4\r\nWiki\r\n"
    yield b"5\r\npedia\r\n"
    yield b"0\r\n\r\n"

for chunk in generate_chunks():
    conn.send(chunk)

response = conn.getresponse()
print(response.read().decode())
conn.close()
