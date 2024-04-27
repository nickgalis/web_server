#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MAX_BUFF 1024
/*
 int reg; 
    int dir; 
    int symlink; 
    int fifo; 
    int socket; 
    int block; 
    int character; 
*/


void travDir(char *dirPath, int *reg, int *direc, int *symlink, int *fifo, int *socket, int *block, int *character)
{
    struct dirent *entry; 
    struct stat fileInfo;
    char fullPath [MAX_BUFF];
    DIR *dir = opendir(dirPath); 

    if(dir == NULL)
        perror("Fail to open the directory"); 

    while((entry = readdir(dir)) != NULL)
    {
        if(strcmp(entry->d_name, ".") ==0 || strcmp(entry->d_name, "..") ==0)
            continue;
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name); 
        printf("%s\n", fullPath);
        if(lstat(fullPath, &fileInfo) == 0)
        {
            if(S_ISREG(fileInfo.st_mode))
                (*reg)++;
            else if(S_ISDIR(fileInfo.st_mode))
            {
                (*direc)++; 
                travDir(fullPath, reg, direc, symlink, fifo, socket, block, character);
            }
            else if(S_ISLNK(fileInfo.st_mode))
                (*symlink)++; 
            else if(S_ISFIFO(fileInfo.st_mode))
                (*fifo)++; 
            else if(S_ISSOCK(fileInfo.st_mode))
                (*socket)++; 
            else if(S_ISBLK(fileInfo.st_mode))
                (*block)++; 
            else if(S_ISCHR(fileInfo.st_mode)) 
                (*character)++; 

        }
        
    }
   

}
//Use lstat so we don't follow the symlink 
int main(int argc, char *argv[])
{
    int reg = 0, dir = 0, symlink = 0, fifo = 0, socket = 0, block = 0, character = 0; 
    
    travDir(argv[1], &reg, &dir, &symlink, &fifo, &socket, &block, &character);

    printf("reg: %d, dir: %d, symlink: %d, fifo: %d, socket = %d, block = %d, character = %d", reg, dir, symlink, fifo, socket, block, character);



}