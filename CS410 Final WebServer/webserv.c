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
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(socketfd, (struct sockaddr*)&serverAddr, lenServerAddr) < 0)
        perror("Fail to bind socket"); 

    if(listen(socketfd, 10) < 0) //Backlog queue 10 
        perror("Fail to set server to listen ");

    while(1){
        struct sockaddr_in cliAddr;
        socklen_t clilen = sizeof(cliAddr);
        int newsockfd = accept(socketfd, (struct sockaddr*) &cliAddr, &clilen); 
        if(newsockfd < 0)
            perror("Error on accept"); 
        
        memset(buffer, 0, 256); 
        int n = read(newsockfd, buffer, sizeof(buffer)); 
        if(n < 0)
            perror("Error reading"); 
        printf("Reading: %s\n", buffer); 




    }






    return 0; 
}