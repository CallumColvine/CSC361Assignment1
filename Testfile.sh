echo -e -n "GET / HTTP/1.0\r\n\r\n" | nc -u -s 127.0.0.1 10.0.2.15 8080 &
echo -e -n "GET / HTTP/1.1\r\n\r\n" | nc -u -s 127.0.0.1 10.0.2.15 8080 &
echo -e -n "GET /nofile HTTP/1.0\r\n\r\n" | nc -u -s 127.0.0.1 10.0.2.15 8080 &