#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>


int main(int argc, const char *argv[])
{
    int portNum = atoi(argv[1]); //Receive port number  
    char buffer [256]; 

    struct sockaddr_in serverAddr; //Instance of sockaddr_in
    socklen_t lenServerAddr = sizeof(serverAddr); //Size of serverAddr
    
    int socketfd = socket(AF_INET, SOCK_STREAM, 0); //Internet socket w/h file like attributes read and write (TCP)
    if(socketfd < 0)
        perror("Fail to create socket"); 
    
    //Binding socket to server "Esentially I will receive infomration here from client"
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; //IPv4 address 
    serverAddr.sin_port = htons(portNum);  //Convert into uint 16 
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //Bind socket to address 
    if(bind(socketfd, (struct sockaddr*)&serverAddr, lenServerAddr) < 0)
        perror("Fail to bind socket"); 
    printf("Socket sucesfful bound to server\n"); 


    if(listen(socketfd, 10) < 0) //Backlog queue 10 
        perror("Fail to set server to listen ");
    printf("Server listening for connections\n");

    while(1){
        struct sockaddr_in cliAddr;
        socklen_t clilen = sizeof(cliAddr);
        int clientfd = accept(socketfd, (struct sockaddr*) &cliAddr, &clilen); 
        if(clientfd < 0)
            perror("Error on accept"); 
        
        printf("Connection accepted\n"); 

        //read from socket 
        int valread = read(clientfd, buffer, sizeof(buffer));
        if(valread < 0)
            perror("Error (read)"); 
        
        char response [] = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html; charset=UTF-8\r\n"
                            "Content-Length: 13\r\n"
                            "\r\n"
                            "Hello, World!";

        write(clientfd, response, strlen(response)-1); 
   
    



    }






    return 0; 
}