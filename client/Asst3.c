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
  printf("%d\n", sock);
  printf("Connected to server\n");
  return 0; 
} 

void sendToServer(char* sendText){
  char buff[100];
  printf("%d\n", sock);
  send(sock, sendText, strlen(sendText), 0);
  read(sock, buff, 80);
  printf("server sent: %s\n", buff);
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

// creates string to be sent to server
char* createSendString(char* file){
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
      char* fp = (char*) malloc(strlen("createdFiles/") + strlen(splitFilePath[j]));
      strcpy(fp, "");
      strcat(fp, "createdFiles/");
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

void update(char* project){}

void upgrade(char* project){}

void commit(char* project){}

void push(char* project){}

void create(char* projectName){
  // +7 accounts for "create:"
  // +1 accounts for terminating character
  char* sendText = (char*) malloc(strlen(projectName) + 7 + 1);
  strcpy(sendText, "");
  strcat(sendText, "create:");
  strcat(sendText, projectName);
  send(sock, sendText, strlen(sendText), 0);
  //create file locally 
  char cmd[50];
  if(!projectFileExists()) system("mkdir projects");
  if(projectExists(projectName))
  {
    printf("Project Already Exists!\n");
    return;
  }
  sprintf(cmd, "mkdir projects/%s", projectName);
  system(cmd);


  //add .Manifest
  sprintf(cmd, "touch projects/%s/.Manifest", projectName);
  system(cmd);

  char file_loc [20];
  sprintf(file_loc, "projects/%s/.Manifest", projectName);

  //set Manifest to V1
  addFileHash(".Manifest", file_loc, 1, projectName);
  return;

}

void destroy(char* project){}

void add(char* project, char* filename){}

void rem(char* project, char* filename){}

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
  sendToServer("This is a test message");
  // create("Test");
  // char* s = createSendString("testFiles/test1.txt");
  char* s = createSendString("testFiles/test1.txt");
  char* s2 = (char*) malloc(strlen("a") + strlen(s));
  strcpy(s2, "a");
  strcat(s2, s);
  sendManifest("projects/pr1");
  //parseInputString(s2);
}
