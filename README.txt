Michael Giancola
CS 3516 A15
Program 1

*** OVERVIEW ***
My program is fully-functional according to the specification document.

*** COMPILATION & RUNNING ***
To compile both programs, run "make". The executables mg_MEClient and mg_MEServer will be made.
To run mg_MEClient, run "./mg_MEClient <Server Machine Name> [<MEClient.txt>]"
(If the Client is run without a text file, it accepts commands from standard in)
To run mg_MEServer, run "./mg_MEServer"
The client and server use the well-known port "1100"
To clean up compiled files, run "make clean"

*** NOTES ON FILES ***
As requested in the spec, mg_MEClient creates a MEClient.log file which contains all correspondance 
between client and server. Notice that the server's responses are prefixed by "S: " to improve readability.
Also, mg_MEServer creates a MEDatabase.txt file which contains the contents of the database when the server 
terminates.

*** SOURCE FILES ***
-Source code for the Client and Server are in mg_MEClient.c and mg_MEServer.c respectively.
-Source code for my implementation of Linked List is in LinkedList.c and LinkedList.h
-prog1.c and prog1.h contain functions, constant definitions, and includes that
 are common between the server and client programs.

*** NOTES ON USRAUTH ***
-usrAuth has three states: -1, 0, 1
- -1 indicates that no one is logged in
- 0  indicates that a Query user is logged in
- 1  indicates that a FEMA  user is logged in
