#include "../WTF.h"

char* ip;
unsigned short port; 
int sock = 0;

int connectToServer(){ 
  struct sockaddr_in address; 
  int valread; 
  struct sockaddr_in serv_addr; 
  char buffer[1024] = {0}; 
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
    printf("\n Socket creation error \n"); 
    return -1; 
  } 

  memset(&serv_addr, '0', sizeof(serv_addr)); 

  serv_addr.sin_family = AF_INET; 
  serv_addr.sin_port = htons(port); 
  // Convert IPv4 and IPv6 addresses from text to binary form 

  if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0){ 
    printf("\nInvalid address/ Address not supported \n"); 
    return -1; 
  } 

  while (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
    time_t tim;
    printf("Unable to connect to socket, trying again in 3 seconds\n");
    sleep(3); 
  }
  printf("Connected to server\n");
  return 0; 
} 

char *sendToServer(char* sendText){
  char buff[1000];
  send(sock, sendText, strlen(sendText), 0);
  recv(sock, buff, 1000, 0);
  // read(sock, buff, 1000);
  close(sock);
  connectToServer();
  return buff;
}

void configure(){
  char portStr[6];
  strcpy(portStr, "");
  int fd = open(".configure", O_CREAT | O_TRUNC | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IXUSR);
  if (fd < 0){
    printf("Unable to configure\nTerminating program");
    return;
  }
  char buff[2];
  strcpy(buff, "");
  buff[0] = '\n';
  buff[1] = '\0';
  char strToWrite[50];
  sprintf(portStr, "%d", port);
  strcpy(strToWrite, "");
  strcat(strToWrite, ip);
  strcat(strToWrite, buff);
  strcat(strToWrite, portStr);
  write(fd, strToWrite, strlen(strToWrite));
  close(fd);
  printf("Configured successfully!\n");
}

int readConfigure(){
  int fd = open(".configure", O_RDONLY);
  if (fd < 0) return 0;
  char* configData = readFromFile(fd);
  char** splitConfigData = splitString(configData, '\n');
  ip = splitConfigData[0];
  port = atoi(splitConfigData[1]);
}


void sendManifest(char* projectName){
  int i;
  char* file = (char*) malloc(strlen(projectName) + strlen("/.Manifest") + 1);
  strcpy(file, projectName);
  strcat(file, "/.Manifest");
  int fd = open(file, O_RDONLY, 0);
  char* str = readFromFile(fd);
  char* sendString = (char*) malloc (2000000);
  strcpy(sendString, "");
  close(fd);
  char numAsStr[10];
  strcpy(numAsStr,"");
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

void sendFilesInCommit(char* projectName){
  int i;
  if (!projectName) printf("Project name null\n");
  if(!projectExists(projectName)) 
  {
    printf("Project does not exist\nTerminating program\n");
    return;
  }
  char* file = (char*) malloc(strlen("projects/") + strlen(projectName) + strlen("/.Commit") + 1);
  strcpy(file, "projects/");
  strcat(file, projectName);
  strcat(file, "/.Commit");
  int fd = open(file, O_RDONLY, 0);
  char* str = readFromFile(fd);
  char* sendString = (char*) malloc (2000000);
  strcpy(sendString, "");
  close(fd);
  char numAsStr[10];
  strcpy(numAsStr,"");
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
  char* temp = (char*) malloc(strlen("push") + strlen(sendString) + 1);
  sprintf(temp, "push%s", sendString);
  sendString = temp;
  printf("sending: %s\n", sendString);
  printf("%s\n", sendToServer(sendString));
  return;
}

void parseInputString(char* str){
  char** split = splitString(str, ':');
  int i = 0;
  int fd;
  int fileSize;
  // every file has 4 index spots, 2 for strlen of filepath and content, and 2 for filepath and content
  int fileDataCounter = 0;
  // split[0] is most likely command (tbd for parsing)
  // split [1] is number of files
  while(split[i] != NULL){
    if (fileDataCounter == 4) {
      fileDataCounter = 0;
      close(fd);
    }
    if (i == 0){
      i++;
      continue; // Temporary, need to figure out what to do with command
    }
    if (i == 1){
      i++;
      continue; // Temporary, need to figure out what to do with number of files
    }
    printf("%s\n", split[i]);
    if (fileDataCounter == 0){
      char** splitFilePath = splitString(split[i], '/');
      int j = 0;
      while (splitFilePath[j + 1] != NULL) j++;
      char* fp = (char*) malloc(strlen("projects/") + strlen(splitFilePath[j]));
      strcpy(fp, "");
      strcat(fp, "projects/");
      strcat(fp, splitFilePath[j]);
      fd = open(fp, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);
      if (fd < 0) perror("Unable to open/create fd error: \n");
    }
    if (fileDataCounter == 1) fileSize = atoi(split[i]);
    if (fileDataCounter == 2){
      write(fd, split[i], fileSize);
    }
    fileDataCounter++;
    i++;
  }
}

void checkout(char* project){}

void update(char* project_Name)
{


  char path[50];
  sprintf(path, "projects/%s", project_Name);

  struct stat stat_record;
  // ask server for Manifest --> to be imlemented 
  // comapare .Manifest 

  // check if server manifest is empty --> first commit 
  char manifestGrab[50];
  sprintf(manifestGrab, "manifest:%s", project_Name);
  char  * server_manifest = sendToServer(manifestGrab);
  if(strcmp(server_manifest, "NULL") ==0)
  {
    printf("Project doesn't Exist. Use .Update failed\n");
    return;
  }
  char ** smanifest_split_line = splitString(server_manifest, '\n');
  char ** smanifest_line = getMatchingLine(".Manifest", smanifest_split_line);



  char client_manifest[50];
  sprintf(client_manifest, "%s/.Manifest", path);

  int cmfd = open(client_manifest, O_RDONLY, 0);
  char * local_manifest_data = readFromFile(cmfd);
  close(cmfd);

  char ** local_split = splitString(local_manifest_data, '\n');
  char ** cmanifest_line = getMatchingLine(".Manifest", local_split);


  bool conflict = 0;

  char update_loc[50];
  sprintf(update_loc, "projects/%s/.Update", project_Name);

  char con_loc[50];
  sprintf(con_loc, "projects/%s/.Conflict", project_Name);

  remove(update_loc);

  FILE * updateFile = fopen(update_loc, "w");
  fclose(updateFile);
  FILE * conFile = fopen(con_loc, "w");
  fclose(conFile);

  if(stat(client_manifest, &stat_record) || stat_record.st_size <= 1) 
  {

    int i=0;
    FILE * updateFile = fopen(update_loc, "a");
    while(smanifest_split_line[i])
    {
      char print_Line[100];
      sprintf(print_Line,"%s %c:\n", smanifest_split_line[i], 'A');
      fprintf(updateFile, "%s\n", print_Line);
      printf("%s\n", print_Line);
      i++;

    }
  }

  if(strcmp(server_manifest, "") ==0)
  {
    //server manifest is empty 
    //cp client/.Manifest to .Commit 

    FILE * updatefile = fopen(update_loc, "w");
    fclose(updateFile);
    return;

  }
  else //compare files
  {
    bool matches =  atoi(smanifest_line[3]) == atoi(cmanifest_line[3]);
    char clm[50];
    sprintf(clm, "%s/.Manifest", path);

    int clfd = open(client_manifest, O_RDONLY);
    int sfd = open(".Manifest", O_RDONLY);

    char * cl = readFromFile(clfd);
    //char * sl =  readFromFile(sfd);
    char sl[strlen(server_manifest)+1];
    strcpy(sl, server_manifest);

    int dif = compareFiles(cl, sl);
    if(dif == 0)
    {
      FILE * updatefile = fopen(update_loc, "w");
      fclose(updateFile);
      printf("Everything up-to-date.\n");
      return;
    }
    else
    {

      char ** cl_list = splitString(cl, '\n');
      char ** sl_list = splitString(sl, '\n');

      int i =0;
      while(cl_list[i])
      {


        char cln[100];
        strcpy(cln, cl_list[i]);
        if(!sl_list[i])
        {
          if(matches)
          {
            char ** cline = splitString(cln, ' ');
            printf("%s %s %s %c\n", cline[0], cline[1], cline[2], 'U');
            i++;
            continue;
          }
          else
          {
            char ** cline = splitString(cln, ' ');
            FILE * updateFile = fopen(update_loc, "a");
            fprintf(updateFile,"%s %s %s %s %c\n", cline[0], cline[1], cline[2], cline[3], 'D');
            fclose(updateFile);
            i++;
            continue;
          }

        }

        char sln[100];
        strcpy(sln, sl_list[i]);

        char ** cline = splitString(cln, ' ');
        char ** sline = splitString(sln, ' ');
        if(strcmp(cline[1], ".Manifest") == 0) 
        {
          i++;
          continue;
        }



        char  **matched = getMatchingLine(cline[1] , sl_list);
        if(matched == NULL) // in the clients, not in servers
        {
          if(matches)
          {
            char ** cline = splitString(cln, ' ');
            printf("%s %s %s %s %c\n", cline[0], cline[1], cline[2], cline[3], 'U');
            i++;
            continue;
          }
          else
          {
            char ** cline = splitString(cln, ' ');
            FILE * updateFile = fopen(update_loc, "a");
            fprintf(updateFile,"%s %s %s %s %c\n", cline[0], cline[1], cline[2], cline[3], 'D');
            fclose(updateFile);
            i++;
            continue;
          }

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
            if(matches)
            {
              int fd = open(cline[2], O_RDONLY);
              if(strcmp(getFileHash(readFromFile(fd)), sline[0]) != 0)
              {
                char ** cline = splitString(cln, ' ');
                printf("%s %s %s %s %c\n", cline[0], cline[1], cline[2], cline[3], 'U');
                i++;
                continue;

              }
            }
            else // live hashes differet but in both manifest 
            {
              if(atoi(cline[3]) == atoi(matched[3])) 
              {
                i++;
                continue;
              }
              int fd = open(cline[2], O_RDONLY);
              if(strcmp(getFileHash(readFromFile(fd)), cline[0]) != 0)
              {
                char cmd[50];
                sprintf(cmd, "%s/.Update",  path);
                FILE * f = fopen(cmd, "a");
                fprintf(f,"%s %s %s %s %c\n", cline[0], cline[1], cline[2], cline[3], 'M');
                printf("%s %s %s %s %c\n", cline[0], cline[1], cline[2], cline[3], 'M');
                fclose(f); 
                i++;

                continue;

              }
            }
          } 
        }
      } 
      i = 0;
      while(sl_list[i])
      {


        char sln[100];
        strcpy(sln, sl_list[i]);
        char ** sline = splitString(sln, ' ');
        if(!sline) break;
        
        if(!cl_list[i])
        {
                char cmd[50];
                sprintf(cmd, "%s/.Update",  path);
                FILE * f = fopen(cmd, "a");
                fprintf(f,"%s %s %s %s %c\n", sline[0], sline[1], sline[2], sline[3], 'A');
                printf("%s %s %s %s %c\n", sline[0], sline[1], sline[2], sline[3], 'A');
                fclose(f); 

                i++;
                continue;
        }
        char cln[100];
        strcpy(cln, cl_list[i]);

        sline = splitString(sln, ' ');
        if(!sline) break;

        if(strcmp(sline[1], ".Manifest") == 0) 
        {
          i++;
          continue;
        }
        cmanifest_line = getMatchingLine(sline[1], cl_list);
        if(!cmanifest_line)
        {
          if(!matches)
          {
                char cmd[50];
                sprintf(cmd, "%s/.Update",  path);
                FILE * f = fopen(cmd, "a");
                fprintf(f,"%s %s %s %s %c\n", sline[0], sline[1], sline[2], sline[3], 'A');
                printf("%s %s %s %s %c\n", sline[0], sline[1], sline[2], sline[3], 'A');
                fclose(f); 

          }

        }
        i++;
      }

      int upfd = open(update_loc, O_RDONLY);
      printf("\n\n\n");

      char * fileData = readFromFile(upfd);
      printf("Updates: %s\n", fileData);


      return;

    }


  }
}

void upgrade(char* project){}


void commit(char * project_Name)
{


  char path[50];
  sprintf(path, "projects/%s", project_Name);

  char commit_path[50];
  sprintf(commit_path, "%s/.Commit", path);
  remove(commit_path);


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
    char manifestGrab[50];
    sprintf(manifestGrab, "manifest:%s", project_Name);
    char  * server_manifest = sendToServer(manifestGrab);
    if(strcmp(server_manifest, "NULL") ==0)
    {
      printf("Project doesn't Exist. Use ./WTF Commit and Add First\n");
      return;
    }
    char ** smanifest_split_line = splitString(server_manifest, '\n');
    char ** smanifest_line = getMatchingLine(".Manifest", smanifest_split_line);

    char client_manifest[50];
    sprintf(client_manifest, "%s/.Manifest", path);

    int cmfd = open(client_manifest, O_RDONLY, 0);
    char * local_manifest_data = readFromFile(cmfd);
    close(cmfd);

    char ** local_split = splitString(local_manifest_data, '\n');
    char ** cmanifest_line = getMatchingLine(".Manifest", local_split);

    if(atoi(smanifest_line[3]) != atoi(cmanifest_line[3]))
    {
      printf("Repository not Up to date.\n");
      return;
    }

    if(stat(client_manifest, &stat_record) || stat_record.st_size <= 1) 
    {
      printf("Nothing to commit\n");
      return;
    }

    if(strcmp(server_manifest, "") ==0)
    {
      //server manifest is empty 
      //cp client/.Manifest to .Commit 

      FILE * fp1 = fopen(client_manifest, "r");

      char commit_File[50];
      sprintf(commit_File,"%s/.Commit", path);

      FILE * fp2 = fopen(commit_File, "w");
      fileCopy(fp1, fp2);

      char sed[50];
      sprintf(sed, "sed 's/$/A/' %s/.Commit", path);


      int fd = open(commit_path, O_RDONLY, 0);
      char * commit_data = readFromFile(fd);

      char dataToSend[strlen(commit_data) + 10];
      sprintf(dataToSend, "commit:%s:%s", project_Name, commit_data);

      sendToServer(dataToSend);
      printf("Commited Successfully\n");

      return;

    }
    else
    {
      char clm[50];
      sprintf(clm, "%s/.Manifest", path);

      int clfd = open(client_manifest, O_RDONLY);
      int sfd = open(".Manifest", O_RDONLY);

      char * cl = readFromFile(clfd);
      //char * sl =  readFromFile(sfd);
      char sl[strlen(server_manifest)+1];
      strcpy(sl, server_manifest);

      int dif = compareFiles(cl, sl);
      if(dif == 0)
      {
        printf("Everything up-to-date. Nothing to commit\n");
        return;
      }
      else
      {

        char ** cl_list = splitString(cl, '\n');
        char ** sl_list = splitString(sl, '\n');

        int i =0;
        while(cl_list[i])
        {


          char cln[100];
          strcpy(cln, cl_list[i]);
          if(!sl_list[i])
          {

            char ** cline = splitString(cln, ' ');
            char cmd[50];
            sprintf(cmd, "%s/.Commit",  path);
            FILE * f = fopen(cmd, "a");
            fprintf(f,"%s %s %s %s %c\n", cline[0], cline[1], cline[2], cline[3], 'A');
            fclose(f); 
            i++;
            continue;
          }

          char sln[100];
          strcpy(sln, sl_list[i]);

          char ** cline = splitString(cln, ' ');
          char ** sline = splitString(sln, ' ');
          if(strcmp(cline[1], ".Manifest") == 0) 
          {
            i++;
            continue;
          }



          char  **matched = getMatchingLine(cline[1] , sl_list);
          if(matched == NULL) // commit file doesnt have, add to .Commit
          {
            char cmd[50];
            sprintf(cmd, "%s/.Commit",  path);
            FILE * f = fopen(cmd, "a");
            fprintf(f,"%s %s %s %s %c\n", cline[0], cline[1], cline[2], cline[3], 'A');
            fclose(f); 
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
              if(atoi(matched[3]) > atoi(cline[3]))
              {
                printf("Repository not up-to-date. Update before commiting\n");
                return;
              }


              char cmd[50];
              sprintf(cmd, "%s/.Commit",  path);
              FILE * f = fopen(cmd, "a");
              fprintf(f,"%s %s %s %s %c\n", cline[0], cline[1], cline[2], cline[3], 'M');
              fclose(f); 
              i++;
              continue;


            } 
          }
        } 
        i = 0;
        while(sl_list[i])
        {


          char sln[100];
          strcpy(sln, sl_list[i]);

          char ** sline = splitString(sln, ' ');
          if(!sline) break;

          if(strcmp(sline[1], ".Manifest") == 0) 
          {
            i++;
            continue;
          }
          if(!fileInProject(sline[1], project_Name))
          {
            char cmd[50];
            sprintf(cmd, "%s/.Commit",  path);
            FILE * f = fopen(cmd, "a");
            fprintf(f,"%s %s %s %s %c\n", sline[0], sline[1], sline[2], sline[3], 'D');
            fclose(f); 

          }
          i++;
        }

        char commit_File[50];
        sprintf(commit_File,"%s/.Commit", path);


        int fd = open(commit_File, O_RDONLY, 0);
        char * commit_data = readFromFile(fd);

        char dataToSend[strlen(commit_data) + 10];
        sprintf(dataToSend, "commit:%s:%s", project_Name, commit_data);

        sendToServer(dataToSend);
        printf("Commited successfully\n");

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
        char ** manifestLines = splitString(manifest_data, '\n');
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

void push(char* project){
  char* filepath = (char*) malloc(strlen("projects/") + strlen(project) + strlen("/.Commit") + 1);
  sprintf(filepath, "projects/%s/.Commit", project);
  int fd = open(filepath, O_RDONLY);
  if (fd < 0){
    printf("Cannot find file\nTerminating program\n");
    exit(0);
  }
  char* fileContent = readFromFile(fd);
  char** splitFile = splitString(fileContent, '\n');
  int i = 0;
  char* str = createSendString(filepath);
  char* sendString = (char*) malloc(strlen("push:sendingCommit") + strlen(fileContent) + 1);
  sprintf(sendString, "push:sendingCommit:%s", fileContent);
  char* receivedText = sendToServer(sendString);
  while(splitFile[i] != NULL){
    char** splitLine = splitString(splitFile[i], ' ');
    char* sendString = splitLine[4];
    i++;
  }
  free(sendString);
  char* filesInCreate = createSendString(filepath);
  sendString = (char*) malloc(strlen("push") + strlen(filesInCreate) + 1);
  strcpy(sendString, "");
  sprintf(sendString, "push%s", filesInCreate);
  sendToServer(sendString);
}

void create(char* projectName){
  // +7 accounts for "create:"
  // +1 accounts for terminating character
  char* sendText = (char*) malloc(strlen(projectName) + 7 + 1);
  strcpy(sendText, "");
  strcat(sendText, "create:");
  strcat(sendText, projectName);
  printf("%s:", sendText);
  sendToServer(sendText);
  //create file locally 
  char cmd[50];
  if(!projectFileExists()) mkdir("projects",ACCESSPERMS);
  if(projectExists(projectName))
  {
    printf("Project Already Exists!\n");
    return;
  }
  //open(cmd, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);
  sprintf(cmd, "projects/%s", projectName);
  mkdir(cmd, ACCESSPERMS);


  //add .Manifest
  sprintf(cmd, "projects/%s/.Manifest", projectName);
  FILE *file = fopen(cmd, "w");
  fclose(file);


  //set Manifest to V1
  addFileHash(".Manifest", cmd, 1, projectName);
  return;

}

void destroy(char* project){}

//void add(char* project, char* filename){}

void rem(char * projectname, char * filename)
{

  if(!projectExists(projectname))
  {
    printf("Project does not exist!\n");
    return;
  }
  else delFileHash(filename, projectname); 

}


void currentVersion(char* project){}

void history(char* project){}

void rollback(char* project, char* version){}

int main(int argc, char* args[]){
  int validOption = 1;
  //  Terminates program if no additional arguments are entered
  if (argc < 2){
    printf("Not enough arguments entered\nTerminating program\n");
    return 0;
  }
  // Configures IP address and port number using designated inputs if configure command is called
  if (strcmp(args[1], "configure") == 0){
    if (argc != 4){
      printf("Invalid arguments entered for configuration\nTerminating program\n");
      return 0;
    }
    ip = args[2];
    port = atoi(args[3]);
    configure();
    return 0;
  }
  if (readConfigure() == 0){
    printf("Unable to retrieve configuration data\nTerminating program\n");
    return 0;
  }
  // Breaks program if server has not been configured
  /*	else if (configured == 0){
      printf("Server has not yet been configured\nTerminating program\n");
      return 0;
      }
      */	// If server has been configured, performs desired command
  connectToServer();
  if (argc == 3){
    if (strcmp(args[1], "checkout") == 0) checkout(args[2]);
    else if (strcmp(args[1], "update") == 0) update(args[2]);
    else if (strcmp(args[1], "upgrade") == 0) upgrade(args[2]);
    else if (strcmp(args[1], "commit") == 0) commit(args[2]);
    else if (strcmp(args[1], "push") == 0) push(args[2]);
    else if (strcmp(args[1], "create") == 0) create(args[2]);
    else if (strcmp(args[1], "destroy") == 0) destroy(args[2]);
    else if (strcmp(args[1], "currentversion") == 0) currentVersion(args[2]);
    else if (strcmp(args[1], "history") == 0) history(args[2]);
    else validOption = 0;
  }
  else if (argc == 4){
    if (strcmp(args[1], "add") == 0) add(args[2], args[3]);
    else if (strcmp(args[1], "remove") == 0) rem(args[2], args[3]);
    else if (strcmp(args[1], "rollback") == 0) rollback(args[2], args[3]);
    else validOption = 0;
  }
  else validOption = 0;
  if (validOption == 0) printf("Invalid Option\n");
  close(sock);
  return 0;
}
