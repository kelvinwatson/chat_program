/*
 * TCPServer.c
Sources: http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 10     // how many pending connections queue will hold

void signalHandler(int s){
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char* argv[]){
		char *portNumber;
		if(argc<2){
			fprintf(stderr,"\n***ERROR: Missing port number in command-line argument: e.g. ./TCPserver port\n\n");
			exit(0);
		}
		else{
			portNumber = argv[1];
		}
    int status, welcomeSocket, connectionSocket, yes=1;  // listen on sock_fd, new connection on connectionSocket
    struct addrinfo hints, *serverInfo, *p;
    struct sockaddr_storage clientAddr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((status = getaddrinfo(NULL, portNumber, &hints, &serverInfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = serverInfo; p != NULL; p = p->ai_next) {
        if ((welcomeSocket = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (bind(welcomeSocket, p->ai_addr, p->ai_addrlen) == -1) {
            close(welcomeSocket);
            perror("server: bind");
            continue;
        }
        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(serverInfo); // all done with this structure
    
    if (listen(welcomeSocket, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    
    sa.sa_handler = signalHandler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    
    printf("\nSimple Chat System (by Kelvin Watson, OSU ID 932540242, onid: watsokel)");
    printf("\nServer: Waiting for client connections on port %s...\n",portNumber);
    
    while(1) {  // main accept() loop
        sin_size = sizeof clientAddr;
        connectionSocket = accept(welcomeSocket, (struct sockaddr *)&clientAddr, &sin_size);
        if (connectionSocket == -1) {
            perror("accept");
            continue;
        }
				
        if (!fork()) { // this is the child process
            close(welcomeSocket); // child doesn't need the listener
            if (send(connectionSocket, "Watson residence!", 17, 0) == -1)
                perror("send");
            close(connectionSocket);
            exit(0);
        }
        close(connectionSocket);  // parent doesn't need this
    }
    return 0;
}
