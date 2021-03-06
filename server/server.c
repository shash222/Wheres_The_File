// Server side C/C++ program to demonstrate Socket programming 
#include "../WTF.h"


void rollback(char* split[]);

void currentVersion(char* split[]);

void history(char* split[]);


void checkout(char* split[]);

void update(char* split[]);

void upgrade(char* split[]);

void commit(char* split[]);

void push(char* split[], int socketfd);

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

void sendToClient(char* sendText, int socketfd){
	printf("In sendtoserver\n");
	printf("%s\n", sendText);
	send(socketfd, sendText, strlen(sendText), 0);
}


void sendManifest(char* projectName, int socketfd){
	int i;
	if(!projectExists(projectName)) 
	{
		sendToClient("NULL", socketfd);
		return;
	}
	char* file = (char*) malloc(strlen("projects/") + strlen(projectName) + strlen("/.Manifest") + 1);
	strcpy(file, "projects/");
	strcat(file, projectName);
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
	printf("sending: %s\n", str);
	sendToClient(str, socketfd);
	return;
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

void checkout(char* split[]){}

void update(char* split[])
{
	printf("hi\n");
	return; 
}

void upgrade(char* split[]){}

void commit(char* split[])
{
	printf("commit entered");
	char * project = split[1];
	char * commit_data = split[2];
	char project_loc[50];
	sprintf(project_loc, "projects/%s/.Commit", project);
	FILE * commit_file = fopen(project_loc, "w");
	fprintf(commit_file, "%s", commit_data);
	fclose(commit_file);
	return;

}

int getManifestVersion(char* project){
	char* filepath = (char*) malloc(strlen("projects/") + strlen(project) + strlen("/.Manifest") + 1);
	sprintf(filepath, "projects/%s/.Manifest", project);
	int fd = open(filepath, O_RDONLY);
	char* fileData = readFromFile(fd);
	char** splitMan = splitString(fileData, ' ');
	int manVer = atoi(splitMan[3]);
	return manVer;
}

void makeNewProjectVersion(char* projectName, int currVer){
	int newVer = currVer++;
	char* oldVerAsString = (char*) malloc(5);
	sprintf(oldVerAsString, "%d", oldVer);
	char* newVerAsString = (char*) malloc(5);
	sprintf(newVerAsString, "%d", newVer);
	char* oldFilepath = (char*) malloc(strlen("projects/") + strlen(projectName) + strlen(oldVerAsString + 25);
	char* newFilepath = (char*) malloc(strlen("projects/") + strlen(projectName) + strlen(newVerAsString + 25);
	sprintf(oldFilepath, "projects/%s/%s", projectName, oldVerAsString);
	sprintf(newFilepath, "projects/%s/%s", projectName, newVerAsString);
	DIR* oldDirectory = opendir(oldFilepath);
	mkdir(newFilepath);
	struct dirent* dir;

	// Loop is supposed to iterate through the current directory (oldDirectory) and copy all files into the 
	// new directory (newDirectory) with identical file names
	// Need to add condition to see if the version is greater than 1, if this is the first time this 
	// project is being pushed to, then it cannot copy new files and just creates files listed in .Commit
	while ((dir = readdir(oldDirectory)) != NULL){
		strcat(oldFilePath, dir -> d_name);
		
		int oldFd = open(oldFilepath, O_RDONLY);
		int newFd = open(newFilepath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);

	}
}

void push(char* split[], int socketfd){
	//char* filepath = (char*) malloc(strlen("/.Commit") + strlen(splitClientFilePath[0]) + strlen(splitClientFilePath[1] + 1));
	int fileCounter = 0;
	if (strcmp(split[1], "sendingCommit") == 0){
		char* clientCommit = strdup(split[2]);
		char** splitClientCommit = splitString(clientCommit, ' ');
		char** splitClientFilePath = splitString(splitClientCommit[2], '/');
		char filepath[50];
		strcpy(filepath, "");
		sprintf(filepath, "projects/%s/.Commit", splitClientFilePath[1]);
		//char* splitCombined = (char*) malloc(strlen(split[]) + strlen(split[]) + strlen(split[]));
		char* temp = (char*) malloc(strlen(split[2]) + 2);
		sprintf(temp, "%s\n", split[2]);
		int fd = open(filepath, O_RDONLY);
		char* fileData = readFromFile(fd);
		if (strcmp(fileData, temp) == 0){
			sendToClient(".Commit files match, pushing committed files\n", socketfd);
		}
		else{
			sendToClient(".Commit files do not match\nTerminating Server Connection\n", socketfd);
			close(socketfd);
		}
		return;
	}
	char* projectName = strdup(split[2]);
	int manVer = getManifestVersion(projectName);
	
}

void create(char* split[]){
	//create file locally 
	char projectName[50];
	strcpy(projectName,split[1]); 
	printf("%s", projectName);
	char cmd[50];
	if(!projectFileExists()) mkdir("projects", ACCESSPERMS);
	if(projectExists(projectName))
	{
		printf("Project Already Exists!\n");
		return;
	}
	sprintf(cmd, "projects/%s", projectName);
	mkdir(cmd, ACCESSPERMS);


	//add .Manifest
	sprintf(cmd, "projects/%s/.Manifest", projectName);
	FILE* fp = fopen(cmd, "w");
	fclose(fp);



	//set Manifest to V1
	addFileHash(".Manifest", cmd, 1, projectName);
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
		int maxSizeOfInput = 1000024;
		char* inputReceived = (char*) calloc(1, maxSizeOfInput);
		char* buffer = (char*) calloc(1, 10000);
		strcpy(inputReceived, "");
		strcpy(buffer, "");
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
		//while(recv(new_socket, buffer, 50, 0) != 0){
		recv(new_socket, buffer, 10000, 0); 
		strcat(inputReceived, buffer);
		printf("client sent: %s\n", inputReceived);
		//}
		char** split = splitString(inputReceived, ':');
		if (strcmp(split[0], "checkout") == 0) checkout(split);
		else if (strcmp(split[0], "update") == 0) update(split);
		else if (strcmp(split[0], "upgrade") == 0) upgrade(split);
		else if (strcmp(split[0], "commit") == 0) commit(split);
		else if (strcmp(split[0], "push") == 0) push(split, new_socket);
		else if (strcmp(split[0], "create") == 0) create(split);
		else if (strcmp(split[0], "destroy") == 0) destroy(split);
		else if (strcmp(split[0], "add") == 0) add(split);
		else if (strcmp(split[0], "rem") == 0) rem(split);
		else if (strcmp(split[0], "currentVersion") == 0) currentVersion(split);
		else if (strcmp(split[0], "history") == 0) history(split);
		else if (strcmp(split[0], "rollback") == 0) rollback(split);
		else if (strcmp(split[0], "manifest") == 0) sendManifest(split[1], new_socket);
		else printf("Incorrect command\n");
		int i;
		for (i = 0; !split[i]; i++) free(split[i]);
		free(buffer);
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

