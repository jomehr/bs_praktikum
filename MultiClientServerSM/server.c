#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "kvs.c"

#define BUF 1024
#define RES 1024

int main (void) {
	//server variables
	int create_socket, new_socket;
	socklen_t addrlen;
	char* buffer = malloc (BUF);
	ssize_t size;
	struct sockaddr_in address;
	const int y = 1;
	//key-value store variables
	char* token[100];
	char* separator = " ";
	char key[BUF];
	char value[BUF];
	char res[BUF];
	//shared memory variables
	struct Data *shmdata;
	int shmid, pid;

	if((create_socket=socket (AF_INET, SOCK_STREAM, 0)) > 0){
		printf ("Socket created!\n");
	}

	setsockopt( create_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons (1500);

	if(bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0){
		printf( "The port is not free – busy!\n");
	}

	if(listen(create_socket, 5)<0){
		printf("Error on listening!\n");
	}
	
	/*0644 to create a new segment with rw-r--r rights
	0777 to create a new segment with unlimited access for all users*/
	shmid = shmget (IPC_PRIVATE, sizeof(struct Data), IPC_CREAT|0777);
	/*if ((shmid = shmget (IPC_PRIVATE, sizeof(struct Data), IPC_CREAT|0777)) == -1){
		printf ("Fehler bei key %d, mit der Größe %d\n",
		IPC_PRIVATE, sizeof(struct Data));
		//DeleteShmid = shmid;
	}*/
	/*attaches a shared memory segment identified by the variable shmid to the address space of the calling process*/
	shmdata = (struct Data *) shmat(shmid,0,0);
	
	shmdata->size=0;
	shmdata->realSize=0;
	
	addrlen = sizeof (struct sockaddr_in);

	/*Kindprozesse erzeugen
	for(i = 0; i < NUM_OF_CHILDS; i++) {
		pid[i] = fork();
		if (pid[i] == -1) {
			printf("Kindprozess konnte nicht erzeugt werden!\n");
			exit(1);
		}
	}*/

	while(1){
		new_socket = accept ( create_socket, (struct sockaddr *) &address, &addrlen );
		pid=fork();
		if(pid<0){
			printf("Fork not successful!\n");
			exit(1);
		}else if(pid>0){
			//parent process
			close(new_socket);
		}else if(pid == 0){
			//child process
			close(create_socket);
			printf ("Client (%s) is connected!\n", inet_ntoa (address.sin_addr));
			char menu[] = "Select put | get | del | list | quit: \n";
			write(new_socket, menu, strlen(menu));

			do{
				bzero(buffer,BUF);
				recv (new_socket, buffer, BUF, 0);
				//removes trailing line from buffer
				if ((strlen(buffer)>0) && (buffer[strlen (buffer) - 1] == '\n')){
					buffer[strlen (buffer) - 1] = '\0';
				}
				printf("%s\n", buffer);
				//adds a trailing whitespace to the buffer
				if ((buffer[strlen (buffer) - 2] != ' ')){
					buffer[strlen (buffer) - 1] = ' ';
				}
				strtoken(buffer, separator, token, 3);
				
				if( strcmp(token[0], "put")==0){
					printf("Executing put function...\n");
					put(token[1], token[2], res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
				}else if (strcmp(token[0], "get")==0){
					printf("Executing get function...\n");
					get(token[1], res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
				}else if( strcmp(token[0], "del")==0){
					printf("Executing del function...\n");
					del(token[1], res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
				}else if( strcmp(token[0], "list")==0){
					printf("Executing list function...\n");
					bzero(res, RES);
					list(res, shmdata);
					write(new_socket, res, RES);
				}else{
					printf("Invalid input!\n");
					write(new_socket, "Invalid input!\n", 29);
				}
				bzero(res, RES);
			}while(strstr(buffer, "quit") == 0);
			printf("Executing quit...\n");

			close (new_socket);
			//close (create_socket);
			//return EXIT_SUCCESS;
		}
	}
	return 0;
}
