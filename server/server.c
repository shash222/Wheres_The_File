// Server side C/C++ program to demonstrate Socket programming 
#include "../WTF.h"

unsigned short port;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_lock(&mutex);
// pthread_mutex_unlock(&mutex);
 
int new_socket;
int highestClientSocket;

void checkout(char* split[]){}

void update(char* split[]){}

void upgrade(char* split[]){}

void commit(char* split[]){}

void push(char* split[]){}

void create(char* split[]){
	
}

void destroy(char* split[]){}

void add(char* split[]){}

void rem(char* split[]){}

void currentVersion(char* split[]){}

void history(char* split[]){}

void rollback(char* split[]){}

void splitInput(char* input){
	int colons = 0;
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
	free(word);
	word = NULL;
	for (i = 0; i < colons + 1; i++) free(split[i]);
}

void runServer(){ 
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
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
		while((bytes = read(new_socket, buffer, 1000)) != 0){
			readBytes += bytes;
			if (readBytes >= .6 * maxSizeOfInput){
				maxSizeOfInput *= 2;
				inputReceived = realloc(inputReceived, maxSizeOfInput);
			}
			strcat(inputReceived, buffer);
		}
		splitInput(inputReceived);
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















