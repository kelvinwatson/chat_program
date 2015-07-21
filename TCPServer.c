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
#define HANDLE_SIZE 11
#define MESSAGE_SIZE 500

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
    
    printf("\n\nSimple Chat System (SERVER SIDE)");
		printf("\nProgrammed by Kelvin Watson, OSU ID 932540242, onid: watsokel)");
    printf("\n**************************************************************");
		
		//getUserInput();
		printf("\nEnter a username (max 10 characters): ");
		fflush(stdout);
        char user[11];		//extra space for null terminator
		fgets(user,HANDLE_SIZE-1,stdin);
        //need to remove new line from the user!
        int length = (int)strlen(user);
        //printf("length of user %d\n",length);
        //printf("second last char in ASCII num is originally %d\n",user[length-1]);
        //printf("last char in ASCII num is originally %d\n",user[length]);           
        user[(int)((int)strlen(user)-1)] = '\0';
        length = (int)strlen(user);
        //printf("length of user after null terminator appended %d\n",length);
        //printf("last char in ASCII num after null terminator appended %d\n",  user[length-1]);
                                
        //fflush(stdout);
		printf("Welcome %s!\n",user);
		fflush(stdout);
        size_t len = strlen(user);
		char* handle = malloc(len+3); 	//extra space for null terminator
		strcpy(handle,user);
		handle[len] = '>';
		handle[len+1] = 32;
		handle[len+2] = '\0';
		size_t handleLen = strlen(handle);
		printf("\nYour handle is %s",handle);
		fflush(stdout);
        printf("\nServer: Waiting for client connections on port %s...\n",portNumber);
        fflush(stdout);
		
        char response[MESSAGE_SIZE], input[MESSAGE_SIZE];
		memset(response,0,MESSAGE_SIZE);
		int charsRecv,sendStatus;
        while(1) {  // main accept() loop
            sin_size = sizeof clientAddr;
        /*open a connection socket*/
			connectionSocket = accept(welcomeSocket, (struct sockaddr *)&clientAddr, &sin_size);
            if (connectionSocket == -1) {
                perror("accept");
                continue;
            }
            if (!fork()) {
                close(welcomeSocket);
    				while(1){
    					if((charsRecv=recv(connectionSocket,response,MESSAGE_SIZE,0))==-1){
    					 		//error receiving
    							perror("receive");
    					}
    					else if(charsRecv==0){
    					 		//client closed the connection	
    							break;
    					}
    					else{ //receive success	
                            //printf("numChars received = %d\n",charsRecv);
                            //printf("last char in ASCII num is %d\n",response[charsRecv-1]);
                            response[charsRecv]= '\0'; //append null terminator to message, which means thre is newline then null
                            //printf("last char in ASCII num is now %d\n",response[charsRecv]);
                            printf("%s",response);
                            fflush(stdout); // Prints to screen or whatever your standard out is
    					}
    					
    					printf("%s",handle);
                        //Clear the buffer
                        fflush(stdout); // Prints to screen or whatever your standard out is
                            
                        //NOTE scanf stops reading at space! Must use fgets
                        //memset(input,0,MESSAGE_SIZE);
                        //fflush(stdin);
                        fgets(input, MESSAGE_SIZE-1, stdin); //truncates string to the input length, PLACES NULL TERMINATOR FOR YOU
    					//printf("you just typed %s\n",input);
                        //fflush(stdout);
                        
                        int inputLen = (int)strlen(input);
    					printf("length of input is %d and",inputLen);
                        fflush(stdout);
                        printf("last char is %d\n",input[inputLen]);
                        fflush(stdout);
                        int messageLen = (int)(handleLen+inputLen);
    					char* message = (char*)malloc(messageLen);
    					strcpy(message,handle);
    					strcat(message,input);
    					if((sendStatus=send(connectionSocket,message,messageLen,0))<=-1) {
    						perror("send");
    					}
    					else{
    						//printf("sending message:%s",message);
                            fflush(stdout);
    					}
                        //printf("send success on this side!123");
                        fflush(stdout);
    				}
    				close(connectionSocket);
                exit(0);
            }
            close(connectionSocket);  // parent doesn't need this
        }
    return 0;
}
