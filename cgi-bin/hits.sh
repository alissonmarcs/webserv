#!/bin/bash
 
printf "HTTP/1.1 200 OK\r\n"
printf "content-type: text/html\r\n"
printf "\r\n"

printf "<html> <head> <title> CGI script </title> </head>"
printf "<body>"

ARQ="/tmp/page.hits"

n="$(cat $ARQ 2> /dev/null)" || n=0
$((n=n+1))
printf $n > "$ARQ"

printf "
<h1>Esta pagina ja foi visualizada: $n vezes</h1>
<br>

</body>
</html>"