#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h> //For internet domain address
#include <arpa/inet.h>
#include <dirent.h>
#include <libgen.h>
#include <limits.h>

#define BACKLOG_QUEUE 10


char *parsehttp(char *http, char *me, char *ui, char *ver)
{
    char *httpHeader = strtok(http, "\r\n");
    sscanf(httpHeader, "%s %s %s", me, ui, ver);  //method, uri, version
    return httpHeader;

}

void get_dir_path(char* buffer, size_t len) {
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count != -1) {
        dirname(path);
        strncpy(buffer, path, len);
    }
}

int handle_cgi_request(int clientfd) {
    char path_to_script[PATH_MAX];
    get_dir_path(path_to_script, PATH_MAX);
    strncat(path_to_script, "/script.cgi", PATH_MAX - strlen(path_to_script) - 1);

    char *argv[2];
    char *envp[1];

    argv[0] = path_to_script;
    argv[1] = NULL;
    envp[0] = NULL;

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }
    if (pid == 0) {
        // This is the child process
        dup2(clientfd, STDOUT_FILENO);  // Redirect stdout to the client socket
        close(clientfd);

        execve(argv[0], &argv[0], envp);
        perror("execve");   // execve() only returns on error
        exit(EXIT_FAILURE);
    } else {
        // This is the parent process
        waitpid(pid, NULL, 0);  // Wait for child process to finish
    }

    return 0;
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
        perror("Error opening image file");
        return;
    }

    // Get the file size
    long file_size = get_file_size(image_file);

    // Allocate memory for binary data of image
    char* image_data = malloc(file_size);

    // Read image file into buffer
    fread(image_data, 1, file_size, image_file);

    // Prepare HTTP response headers
    char response[256] = {0};
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", mime_type, file_size);

    // Write HTTP headers
    write(clientfd, response, strlen(response));

    // Write binary data of image
    write(clientfd, image_data, file_size);

    free(image_data);
    fclose(image_file);
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


    if(listen(socketfd, BACKLOG_QUEUE) < 0) 
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

        if(strstr(uri, ".jpg") != NULL || strstr(uri, ".jpeg") != NULL) {
            char file_path[200];
            sprintf(file_path, ".%s", uri);
            handle_image_request(clientfd, file_path, "image/jpeg");
        }
        else if(strstr(uri, ".gif") != NULL) {
            char file_path[200];
            sprintf(file_path, ".%s", uri);
            handle_image_request(clientfd, file_path, "image/gif");
        }

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



