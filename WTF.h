#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>


/*
 * adds sha256 hashsum of fl under client/.Manifest fl 
 * 
 * */
void addFileHash(char * filename, char * file_loc)
{
  char cmd[50];
  sprintf(cmd, "sha256sum %s >> %s", filename, file_loc);
  system(cmd);
}

/*
 * delets fl and current hashsum from client/.Manifest
 *  -- to be used in updates / commits
 *
 * */

void delFileHash(char * filename, char * file_loc)
{
  char cmd[50];
  sprintf(cmd, "sed -i '\\:%s:d' %s", filename, file_loc);
  cmd[strlen(cmd)] = '\0';
  system(cmd);
}
int compareFiles(FILE *fp1, FILE *fp2) 
{ 
  char ch1 = getc(fp1); 
  char ch2 = getc(fp2); 
  int error = 0; 
  while (ch1 != EOF && ch2 != EOF) 
  { 
    if (ch1 != ch2)  error++; 

    ch1 = getc(fp1); 
    ch2 = getc(fp2); 
  } 

  return error;
} 

char** splitLine(char *fl, char  delim) {

  int spaces = 0;
  int i = 0;
  while (i < strlen(fl)) 
  {
    if(fl[i]  == delim) 
    {
      spaces ++;
    }
    i ++;
  }

  char **line_arr = (char**) malloc(sizeof(char*) * (spaces+1));
  char *pdelm = &delim;

  char *token = strtok(fl, pdelm);
  line_arr[0] = token;

  i = 1; 
  while(token != NULL) {
    token = strtok(NULL, pdelm);
    line_arr[i] = token;
    i++;
  }

  return line_arr;
} 


char* readFromFile(int fd){
  int maxCapacity = 4096;
  char* buff = (char*) malloc(1024);
  strcpy(buff, "");
  char* str = (char*) malloc(maxCapacity);
  strcpy(str, "");
  while(read(fd, buff, 1020) != 0){
    if (strlen(str) > .75 * maxCapacity){
      maxCapacity *= 2;
      str = realloc(str, maxCapacity);
    }
    str = strcat(str, strdup(buff));
  }
  return str;
}

char ** getMatchingLine(char *c, char ** s)
{
  
  int i=0;
  for(;s[i]; i++)
  {
    char ns[1000];
    strcpy(ns, s[i]);
    char ** sline = splitLine(ns, ' ');

    if(strcmp(c, sline[1])==0) {
    strcpy(ns, s[i]);
      return splitLine(ns, ' ');
    }

  }
  return NULL;


}
