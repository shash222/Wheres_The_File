#include<stdio.h> 
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "WTF.h"

/* get .Manifest from server 
 * if Project doesnt exist on server || server cant be contaacted || can't get servers .Manifest || .update file with data -> break
 *
 *
 *
 * */

void commit(char * project_Name)
{
  char rmcmd[100];
  sprintf(rmcmd, "rm client/.Commit");
  system(rmcmd);

  char build_path[50];
  sprintf(build_path, "client/.Update");

  struct stat stat_record;
  if(stat(build_path, &stat_record) || stat_record.st_size <= 1)
  {
    // ask server for Manifest --> to be imlemented 
    // comapare .Manifest 

    // check if server manifest is empty --> first commit 
    char server_manifest[50];
    sprintf(server_manifest, ".Manifest");

    char client_manifest[50];
    sprintf(client_manifest, "client/.Manifest");
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
      sprintf(cmd, "cp client/.Manifest client/.Commit");
      system(cmd);
      //send commit file to server 
      printf("Commited successfully\n");
      return;

    }
    else
    {
      FILE * server_man = fopen(".Manifest", "r");
      FILE * client_man = fopen("client/.Manifest", "r");

      int clfd = open("client/.Manifest", O_RDONLY);
      int sfd = open(".Manifest", O_RDONLY);
      int dif = compareFiles(server_man, client_man);
      if(dif == 0)
      {
        printf("Everything up-to-date. Nothing to commit");
        return;
      }
      else
      {

        char * cl = readFromFile(clfd);
        char * sl =  readFromFile(sfd);

        
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
          if(matched == NULL) // config file doesnt have, add to .Config
          {
            char cmd[50];
            sprintf(cmd, "echo %s >> client/.Commit", cl_list[i]);
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
              char cmd[50];
              sprintf(cmd, "echo %s >> client/.Commit", cl_list[i]);
              system(cmd);

            } 
          }
          i++;
        }
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

int main()
{
  commit("pr1");
  return 0;
}
