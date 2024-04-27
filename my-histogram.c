#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MAX_BUFF 1024
/*
 fprinf(outputFile, format ) 
*/
void makeGNUPlot()
{
    FILE *gp = popen("gnuplot", "w"); 
    if(gp == NULL)
        perror("Error opening pipe to gnuplot"); 

    //Setting output 
    fprintf(gp, "set terminal jpeg\n"); 
    fprintf(gp, "set output 'histogram.jpeg'\n"); 

    //Setting 
    fprintf(gp, "set style data histogram\n"); 
    fprintf(gp, "set title 'Histogram'\n");
    fprintf(gp, "set style fill solid\n");
    fprintf(gp, "set ylabel 'Frequency'\n"); 
    fprintf(gp, "set xlabel 'File Types'\n"); 

    //yRange can be manually set here: Or remove if max of dataset is not known (gnuplot) will dynamically adjust y axis
    fprintf(gp, "set yrange [0:20]\n");
    fprintf(gp, "plot 'output.txt' using 2:xtic(1) title 'Frequency'"); 

    

}

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
    
    /*argv[1] will be taking the directory path (This must be the full path including ./)
        - The code that follows does not add ./ so please include it as argv[1] of my-histogram.c
        - output will be a 'histogram.jpeg' 
        - Check 'output.txt' for name value pairs for the file type and count
        - You must make clean before each run as output.txt is on append mode and thus it will append to old data if not delted 
        - Returns counts have been tested and verfied for everything but (Socket, Block, Character)
    */
    travDir(argv[1], &reg, &dir, &symlink, &fifo, &socket, &block, &character);
    makeGNUPlot();

    FILE *fd = fopen("output.txt", "a");
    if(fd == NULL)
        perror("Error creating output.txt > gnuplot");

     
    //Redirecting output to output.txt
    fprintf(fd, "Regular %d\n", reg); 
    fprintf(fd, "Directory %d\n", dir); 
    fprintf(fd, "Symlink %d\n", symlink); 
    fprintf(fd, "FIFO %d\n", fifo); 
    fprintf(fd, "Socket %d\n", socket); 
    fprintf(fd, "Block %d\n", block); 
    fprintf(fd, "Character %d\n", character); 

    fclose(fd); 


}