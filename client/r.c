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

void commit(char * project_Name)
{


  char path[50];
  sprintf(path, "projects/%s", project_Name);

  char rmcmd[100];
  sprintf(rmcmd, "rm -f %s/.Commit", path);
  system(rmcmd);


  if(!projectExists(project_Name))
  {
    printf("Project does not exist!\n");
    return;
  }

  char build_path[50];
  sprintf(build_path, "%s/.Update", path);

  struct stat stat_record;
  if(stat(build_path, &stat_record) || stat_record.st_size <= 1)
  {
    // ask server for Manifest --> to be imlemented 
    // comapare .Manifest 

    // check if server manifest is empty --> first commit 
    char server_manifest[50];
    sprintf(server_manifest, ".Manifest");

    char client_manifest[50];
    sprintf(client_manifest, "%s/.Manifest", path);
    if(stat(client_manifest, &stat_record) || stat_record.st_size <= 1) 
    {
      printf("Nothing to commit\n");
      return;
    }

    if(stat(server_manifest, &stat_record) || stat_record.st_size <= 1)
    {
      //server manifest is empty 
      //cp client/.Manifest to .Commit 
      char cmd[50];
      sprintf(cmd, "cp %s/.Manifest %s/.Commit", path, path);
      system(cmd);
      //send commit file to server 

      char commit_File[50];
      sprintf(commit_File,"%s/.Commit", path);

      sprint(sendString, "commit:%s", createSendString(commit_File));
      //sendToServer(sendString);


      printf("Commited successfully\n");
      return;

    }
    else
    {
      FILE * server_man = fopen(".Manifest", "r");
      char clm[50];
      sprintf(clm, "%s/.Manifest", path);
      FILE * client_man = fopen(clm, "r");

      int clfd = open(clm, O_RDONLY);
      int sfd = open(".Manifest", O_RDONLY);

      char * cl = readFromFile(clfd);
      char * sl =  readFromFile(sfd);

      int dif = compareFiles(cl, sl);
      if(dif == 0)
      {
        printf("Everything up-to-date. Nothing to commit\n");
        return;
      }
      else
      {



        char ** cl_list = splitLine(cl, '\n');
        char ** sl_list = splitLine(sl, '\n');

        int i =0;
        while(cl_list[i])
        {
          char cln[1000];
          strcpy(cln, cl_list[i]);

          char sln[1000];
          strcpy(sln, cl_list[i]);

          char ** cline = splitLine(cln, ' ');
          char ** sline = splitLine(sln, ' ');
          char  **matched = getMatchingLine(cline[1] , sl_list);
          if(matched == NULL) // commit file doesnt have, add to .Commit
          {
            char cmd[50];
            sprintf(cmd, "echo %s >> %s/.Commit", cl_list[i], path);
            char sendString[100];
            char commit_File[50];
            sprintf(commit_File,"%s/.Commit", path);
            sprint(sendString, "commit:%s", createSendString(commit_File));
            //sendToServer(sendString);
            printf("Commited successfully\n");
            system(cmd);
          }
          else //check hashes
          {
            if(strcmp(cline[0],matched[0])==0) //hashes match, continue
            {
              i++;
              continue;
            }
            else
            {
              if(atoi(matched[3]) >= atoi(cline[3]))
              {
                printf("Repository not up-to-date. Update before commiting\n");
                return;
              }

              char cmd[50];
              sprintf(cmd, "echo %s >> %s/.Commit", cl_list[i], path);


              char sendString[100];


              char commit_File[50];
              sprintf(commit_File,"%s/.Commit", path);

              sprint(sendString, "commit:%s", createSendString(commit_File));
              //sendToServer(sendString);

              printf("Commited successfully\n");
              system(cmd);

            } 
          }
          i++;
        } // still have to check that server contains file that client doesn't
        // send .Commit to server if ok 
        return;

      }


    }
  }
  else 
  {
    printf("Repository not up to date, Update before commiting!\n");
    return;
  }
}

void add(char * projectName, char * fileName)
{
  char path[50];
  sprintf(path, "projects/%s", projectName); 
  char filepath[50];
  sprintf(filepath, "%s/%s", path, fileName);
  if(!projectExists(projectName)) 
  {
    printf("Project does not exist!\n");
    return;
  }
  else
  {
    if(!fileInProject(projectName, fileName))
    {
      printf("File does not exist!\n");
      return;
    } 
    else
    {

      char client_manifest[50];
      sprintf(client_manifest, "%s/.Manifest", path);
      struct stat stat_record;
      if(stat(client_manifest, &stat_record) || stat_record.st_size <= 1) 
      {
        addFileHash(fileName, filepath, 1, projectName); 
      }
      else
      {
        //check if file exists, if it does delete and add with version + 1
        //else add with v1
        int fd = open(client_manifest, O_RDONLY);
        char * manifest_data = readFromFile(fd);
        char ** manifestLines = splitLine(manifest_data, '\n');
        char ** match = getMatchingLine(fileName, manifestLines);

        if(match)
        {
          int version = atoi(match[3]);          
          delFileHash(fileName, projectName);
          addFileHash(fileName, filepath, version+1, projectName); 
        }
        else
        {
          addFileHash(fileName, filepath, 1, projectName); 
        }
      }

    }
  }

}

void removeHash(char * projectname, char * filename)
{

  if(!projectExists(projectname))
  {
    printf("Project does not exist!\n");
    return;
  }
  else delFileHash(filename, projectname); 

}

void currentversion(char * project)
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
}



int main()
{
  currentversion("pr1");
  return 0;
}
