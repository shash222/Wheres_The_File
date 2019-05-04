// Server side C/C++ program to demonstrate Socket programming 
#include "../WTF.h"


void rollback(char* split[]);

void currentVersion(char* split[]);

void history(char* split[]);


void checkout(char* split[]);

void update(char* split[]);

void upgrade(char* split[]);

void commit(char* split[]);

void push(char* split[]);

void create(char* split[]);

void destroy(char* split[]);

void add(char* split[]);

void rem(char* split[]);

unsigned short port;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_lock(&mutex);
// pthread_mutex_unlock(&mutex);

int new_socket;
int highestClientSocket;

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

// creates string to be sent to client
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

void createFiles(char** split){
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


void parseInputString(char* str){
  /*	int colons = 0;
      int i;
      for (i = 0; i < strlen(input); i++){
      if (input[i] == ':') colons++;
      }
      char* split[colons + 1];
      for (i = 0; i < colons + 1; i++) split[i] = (char*) malloc(sizeof(char*));
      char* word = (char*) malloc(200);
      word = strtok(input, ":");
      i = 0;
      while(word != NULL){
      split[i] = strdup(word);
      word = strtok(NULL, ":");
      i++;
      }
      */
  char** split = splitString(str, ':');
  if (strcmp(split[0], "checkout") == 0) checkout(split);
  else if (strcmp(split[0], "update") == 0) update(split);
  else if (strcmp(split[0], "upgrade") == 0) upgrade(split);
  else if (strcmp(split[0], "commit") == 0) commit(split);
  else if (strcmp(split[0], "push") == 0) push(split);
  else if (strcmp(split[0], "create") == 0) create(split);
  else if (strcmp(split[0], "destroy") == 0) destroy(split);
  else if (strcmp(split[0], "add") == 0) add(split);
  else if (strcmp(split[0], "rem") == 0) rem(split);
  else if (strcmp(split[0], "currentVersion") == 0) currentVersion(split);
  else if (strcmp(split[0], "history") == 0) history(split);
  else if (strcmp(split[0], "rollback") == 0) rollback(split);
  else printf("Incorrect command\n");
  //free(word);
  //word = NULL;
  int i;
  for (i = 0; !split[i]; i++) free(split[i]);
}


void checkout(char* split[]){}

void update(char* split[]){}

void upgrade(char* split[]){}

void commit(char* split[]){}

void push(char* split[]){}

void create(char* split[]){

  char projectName[50];
  strcpy(projectName, split[1]); 
  //create file locally 
  char cmd[50];
  if(projectFileExists() == 0) system("mkdir projects");
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


void destroy(char* split[]){}

void add(char* split[]){}

void rem(char* split[]){}

void currentVersion(char* split[]){}

void history(char* split[]){}

void rollback(char* split[]){}

void runServer(){ 
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1001000] = {0};
  // Creating socket file descriptor 
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  // Forcefully attaching socket to the port 8080 
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){ 
    perror("setsockopt"); 
    exit(EXIT_FAILURE); 
  } 
  address.sin_family = AF_INET; 
  address.sin_addr.s_addr = INADDR_ANY; 
  address.sin_port = htons( port ); 

  // Forcefully attaching socket to the port 8080 
  if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0){ 
    perror("bind failed"); 
    exit(EXIT_FAILURE); 
  } 

  for(;;){
    int maxSizeOfInput = 1024;
    char* inputReceived = (char*) malloc(maxSizeOfInput);
    strcpy(inputReceived, "");
    int bytes = 0;
    int readBytes;
    if (listen(server_fd, 3) < 0){ 
      perror("listen"); 
      exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){ 
      perror("accept"); 
      exit(EXIT_FAILURE); 
    }
    if (new_socket > highestClientSocket) highestClientSocket = new_socket;
    printf("Client has connected\n");
    read(new_socket, buffer, 1000000);
    readBytes += bytes;
    if (readBytes >= .6 * maxSizeOfInput){
      maxSizeOfInput *= 2;
      inputReceived = realloc(inputReceived, maxSizeOfInput);
    }
    strcat(inputReceived, buffer);
    printf("client sent: %s\n", inputReceived);
    send(new_socket, inputReceived, strlen(inputReceived), 0);
    parseInputString(inputReceived);
    free(inputReceived);
  }
}

void exitProgram(){
  printf("Ending program\n");
}

int main(int argc, char* args[]){
  atexit(exitProgram);
  if (argc != 2){
    printf("Starting server with incorrect arguments\nTerminating server");
    return 0;
  }
  port = atoi(args[1]);
  runServer();
  return 0;
}















