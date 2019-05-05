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



