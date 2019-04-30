// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>

unsigned short port;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_lock(&mutex);
// pthread_mutex_unlock(&mutex);
 
int new_socket;
int highestClientSocket;

void runServer(){ 
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char *hello = "Hello from server";
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
		if (listen(server_fd, 3) < 0){ 
			perror("listen"); 
			exit(EXIT_FAILURE); 
		} 
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		}
		if (new_socket > highestClientSocket) highestClientSocket = new_socket;
		printf("Client has connected");
	
	}
}

// Will take all commands from client and perform appropriate operations
void handleRequests(char* readData){
	int sizeOfReadValue = 4096;
	char* readValue = (char*) malloc(sizeOfReadValue);
	char* splitArr;
	int bytes = 0;
	char* buffer = (char*) malloc(1025);
	for(;;){
		while((bytes += read()) != 0){
			if (
		}
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
	handleRequests();
	return 0;
}















