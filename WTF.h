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
 * adds sha256 hashsum of file under client/.Manifest file 
 * 
 * */
void addFileHash(char * filename)
{
  char cmd[50];
  sprintf(cmd, "sha256sum %s >> client/.Manifest", filename);
  system(cmd);
}

/*
 * delets file and current hashsum from client/.Manifest
 *  -- to be used in updates / commits
 *
 * */

void delFileHash(char * filename)
{
  char cmd[50];
  sprintf(cmd, "sed -i '\\:%s:d' client/.Manifest", filename);
  cmd[strlen(cmd)] = '\0';
  system(cmd);
}

