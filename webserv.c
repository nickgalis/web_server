#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h> //For internet domain address 
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/wait.h>


#define MAX_BUF_SIZE 512 //d_name 256 so had to do 512
#define HTML_BUF_SIZE 10000
/*
Socket is created
accept() checks if there are any client requests on the listening socket socketfd
    If request exists, accept() will take the request and return new socket to communicate with client 


*/

void parsehttp(char *http, char *me, char *ui, char *ver)
{
    char *httpHeader = strtok(http, "\r\n"); 
    sscanf(httpHeader, "%s %s %s", me, ui, ver);  //method, uri, version 
}

//Function to list Directory 
void requestrDirLst(int clientFD, char *ui)
{

    char fullpath [MAX_BUF_SIZE];
    sprintf(fullpath, ".%s", ui); 

    DIR *dir = opendir(fullpath); 

    //404 Error code: Directory does not exist 
    if(dir == NULL){
        char notFound [] = "HTTP/1.1 404 Not Found\r\n"
                            "Content-Type: text/html; charset=UTF-8\r\n"
                            "\r\n"
                            "<h1>404 Directory Not Found</h1>";

        write(clientFD, notFound, strlen(notFound)); 
    }

    struct dirent* entity; 
    entity = readdir(dir); 

    //HTTP Header Request 
    char hdRequest [] = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html; charset=UTF-8\r\n"
                        "\r\n";
    write(clientFD, hdRequest, strlen(hdRequest));

    //Header for Webserver 
    char title [] = "<h1>Directory Listing</h1>"; 
    write(clientFD, title, strlen(title)); 
          
    
//**** Start HTML Document
    char *starthtml = "<html><head><title>Directory Listing</title></head><body><ul>"; 
    write(clientFD, starthtml, strlen(starthtml)); 


    while(entity != NULL)
    {
        char lstBuff [MAX_BUF_SIZE]; 
        snprintf(lstBuff, sizeof(lstBuff), "<li>%s</li>",entity->d_name); 
        write(clientFD, lstBuff, strlen(lstBuff));
        entity = readdir(dir); 
    } 

//**** END HTML DOCUMENT
    char *html_end = "</ul></body></html>"; 
    write(clientFD, html_end, strlen(html_end)); 
    
}

long get_file_size(FILE* file) {
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

// Function to handle static image request
void handle_image_request(int clientfd, const char* file_path, const char* mime_type) {
    FILE* image_file = fopen(file_path, "rb");  // Open image file in binary mode
    if (image_file == NULL) {
        char notFound [MAX_BUF_SIZE]; 
        
        strcpy(notFound,"HTTP/1.1 404 Not Found\r\n"
                        "Content-Type: text/html; charset=UTF-8\r\n"
                        "\r\n"
                        "<h1>404 Image Not Found</h1>");
       
        write(clientfd, notFound, strlen(notFound)); 
        return;
    }

    // Get the file size
    long file_size = get_file_size(image_file);

    // Allocate memory for binary data of image
    char* image_data = malloc(file_size);

    // Read image file into buffer
    fread(image_data, 1, file_size, image_file);

    // Prepare HTTP response headers
    char headerResponse[MAX_BUF_SIZE] = {0};
    sprintf(headerResponse, "HTTP/1.1 200 OK\r\nContent-Type: %s; charset=UTF-8\r\nContent-Length: %ld\r\n\r\n", mime_type, file_size);

    // Write HTTP headers
    write(clientfd, headerResponse, strlen(headerResponse));

    // Write binary data of image
    write(clientfd, image_data, file_size);

    free(image_data);
    fclose(image_file);
}

//Request HTML File 
void requestHTML(int clientfd, char *path)
{
    FILE *file = fopen(path, "r");  //Open with read permissions
    if(file == NULL)
    {
        char notFound [] = "HTTP/1.1 404 Not Found\r\n"
                            "Content-Type: text/html; charset=UTF-8\r\n"
                            "\r\n"
                            "<h1>404 HTML FIle Not Found</h1>";
        write(clientfd, notFound, strlen(notFound)); 
    }

    //Getting file size 
    int fileSize = get_file_size(file); 

    //Allocate memory for htmlData
    char *htmlData = (char *)malloc(fileSize);

    //Copying file data to htmlData 
    fread(htmlData, 1,fileSize, file);

    //sprintf for formatted output
    char headerResponse [MAX_BUF_SIZE]; 
    sprintf(headerResponse, "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html; charset=UTF-8\r\n"
                            "Content-Length: %d\r\n\r\n", fileSize); 

    write(clientfd, headerResponse, strlen(headerResponse));
    write(clientfd, htmlData, strlen(htmlData)); 

    free(htmlData); 
    //Closing file
    fclose(file); 
}

/*
Please make sure that the cgi file is exectuable 
using command chmod +x 
*/

void execute_CGI_script(int clientfd, char* fullpath) {
    int pathNotFound = 0; 
    int pipefd[2];
    int statuscode[2]; 
    //char notValid [2]; //Checks for validity of the file (set to 0 if not valid set to 1 if valid)
    //fd[0] - read / fd[1] - write
    char notValid [2];

    char script_output[MAX_BUF_SIZE];
    memset(script_output, 0, sizeof(script_output));


    // Create pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }
    if(pipe(statuscode) == -1)
    {
        perror("pipe");
        return; 
    }

    pid_t pid = fork();
    if(pid == 0)        //Child Process
    {
        // Redirect stdout to the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(statuscode[0]); 

        //Error mapping the CGI file 
        if(execl(fullpath, fullpath, NULL) == -1)
        {
            char notFound [] = "HTTP/1.1 404 Not Found\r\n"
                                "Content-Type: text/html; charset=UTF-8\r\n"
                                "\r\n"
                                "<h1>404 CGI Script Not Found</h1>";
            
            write(clientfd, notFound, strlen(notFound)); 
            write(statuscode[1], "0", 1); 
        }
        else
        {
            write(statuscode[1], "1", 1);
        }
        
        exit(0);
    }
    else if(pid > 0)    //Parent Process d
    {
        wait(NULL); //Force Child process to execute first 

        close(pipefd[1]);
        close(statuscode[1]); 

        read(statuscode[0], notValid, sizeof(notValid));

       if(notValid[0] != '0') // (-1) Failed status code (1) success status code
        {
            read(pipefd[0], script_output, sizeof(script_output));
            // create the HTTP response
            char headerResponse[MAX_BUF_SIZE] = {0};
            sprintf(headerResponse, "HTTP/1.1 200 OK\r\n");
            // Write HTTP headers
            write(clientfd, headerResponse, strlen(headerResponse));
            // Write output
            write(clientfd, script_output, strlen(script_output));
        }

        close(pipefd[0]); 
        close(statuscode[0]); 
        
    }
    else    //Failed to fork
    {
        perror("fork");
        return;
    }
}







int main(int argc, const char *argv[])
{
    int portNum = atoi(argv[1]); //Receive port number  
    char buffer [MAX_BUF_SIZE]; 

    char method[10]; //GET, POST, PUT
    char uri[25]; //The resource client wants to interact with 
    char version[10]; //Version of http  d

    struct sockaddr_in serverAddr; //Instance of sockaddr_in
    socklen_t lenServerAddr = sizeof(serverAddr); //Size of serverAddr
    
    int socketfd = socket(AF_INET, SOCK_STREAM, 0); //Internet socket w/h file like attributes read and write SockStream(TCP)
    if(socketfd < 0)
        perror("Fail to create socket"); 
    
    //CREATING SERVER IPv4 
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

        pid_t pid = fork(); 

        if(pid == 0)//Child process
        {
            close(socketfd);
            int valread = read(clientfd, buffer, sizeof(buffer)); 
            if(valread < 0)
                perror("Error (read)"); 
            printf("Connection accepted\n"); 
            printf("Connected to Client at %s %d\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));

            //Parsing HTTP
            parsehttp(buffer, method, uri, version); //Method, uri, version: will be intialized (GET /Request /HTTP1.1)

            //If the str not contain a . (Meaning its a directory)
            if(strchr(uri, '.') == NULL)
                requestrDirLst(clientfd, uri); //Sending (/response)
            else
            {
                if(strstr(uri, ".jpg") != NULL || strstr(uri, ".jpeg") != NULL) {
                    char file_path[MAX_BUF_SIZE];
                    sprintf(file_path, ".%s", uri);
                    handle_image_request(clientfd, file_path, "image/jpeg");
                }
                else if(strstr(uri, ".gif") != NULL) {
                    char file_path[MAX_BUF_SIZE];
                    sprintf(file_path, ".%s", uri);
                    handle_image_request(clientfd, file_path, "image/gif");
                }
                else if(strstr(uri, ".html") != NULL)
                {
                    char file_path[MAX_BUF_SIZE]; 
                    sprintf(file_path, ".%s", uri); 
                    requestHTML(clientfd, file_path); 
                }
                else if(strstr(uri, ".cgi") != NULL)
                {
                    char file_path[MAX_BUF_SIZE]; 
                    sprintf(file_path, ".%s", uri);  // Assuming the uri is a path relative to the current directory
                    execute_CGI_script(clientfd, file_path); 
                }
            }

            
            close(clientfd); //Have to close clientfd 
            exit(0); 
        }
        
        

    }






    return 0; 
}


/*
 char response [] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n"
"Content-Length: 13\r\n"
"\r\n"
"Hello, World!";

*/