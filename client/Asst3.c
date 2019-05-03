#include "../WTF.h"

char* ip;
unsigned short port; 
int sock = 0;

int connectToServer(){ 
	struct sockaddr_in address; 
	int valread; 
	struct sockaddr_in serv_addr; 
	char *hello = "Hello from client"; 
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

void configure(){
	int fd = open("./.configure", O_CREAT | O_TRUNC | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IXUSR);
	write(fd, ip, strlen(ip));
	write(fd, "\n", 2);
	write(fd, port, sizeof(unsigned short));
	close(fd);
	char c[2];
	char s[500];
	strcpy(c, "");
	strcpy(s, "");
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
		i++;
	}
	split[i] = NULL;
	return split;
}

// creates string to be sent to server
char* createSendString(char* file){
	int i;
	int fd = open(file, O_RDONLY, 0);
	char* str = readFromFile(fd);
	char** split = strcmp(file, ".Manifest") == 0 ?  splitString(str, '\n'): NULL;
	char* sendString = (char*) malloc (2000000);
	strcpy(sendString, "");
	close(fd);
	char numAsStr[10];
	strcpy(numAsStr,"");
	if (split != NULL){
		for (i = 0; split[i] != NULL; i++){
			strcat(sendString, ":");
			sprintf(numAsStr,"%d",strlen(split[i]));
			strcat(sendString, numAsStr);
			strcat(sendString, ":");
			strcat(sendString, split[i]);
			fd = open(split[i], O_RDONLY, 0);
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

void parseInputString(char* str){
	printf("%s\n", str);
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
			printf("%s\n\n", split[i]);
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

void commit(char* project){

  char rmcmd[100];
  sprintf(rmcmd, "createdFiles/%s/.Commit", project);
  system(rmcmd);
  
  if(!projectExists(project))
  {
    printf("Project does not exist!\n");
    return;
  }

  char build_path[50];
  sprintf(build_path, "createdFiles/%s/.Update", project);

  struct stat stat_record;
  if(stat(build_path, &stat_record) || stat_record.st_size <= 1)
  {
    // ask server for Manifest --> to be imlemented 
    // comapare .Manifest 

    // check if server manifest is empty --> first commit 
    char server_manifest[50];
    sprintf(server_manifest, ".Manifest"); // supposed to be actual server manifest

    char client_manifest[50];
    sprintf(client_manifest, "createdFiles/%s/.Manifest", project);
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
      sprintf(cmd, "cp createdFiles/%s/.Manifest createdFiles/%s/.Commit", project, project);
      system(cmd);
      //send commit file to server 
      printf("Commited successfully\n");
      return;

    }
    else
    {
      FILE * server_man = fopen(".Manifest", "r");
      char cli_manifest[50];
      sprintf(cli_manifest, "createdFiles/%s/.Manifest", project);
      FILE * client_man = fopen(cli_manifest, "r");

      int clfd = open(cli_manifest, O_RDONLY);
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
          if(matched == NULL) // commit file doesnt have, add to .Commit
          {
            char cmd[50];
            sprintf(cmd, "echo %s >> createdFiles/%s/.Commit", cl_list[i], project);
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
              sprintf(cmd, "echo %s >> createdFiles/%s/.Commit", cl_list[i], project);
              system(cmd);

            } 
          }
          i++;
        } // still have to check that server contains file that client doesn't
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

void push(char* project){}

void create(char* projectName){
	// +7 accounts for "create:"
	// +1 accounts for terminating character
	char* sendText = (char*) malloc(strlen(projectName) + 7 + 1);
	strcpy(sendText, "");
	strcat(sendText, "create:");
	strcat(sendText, projectName);
	send(sock, sendText, strlen(sendText), 0);
}

void destroy(char* project){}

void add(char* project, char* filename){}

void rem(char* project, char* filename){}

void currentVersion(char* project){}

void history(char* project){}

void rollback(char* project, char* version){}

int main(int argc, char* args[]){
	int configured = 1;
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
		configured == 1;
		printf("End of configured\n");
	}

	// Breaks program if server has not been configured
	else if (configured == 0){
		printf("Server has not yet been configured\nTerminating program\n");
		return 0;
	}
	// If server has been configured, performs desired command
	else if (argc == 3){
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
	if (validOption == 0) printf("Invalid Option\n");
	// connectToServer();
	// create("Test");
	// char* s = createSendString("testFiles/test1.txt");
	char* s = createSendString(".Manifest");
	char* s2 = (char*) malloc(strlen("a") + strlen(s));
	strcpy(s2, "a");
	strcat(s2, s);
	printf("%s\n", s2);
	parseInputString(s2);
}



















