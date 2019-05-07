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
#include <limits.h>
#include <openssl/sha.h>
#include <stdbool.h>


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
  //sprintf(cmd, "'read -x sha < <(/ilab/users/us71/Desktop/systems/assignment3/client/projects/pr1/test1.txt)'");
  //system(cmd);


  sprintf(cmd, "{ sha256sum %s | sed 's/\\s.*$//'; echo %s; echo %s; echo %d; printf '\n'; } | tr '\n' ' ' >> %s", filePath, filename, file_loc, version, projectPath);
  system(cmd);
  sprintf(cmd, "printf '\n' >> %s",projectPath);
  system(cmd);
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
  while (ch1 && ch2) 
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

char** splitString(char* str, char delim){
  int i;
  int delimsFound = 0;
  for (i = 0; i < strlen(str); i++){
    if (str[i] == delim) delimsFound++;
  }
  // allocating 2 more than number of delims found to add additional NULL value at the end
  // NULL value will allow for finding end of array
  char** split = (char**) malloc((delimsFound + 2) * sizeof(char*));
  char* word = (char*) malloc(20000);
  char delimStr[2];
  delimStr[0] = delim;
  delimStr[1] = '\0';
  word = strtok(str, delimStr);
  i = 0;
  while (word != NULL){ 
    split[i] = (word);
    word = strtok(NULL, delimStr);
    if (!word) break;
    i++;
  }
  split[++i] = NULL;
  return split;
}

// creates string to be sent to client
char* createSendString(char* file){
  printf("file %s\n", file);
  int i;
  int fd = open(file, O_RDONLY, 0);
  char* str = readFromFile(fd);
  char** splitFilePath = splitString(strdup(file), '/');
  for(i = 0; splitFilePath[i + 1] != NULL; i++);
  char* projectName = splitFilePath[i - 1];
  char* fileName = splitFilePath[i];
  char** split = strcmp(fileName, ".Manifest") == 0 ? splitString(str, '\n'): NULL;
  split = strcmp(fileName, ".Commit") == 0 ? splitString(str, '\n') : split;
  char* sendString = (char*) malloc (2000000);
  strcpy(sendString, "");
  close(fd);
  char numAsStr[10];
  strcpy(numAsStr,"");
  if (split != NULL){
    for (i = 0; split[i] != NULL; i++){
      char** splitData = splitString(split[i], ' ');
      strcat(sendString, ":");
      sprintf(numAsStr,"%d",strlen(projectName));
      strcat(sendString, numAsStr);
      strcat(sendString, ":");
      strcat(sendString, projectName);
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
      return sline;
    }

  }
  return NULL;

}

void fileCopy(FILE* fp1, FILE * fp2)
{
  char c; 

  if (fp1 == NULL) 
  { 
    return; 
  } 


  if (fp2 == NULL) 
  { 
    return ; 
  } 

  c = fgetc(fp1); 
  while (c != EOF) 
  { 
    fputc(c, fp2); 
    c = fgetc(fp1); 
  } 


  fclose(fp1); 
  fclose(fp2); 
  return;
}
char* getFileHash(char * contents) {
  int x;
  unsigned char hash [SHA256_DIGEST_LENGTH];// Hash array
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, contents, strlen(contents));
  SHA256_Final(hash, &sha256);
  char* s = malloc(SHA256_DIGEST_LENGTH*2 + 1);
  for (x = 0; x < SHA256_DIGEST_LENGTH; x++) {
    sprintf(s+(x*2), "%02x", hash[x]);
  }
  int i = SHA256_DIGEST_LENGTH * 2;
  s[i] = '\0';
  return s;
}

int removeDir(const char *path)
{
  DIR *d = opendir(path);
  size_t path_len = strlen(path);
  int x = -1;

  if (d)
  {
    struct dirent *p;

    x = 0;

    while (!x && (p=readdir(d)))
    {
      int r2 = -1;
      char *buf;
      size_t len;

      if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) continue;


      len = path_len + strlen(p->d_name) + 2; 
      buf = malloc(len);

      if (buf)
      {
        struct stat statbuf;

        snprintf(buf, len, "%s/%s", path, p->d_name);

        if (!stat(buf, &statbuf))
        {
          if (S_ISDIR(statbuf.st_mode)) r2 = removeDir(buf);
          else  r2 = unlink(buf);
        }

        free(buf);
      }

      x = r2;
    }

    closedir(d);
  }

  if (!x)
  {
    // Directorty should be empty by now
    x = rmdir(path);
  }

  return x;
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



