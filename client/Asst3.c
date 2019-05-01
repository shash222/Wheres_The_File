
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
}

void destroy(char* project){}

void add(char* project, char* filename){}

void rem(char* project, char* filename){}

void currentVersion(char* project){}

void history(char* project){}

void rollback(char* project, char* version){}

int main(int argc, char* args[]){
	int configured = 0;
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
	connectToServer();
	create("Test");
}



















