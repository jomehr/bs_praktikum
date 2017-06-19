/*	
	If "Error occured, when creating semaphore!":
	"ipcs" gets SemaphoreID
	"ipcrm -s SemaphoreID" to delete it
*/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "kvs.h"
#include "readwritestore.c"

//Semaphore
#define LOCK -1
#define UNLOCK 1

/*KeyValStore*/
#define BUF 1024
#define RES 1024

struct sembuf semaphore;
int shmrcid, semid, mutex, rc;

int create_semaphore(){
	int id = semget(IPC_PRIVATE,1,IPC_CREAT | 0777);
	if(semid<0){
		printf("Erzeugung der Semaphore fehlgeschlagen");
	}
	unsigned short marker[1];
	marker[0] = 1;
	if(semctl(id, 0, SETALL, marker)  < 0){
		printf("Semaphoren initialisierung (CTL) Fehlgeschlagen");
		return -1;
	}
	printf("Semaphore ID: %d\n",id);
	return id;
}

int semaphore_op(int op, int sem){
	semaphore.sem_op = op;
	semaphore.sem_flg = SEM_UNDO;
	if(semop(sem, &semaphore, 1) == -1){
		perror("Fehler bei der Semaphoren operation");
		return -1;
	}
	return 1;
}

int main (void) {
	const int y = 1;
	int create_socket, new_socket, shmid, pid, whilestop=0, semaphoreid, i, k;
	socklen_t addrlen;
	ssize_t size;
	struct sockaddr_in address;
	struct Data *shmdata;
	char* buffer = malloc(BUF);
	char* token[100];
	char* separator = " ";
	char key[BUF], value[BUF], res[BUF];
	
	if ((create_socket=socket (AF_INET, SOCK_STREAM, 0)) > 0) {
		printf("\nSocket created!\n");
	}

	setsockopt (create_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons (1500);

	if (bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0) {
		printf("\nThe port is not free - busy!\n");
	}

	if (listen(create_socket, 5)<0) {
		printf("\nError on listening!\n");
	}

	/*0644 to create a new segment with rw-r--r rights
	0777 to create a new segment with unlimited access for all users*/
	shmid = shmget(IPC_PRIVATE, sizeof(struct Data), IPC_CREAT|0777);
	
	shmrcid=shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0777);
	semid=create_semaphore();
	mutex=create_semaphore();
	
	/*attaches a shared memory segment identified by the variable shmid to the address space of the calling process*/
	shmdata = (struct Data *) shmat(shmid,0,0);
	shmdata->size = 0;
	shmdata->realSize = 0;
	
	rc = (int*) shmat(shmrcid,0,0);
	rc = 0;
	
	readData(shmdata);
	
	//Output file contents, if file exists
	printf("realSize: %i\tSize: %i\n",shmdata->realSize, shmdata->size);
	for (i=0; i<shmdata->size; i++) {
		printf("ArrayIndex: %i\tDelFlag: %i\tKey: %s\tValue: %s\n",i , shmdata->delFlag[i], shmdata->key[i], shmdata->value[i]);
	}

	addrlen = sizeof(struct sockaddr_in);

	while (whilestop == 0) {
		new_socket = accept (create_socket, (struct sockaddr *) &address, &addrlen);
		pid=fork();
		if (pid < 0) {
			printf("Fork not successful!\n");
			exit(1);
		}else if (pid > 0) {
			//parent process
			close(new_socket);
		}else if (pid == 0 ){
			//child process
			printf("\nChildId: %i\n", getpid());
			close(create_socket);
			printf("Client (%s) is connected!\n", inet_ntoa (address.sin_addr));
			do {
				bzero(buffer, BUF);
				char menu[] = "Select: put | get | del | list | disc | stop\n";
				write(new_socket, menu, strlen(menu));
				recv (new_socket, buffer, BUF, 0);
				//removes trailing line from buffer
				if ((strlen(buffer)>0) && (buffer[strlen (buffer) - 1] == '\n')) {
					buffer[strlen (buffer) - 1] = ' '; //\0 for telnet
				}
				//adds a trailing whitespace to the buffer
				//uncomment for telnet
				/*if ((buffer[strlen (buffer) - 2] != ' ')) {
					buffer[strlen (buffer) - 1] = ' ';
				}*/
				strtoken(buffer, separator, token, 3);
				
				if(	!(strcmp(token[0], "put")) == 0  &&
					!(strcmp(token[0], "get")) == 0  &&
					!(strcmp(token[0], "del")) == 0  &&
					!(strcmp(token[0], "list")) == 0  &&
					!(strcmp(token[0], "disc")) == 0  &&
					!(strcmp(token[0], "stop")) == 0) {
					printf("Invalid input!\n");
					char invinp[] =  "Invalid input!\n";
					write(new_socket, invinp, strlen(invinp));
				}else if (strcmp(token[0], "put") == 0) {
					semaphore_op(LOCK,semid);
					printf("Executing put function...\n");
					put(token[1], token[2], res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
					saveData(shmdata);
					sleep(5);
					semaphore_op(UNLOCK,semid);
				}else if (strcmp(token[0], "get") == 0) {
					semaphore_op(LOCK,mutex);
					rc++;
					if(rc == 1){
						semaphore_op(LOCK,semid);
					} 
					semaphore_op(UNLOCK,mutex);
					printf("Executing get function...\n");
					get(token[1], res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
					semaphore_op(LOCK,mutex);
                    rc--;
                    if(rc == 0){
						semaphore_op(UNLOCK,semid);
					} 
                    semaphore_op(UNLOCK,mutex);
				}else if (strcmp(token[0], "del") == 0) {
					semaphore_op(LOCK,semid);
					printf("Executing del function...\n");
					del(token[1], res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
					saveData(shmdata);
					semaphore_op(UNLOCK,semid);
				}else if (strcmp(token[0], "list") == 0) {
					semaphore_op(LOCK,mutex);
					rc++;
					if(rc == 1){
						semaphore_op(LOCK,semid);
					} 
					semaphore_op(UNLOCK,mutex);
					printf("Executing list function...\n");
					bzero(res, RES);
					list(res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
					semaphore_op(LOCK,mutex);
                    rc--;
                    if(rc == 0){
						semaphore_op(UNLOCK,semid);
					} 
                    semaphore_op(UNLOCK,mutex);
				//Just adds one more doWhile - Invalid input run
				}
				/*else if (strcmp(token[0], "disc") == 0) {
					printf("Disconnecting Client!\n");
					char menu1[] = "Disconnected!\n";
					write(new_socket, menu1, strlen(menu1));
					//Second argument: 0 disables receive, 1 disables send, 2 disables both
					shutdown(new_socket, 2);
				}*/					
				bzero(res, RES);
			}while(strcmp(buffer, "stop") != 0);
			printf("\nExecuting Stop - Stopping Server Socket\n");
			whilestop=1;
			close(new_socket);
		}
	}
		
	/*Writing File Operation*/
	saveData(shmdata);
	
	/*ServerSocket closed*/
	close(create_socket);
	/*
	//Detach Shared Memory
	shmdt(shmdata);
	shmctl(shmid, IPC_RMID, 0);
	//Detach Semaphore
	//Optional, if "semctl (semid, 0, IPC_RMID, 0);" doesnt work
	printf("\nOptional, if \"semctl (semid, 0, IPC_RMID, 0)\" doesnt detach semaphore\nDelete semaphore with shell command \"$ ipcrm -s %d\"\n", semid);
	//semctl (semid, 0, IPC_RMID, 0);
	*/
	/*Kills Child*/
	kill(pid, SIGTERM);
	
	return EXIT_SUCCESS;
}
