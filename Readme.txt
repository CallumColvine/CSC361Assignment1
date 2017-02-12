Callum Colvine
V00789488
B06

NOTE: For testing, there must be a file cantRead with no permissions. This could
not be zipped. It can be easily created and set-up with the command:
$ touch www/cantRead
$ chmod 000 www/cantRead

sws.cpp is a server that takes in UDP requests and replies with the codes:
200 OK, 400 Bad request, and 404 Not found.
It can be used to GET text files. It starts by reading input from the command
line for port and file to look in. The user may press 'q', or type any string
that starts with 'q' to exit the program. This could be changed with string
comparison, but was not specified so was left as is.  The program uses the same
socket for incoming and outgoing requests. The program will check if a file is
real and return error or OK codes based on that. Once checking if it is real,
the server will make a large string containing the OK response and the contents
of the file and send that in as few pieces as possible. The buffer size is
hard-coded to 200 but could be easily changed/specified with a global variable.

No bonuses were implemented. All basic functionality was completed.

To test, run:
$ ./Testfile.sh
