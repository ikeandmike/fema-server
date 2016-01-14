# fema-server
Mock client-server system for maintaining records related to bodies found at a natural disaster.

## History
I developed this system individually for an assignment in CS3516 (Computer Networks) at Worcester Polytechnic Institute during A-term 2015.

The project spec is available at http://users.wpi.edu/~mjgiancola/cs3516/FEMA-System.pdf

## Compilation & Running
To compile both programs, run "make". The executables mg_MEClient and mg_MEServer will be made.

To run mg_MEClient, run "./mg_MEClient <Server Machine Name> [<MEClient.txt>]"
(If the Client is run without a text file, it accepts commands from standard in)
To run mg_MEServer, run "./mg_MEServer"

The client and server use the well-known port "1100"
To clean up compiled files, run "make clean"

## Notes

### Source Files
- Source code for the Client and Server are in mg_MEClient.c and mg_MEServer.c respectively.
- Source code for my implementation of Linked List is in LinkedList.c and LinkedList.h
-prog1.c and prog1.h contain functions, constant definitions, and includes that
 are common between the server and client programs.

### Output Files
- As requested in the spec, mg_MEClient creates a MEClient.log file which contains all correspondance between client and server. Notice that the server's responses are prefixed by "S: " to improve readability.
- mg_MEServer creates a MEDatabase.txt file which contains the contents of the database when the server terminates.

### usrAuth
- usrAuth is a flag which has three states: -1, 0, 1
- -1 indicates that no one is logged in
-  0 indicates that a Query user is logged in
-  1 indicates that a FEMA  user is logged in
