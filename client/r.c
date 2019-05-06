#include<stdio.h> 
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../WTF.h"

/* get .Manifest from server 
 * if Project doesnt exist on server || server cant be contaacted || can't get servers .Manifest || .update file with data -> break
 *
 *
 *
 * */

/*void currentversion(char * project)
{
  //getManifest from server as .Manifest
  int fd = open(".Manifest", O_RDONLY);
  char * manifest = readFromFile(fd);
  char ** manifest_lines = splitLine(manifest, '\n');
  int i=0;
  while(manifest_lines[i])
  {
    char ** line = splitLine(manifest_lines[i], ' ');
    printf("File Name: %s  Version: %d\n", line[1], atoi(line[3]));
    i++;
  }
}*/



int main()
{
  int fd = open("testFiles/test1.txt", O_RDONLY);
  char * s = readFromFile(fd);
  printf("%s\n", s); 
  printf("%s\n", getFileHash(s));
  return 0;
}
