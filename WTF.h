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
#include <dirent.h>
#include <errno.h>

char* readFromFile(int fd);

char** splitString(char* str, char delim);
/*
 * adds sha256 hashsum of fl under client/.Manifest fl 
 * 
 * */
void addFileHash(char * filename, char * file_loc, int version, char* project)
{
  char projectPath[50];
  sprintf(projectPath,"projects/%s/.Manifest", project);

  char filePath[50];
  sprintf(filePath,"projects/%s/%s", project, filename);

  char cmd[100];
  //sprintf(cmd, "'read -r sha < <(/ilab/users/us71/Desktop/systems/assignment3/client/projects/pr1/test1.txt)'");
  //system(cmd);

  
  sprintf(cmd, "{ sha256sum %s | sed 's/\\s.*$//'; echo %s; echo %s; echo %d; printf '\n'; } | tr '\n' ' ' >> %s", filePath, filename, file_loc, version, projectPath);
  system(cmd);
  sprintf(cmd, "printf '\n' >> %s",projectPath);
  system(cmd);
}

/*
 * delets fl and current hashsum from client/.Manifest
 *  -- to be used in updates / commits
 *
 * */

void delFileHash(char * filename, char * project)
{
  char projectPath[50];
  sprintf(projectPath,"projects/%s/.Manifest", project);
  
  char cmd[50];
  sprintf(cmd, "sed -i '\\:%s:d' %s", filename, projectPath);
  cmd[strlen(cmd)] = '\0';
  system(cmd);
}

int compareFiles(char * fp1, char *fp2) 
{ 
  char ch1 = fp1[0]; 
  char ch2 = fp2[0]; 
  int error = 0; 
  int i =0;
  while (ch1 != EOF && ch2 != EOF) 
  { 
    if (ch1 != ch2)  error++; 

    i++;
    ch1 = fp1[i]; 
    ch2 = fp2[i]; 
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

int projectExists(char * projectName)
{

  char path[50];
  sprintf(path,"projects/%s", projectName);

  if(access(path, F_OK) == 0) return 1;
  else return 0;

}

int projectFileExists()
{
  char path[50];
  sprintf(path,"projects/");

  if(access(path, F_OK) == 0) return 1;
  else return 0;
}


int fileInProject(char * projectName, char * fileName)
{

  char path[50];
  sprintf(path,"projects/%s/%s", projectName, fileName);

  if(access(path, F_OK) == 0) return 1;
  else return 0;


}
/*char* createSendString(char* file)
{
	int i;
	int fd = open(file, O_RDONLY, 0);
	char* str = readFromFile(fd);
	char** splitFilePath = splitString(strdup(file), '/');
	for(i = 0; splitFilePath[i + 1] != NULL; i++);
	char* fileName = splitFilePath[i];
	char** split = strcmp(fileName, ".Manifest") == 0 ? splitString(str, '\n'): NULL;
	char* sendString = (char*) malloc (2000000);
	strcpy(sendString, "");
	close(fd);
	char numAsStr[10];
	strcpy(numAsStr,"");
	if (split != NULL){
		for (i = 0; split[i] != NULL; i++){
			char** splitData = splitString(split[i], ' ');
			strcat(sendString, ":");
			sprintf(numAsStr,"%d",strlen(splitData[1]));
			strcat(sendString, numAsStr);
			strcat(sendString, ":");
			strcat(sendString, splitData[1]);
			fd = open(splitData[2], O_RDONLY, 0);
			str = readFromFile(fd);
			strcat(sendString, ":");
			sprintf(numAsStr,"%d",strlen(str));
			strcat(sendString, numAsStr);
			strcat(sendString, ":");
			strcat(sendString, str);
			close(fd);
		}
	}
	else{
		// number of files seems unnecessary
		// strcat(sendString, ":1:");
		strcat(sendString, ":");
		sprintf(numAsStr, "%d", strlen(file));
		strcat(sendString, numAsStr);
		strcat(sendString, ":");
		strcat(sendString, file);
		strcat(sendString, ":");
		sprintf(numAsStr, "%d", strlen(str));
		strcat(sendString, numAsStr);
		strcat(sendString, ":");
		strcat(sendString, str);
	}
	return sendString;
}*/



