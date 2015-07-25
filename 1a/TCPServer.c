/* Programmed by: Kelvin Watson
 * OSU ID: 932540242
 * ONID: watsokel
 * FileName: chatserve.c
 * Description: CS372 Assignment 1: Chat server for 2-way communication with client
 * Sources: http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
 * http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 */

#include <fcntl.h>
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
#include <time.h>

#define BACKLOG 10     // how many pending connections queue will hold
#define HANDLE_SIZE 11
#define MESSAGE_SIZE 500
#define CLEARBUFFER() char ch; while ((ch = getchar()) != '\n' && ch != EOF);

void signalHandler(int);
int sendAll(int,char*,int*);

int main(int argc, char* argv[]){
    char *portNumber;
    if(argc<2){
        fprintf(stderr,"\n***ERROR: Missing port number in command-line argument: e.g. ./TCPserver port\n\n");
        exit(0);
    }
    else portNumber = argv[1];

    printf("\n\nSimple Chat System (SERVER SIDE)");
    printf("\nProgrammed by Kelvin Watson, OSU ID 932540242, onid: watsokel)");
    printf("\n**************************************************************");
        
    /*printf("\nEnter a username (max 10 characters): ");
    fflush(stdout);
    char user[HANDLE_SIZE];     //extra space for null terminator
    fgets(user,HANDLE_SIZE,stdin);        //reads in newline and null terminator
    int length = (int)strlen(user);           //includes new line
    check for presence of newline
    int scroll=0;
    while(scroll!=10){
        if(user[scroll]=='\n'){
            user[length-1] = '\0';
            break;
        }
        scroll++;
    }
    if(length>=10){ 
        CLEARBUFFER()
    }
    //fflush(stdout);*/
    printf("\nWelcome to the chatserve. You are the server!\n");
    fflush(stdout);
    char handle[] = "server> ";
    /*size_t len = strlen(user);
    char* handle = malloc(len+3);   //extra space for null terminator
    strcpy(handle,user);
    handle[len] = '>';
    handle[len+1] = 32;
    handle[len+2] = '\0';*/
    size_t handleLen = strlen(handle);
    printf("\nYour handle is %s",handle);
    fflush(stdout);

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

    int waitForConnection=0;
    while(1){     
        
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


        printf("\nServer: Waiting for client connections on port %s...\n",portNumber);
        fflush(stdout);
        
        char SIGNAL_INT[] = "SIGINT";
        char response[MESSAGE_SIZE], input[MESSAGE_SIZE], chunk[MESSAGE_SIZE];
        memset(response,0,MESSAGE_SIZE);
        int charsRecv, partialCharsRecv, totalCharsRecv,sendStatus;
        
        while(1) {  // main accept() loop
            sin_size = sizeof clientAddr;
            connectionSocket = accept(welcomeSocket, (struct sockaddr *)&clientAddr, &sin_size);        /*open a connection socket*/
            fcntl(connectionSocket, F_SETFL, O_NONBLOCK);
            if(connectionSocket == -1) {
                perror("accept");
                continue;
            }
            int transferStarted = 0;
            int counter = 0;
            clock_t startTime;
            int child;
            if((child=fork())<0){ 
                perror("Fork Error");
                close(welcomeSocket);
                exit(-1);
            }
            if(child==0) {
                //close(welcomeSocket);
                while(1){ 
                    memset(response,0,MESSAGE_SIZE);
                    do{
                        memset(chunk,0,MESSAGE_SIZE);
                        charsRecv=recv(connectionSocket,chunk,MESSAGE_SIZE,0);
                        if(charsRecv==0){
                            waitForConnection=1;
                            break;
                        }
                        else if(charsRecv > 0){ //receive success 
                            if(transferStarted == 0) {
                                startTime = clock();
                                transferStarted = 1;
                            }
                            strcat(response,chunk);
                        }
                        else if(charsRecv < 0 && transferStarted == 1) {
                            if((clock() - startTime) > 1000) {
                                transferStarted = 0;
                                break;    
                            }
                        }
                    }while(1);
                    if(charsRecv==0 && waitForConnection==1) break;

                    response[charsRecv-1]= '\0';
                    
                    if(!strncmp(response,"SIGINT",6)){
                        waitForConnection=0;
                        printf("SIGINT received. Connection closed by client. Exiting program.");
                        fflush(stdout);
                        char *signalToClose = "terminate"; 
                        int terminateLen=strlen(signalToClose);
                        if (sendAll(connectionSocket, signalToClose, &terminateLen) == -1) {
                            perror("sendall");
                            printf("We only sent %d bytes because of the error!\n", terminateLen);
                        } 
                        exit(1);
                    } else if(!strncmp(response,"SIGQUIT",7)){
                        waitForConnection = 1;
                        char *signalToClose = "terminate"; 
                        int terminateLen=strlen(signalToClose);
                        if (sendAll(connectionSocket, signalToClose, &terminateLen) == -1) {
                            perror("sendall");
                            printf("We only sent %d bytes because of the error!\n", terminateLen);
                        } 
                        break;
                    }

                    printf("%s",response);
                    fflush(stdout);
                
                    printf("\n%s",handle);
                    fflush(stdout);
                        
                    char* fGetsStatus = fgets(input, MESSAGE_SIZE, stdin); //truncates string to the input length, PLACES NULL TERMINATOR FOR YOU
                    if((strncmp(input,"\\quit",5))==0){
                        printf("\n**TCP connection closed**\n");fflush(stdout);
                        waitForConnection = 1;
                        char *signalToClose = "terminate"; 
                        int terminateLen=strlen(signalToClose);
                        if (sendAll(connectionSocket, signalToClose, &terminateLen) == -1) {
                            perror("sendall");
                            printf("We only sent %d bytes because of the error!\n", terminateLen);
                        } 
                        break;
                    }
                    int inputLen = (int)strlen(input);
                    int messageLen = (int)(handleLen+inputLen);
                    char* message = (char*)malloc(messageLen);
                    strcpy(message,handle);
                    strcat(message,input);
                    int finalLen = strlen(message);
                    if (sendAll(connectionSocket, message, &finalLen) == -1) {
                        perror("sendall");
                        printf("We only sent %d bytes because of the error!\n", finalLen);
                    } 
                    free(message);
                }
                if(waitForConnection==1){
                    printf("Ready and waiting for new client connections on port %s...\n",portNumber);
                    close(connectionSocket);
                    break;
                }
                else{
                        printf("closing connection");
                        close(connectionSocket);
                //exit(0);
                }
            }
        } continue;
    }
    return 0;
}

void signalHandler(int s){
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

int sendAll(int s, char *buf, int *len){
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }
    *len = total; // return number actually sent here
    return n==-1?-1:0; // return -1 on failure, 0 on success
}
