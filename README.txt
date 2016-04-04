Chat Program (C server, Java client)
*** Before proceeding, please ensure that you have the 3 required files:
	chatserve.c
	makefile
	chatclient.java 
	TCPClient.java

*** INSTRUCTIONS FOR COMPILING, EXECUTING AND CONTROLLING CHATSERVE AND CHATCLIENT***

1. If running both programs on one computer, open two FLIP windows in PuTTY and navigate to the directory where the chatserver.c and chatclient.java are stored.

2. Compile the chatserve program on FLIP by typing: 	make
(alternatively, you could also type "make all" without the quotes)

3. Compile the chatclient program on FLIP by typing: 	javac chatclient.java TCPClient.java

4. Determine the server's IP address by typing "ifconfig" (without the quotes). The "inet addr" is the number you will need to execute the chatclient program. 
	- As an example for the following steps, we will assume the server's IP address is 128.193.54.226, and assume 	the server will use port number 30020, but you will enter the correct IP address for your machine, and  and the port number of your choosing.

5. Execute the chatserve program by typing:	./chatserve 30020 
(The format is "./chatserve <port_number>" without the quotes.)
The chatserve program is now waiting for a client connection.

6. Execute the chatclient program by typing:	java chatclient 128.193.54.226 30020
(The format is "java chatclient <IP_address> <port_number>" without the quotes.)

7. In the chatclient program, type your desired username up to 10 characters, and press <Enter>. This will be your handle for the chat conversation with the server.

8. In the chatclient program, type a message, and press <Enter>.

9. The chatserve program will receive that message, and display it on the console. Now it is the server's turn to send a message. Note that the server and client must take turns sending messages. The server must wait for a response from the client before sending the next message, and the client must wait for a response from the server before sending his/her next message. In the chatserve program, enter a message and press <Enter>.

10. Repeat steps 8 and 9 to exchange messages between chatclient and chatserve.

11. If chatclient would like to close the connection, in the chatclient program, type the command: \quit and press <Enter>. 
The chatserve program will then go back to waiting for a new connection from another client. To establish a new connection with the chatserve program, repeat starting from step 6.

12. If chatserve would like to close the connection, in the chatserve program, enter the command: \quit and press <Enter>. 
The chatserve program will then go back to waiting for a new connection from another client. To establish a new connection with the chatserve program, repeat starting from step 6.

13. If the chatclient would like to tell chatserve to interrupt (terminate its TCP connection and exit the program), then in the chatclient, type: SIGINT and press <Enter> to send the signal to chatserve. The chatserve program will then exit.

CODE SOURCES:
1. http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
2. https://systembash.com/a-simple-java-tcp-server-and-tcp-client/
3. http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
4. docs.oracle.com/javase/tutorial/networking/sockets/index.html
5. http://cboard.cprogramming.com/c-programming/98138-fflush-stdout.html

