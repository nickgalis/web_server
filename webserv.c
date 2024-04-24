#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h> //For internet domain address 
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
/*
Socket is created
accept() checks if there are any client requests on the listening socket socketfd
    If request exists, accept() will take the request and return new socket to communicate with client 


*/

char *parsehttp(char *http, char *me, char *ui, char *ver)
{
    char *httpHeader = strtok(http, "\r\n"); 
    sscanf(httpHeader, "%s %s %s", me, ui, ver);  //method, uri, version 
    return httpHeader; 

}

int main(int argc, const char *argv[])
{
    int portNum = atoi(argv[1]); //Receive port number  
    char buffer [256]; 

    char method[25]; //GET, POST, PUT
    char uri[100]; //The resource client wants to interact with 
    char version[10]; //Version of http 

    struct sockaddr_in serverAddr; //Instance of sockaddr_in
    socklen_t lenServerAddr = sizeof(serverAddr); //Size of serverAddr
    
    int socketfd = socket(AF_INET, SOCK_STREAM, 0); //Internet socket w/h file like attributes read and write SockStream(TCP)
    if(socketfd < 0)
        perror("Fail to create socket"); 
    
    //CREATING SERVER OF TYPE IPv4 
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; //IPv4 address 
    serverAddr.sin_port = htons(portNum);  //IP Port 
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //IP Address

    //Bind socket to IP address
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
        printf("Connected to Client at %s %d\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port)); 
        //read from socket 
        int valread = read(clientfd, buffer, sizeof(buffer));

        //printf("%s", buffer); 

        //Parsing HTTP
        char *parsedHTTP = parsehttp(buffer, method, uri, version);
        
        //Checking for specifics 
        char fullpath [200];
        sprintf(fullpath, ".%s", uri); 
        printf("%s\n", fullpath);

        DIR *dir = opendir(fullpath); 
        if(dir == NULL){
            perror("Fail file does not exist"); 
            return 1; 
        }
        struct dirent* entity; 
        entity = readdir(dir); 

        while(entity != NULL)
        {
            printf("%s\n",entity->d_name); 
            entity = readdir(dir); 
        } 



        if(valread < 0)
            perror("Error (read)"); 
        
      
        
        char response [] = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html; charset=UTF-8\r\n"
                            "Content-Length: 13\r\n"
                            "\r\n"
                            "Hello, World!";
        

        write(clientfd, response, strlen(response)-1);
        sleep(10); 
        close(clientfd);

    }






    return 0; 
}