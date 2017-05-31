﻿#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#define BUF 1024
#define KV_STRING 1024
#define RES 1024

/*sharedmemory-spezifisch*/
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#define NUM_OF_CHILDS 5
#define PROCESSMEM 1024

int strtoken(char* str, char* separator, char** token, int size);
int put(char* key, char* value, char* res);
int get(char* key, char* res);
int del(char* key, char* res);
void list(char* res);
char buf[BUFSIZ];

struct Data{
	char key[BUF][KV_STRING];
	char value[BUF][KV_STRING];
	int delFlag[BUF];				//1 = deleted
	int size;						//last array index
	int realSize;					//no of entries in array
};
// smhget(IPC_PRIVATE, sizeof(Data), IPC_CREAT)
//Only KVStore as a variable has the functions hereunder provided,
//not the stuct itself!
struct Data *KVStore;

int main (void) {


	int create_socket, new_socket;
	socklen_t addrlen;
	char* buffer = malloc (BUF);
	ssize_t size;
	struct sockaddr_in address;
	const int y = 1;
	//variables for the key-value store
	//char* str[100];
	char* token[100];
	char* separator = " ";
	char key[BUF];
	char value[BUF];
	char res[BUF];
	KVStore->size=0;
	KVStore->realSize=0;

	/*sharedmemory-spezifisch*/
    int i, shmid,semid, *shar_mem;
    int pid[NUM_OF_CHILDS];
	static int DeleteShmid = 0;
	void *shmdata;

	/*SharedMemory erstellen*/
    shmid = shmget (IPC_PRIVATE, sizeof(struct KVStore), IPC_CREAT | 0666);
	if (shmid == -1)
		printf ("Fehler bei key %d, mit der Größe %d\n",
        IPC_PRIVATE, PROCESSMEM);
		DeleteShmid = shmid;

	/* Shared-Memory-Segment anbinden */
	shmdata = (struct KVStore *)shmat (shmid, 0, 0);
	if (shmdata == (void *) -1)
		printf ("Fehler bei shmat(): shmid %d\n", shmid);

	/* Kennung am Anfang des Segments schreiben */
	*(int *) shmdata = semid;
	buffer = shmdata + sizeof (int);
	printf ("Server läuft mit Shared-Memory-ID %d\n", shmid);

	/* Das SharedMemory Segment wird abgekoppelt und freigegeben. */
    shmdt(shar_mem);
    shmctl(shmid, IPC_RMID, 0);


	if((create_socket=socket (AF_INET, SOCK_STREAM, 0)) > 0){
		printf ("Socket created!\n");
	}

	setsockopt( create_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons (15000);

	if(bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0){
		printf( "The port is not free – busy!\n");
	}

	listen (create_socket, 5);
	addrlen = sizeof (struct sockaddr_in);

	/*Kindprozesse erzeugen*/
	for(i = 0; i < NUM_OF_CHILDS; i++) {
		pid[i] = fork();
		if (pid[i] == -1) {
			printf("Kindprozess konnte nicht erzeugt werden!\n");
			exit(1);
		}
	}

	while(1){
		new_socket = accept ( create_socket, (struct sockaddr *) &address, &addrlen );

		for(i = 0; i < NUM_OF_CHILDS; i++){
			if(pid[i] == 0){
				if (new_socket > 0){
					//pid[i] = fork();
					printf ("Client (%s) is connected!\n", inet_ntoa (address.sin_addr));
				}

		printf ("Ein Client (%s) ist verbunden!\n", inet_ntoa (address.sin_addr));

		write(new_socket, "Geben Sie die Funktion put / get / del mit den benötigten Parametern ein: \n", 80);

		do{
			bzero(buffer,BUF);
			recv (new_socket, buffer, BUF, 0);
			if ((strlen(buffer)>0) && (buffer[strlen (buffer) - 1] == '\n')){
				buffer[strlen (buffer) - 1] = '\0';
			}
			printf("%s\n", buffer);
			if ((buffer[strlen (buffer) - 2] != ' ')){
				buffer[strlen (buffer) - 1] = ' ';
			}
			strtoken(buffer, separator, token, 3);
			if( strcmp(token[0], "put")==0){
				printf("Jetzt wird die put Funktion ausgeführt\n");
				put(token[1], token[2], res);
				write(new_socket, res, RES);
				}else{
					if (strcmp(token[0], "get")==0){
					printf("Jetzt wird die get Funktion ausgeführt\n");
					get(token[1], res);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
					}else{
						if( strcmp(token[0], "del")==0){
						printf("Jetzt wird die del Funktion ausgeführt\n");
						del(token[1], res);
						write(new_socket, res, RES);
						write(new_socket, "\n", 2);
						} else {
							if( strcmp(token[0], "list")==0){
							bzero(res, RES);
							list(res);
							write(new_socket, "Size: ", 6);
							write(new_socket, res, RES);
								}else{
								printf("Die Eingabe war fehlerhaft\n");
								write(new_socket, "Fehlerhafte Eingabe\n", 20);
								}
							}
						}
					}
					bzero(res, RES);
		}while(strstr(buffer, "quit") == 0);
	}
}

		/* Der Vaterprozess wartet, bis alle Kindprozessefertig sind.  */
		for(i = 0; i < NUM_OF_CHILDS; i++){
			waitpid(pid[i], NULL, 0);
		}

		/* Das SharedMemory Segment wird abgekoppelt und freigegeben. */
		shmdt(shar_mem);
		shmctl(shmid, IPC_RMID, 0);

		close (new_socket);
		close (create_socket);
		return EXIT_SUCCESS;
	}

}



int strtoken(char* str, char* separator, char** token, int size) {
	int i=0;
	token[0] = strtok(str, separator);
	while(token[i++] && i < size)
	token[i] = strtok(NULL, separator);
	return (i);
}

int put(char* key, char* value, char* res){
	int i;
	for(i=0;i<KVStore.size;i++){
		if(strcmp(KVStore.key[i],key)==0){
			strcpy(KVStore.value[i],value);
			strcpy(res,"Put successful! Existing entry replaced.\n");
			return 1;
		}
	}
	for(i=0;i<KVStore.size;i++){
		if(KVStore.delFlag[i]==1){
			strcpy(KVStore.key[i],key);
			strcpy(KVStore.value[i],value);
			KVStore.delFlag[i]=0;
			KVStore.realSize++;
			strcpy(res,"Put successful! Deleted entry replaced.\n");
			return 2;
		}
	}
	strcpy(KVStore.key[KVStore.size],key);
	strcpy(KVStore.value[KVStore.size],value);
	KVStore.size++;
	KVStore.realSize++;
	strcpy(res,"Put successful!\n");
	return 0;
}

int get(char* key, char* res){
	strcpy(res, "");
	strcat(res, buf);
	strcat(res,"\n");
	if(KVStore.size == 0){
		strcpy(res,"Store is empty.\n");
		return 1;
	}
	int i;
	for (i=0; i<KVStore.size;i++) {
		if(strcmp(KVStore.key[i],key)==0){
			strcpy(res,KVStore.value[i]);
			return 0;
		}
	}
	strcpy(res,"Key not found!\n");
	return 1;
}

int del(char* key, char* res){
	strcpy(res, "");
	if(KVStore.size == 0){
		strcpy(res,"Store is empty.\n");
		return 1;
	}
	int i;
	for(i=0;i<KVStore.size;i++){
		if(strcmp(KVStore.key[i],key)==0){
			strcpy(res,KVStore.value[i]);
			KVStore.delFlag[i]=1;
			strcpy(KVStore.key[i],"");
			strcpy(KVStore.value[i],"");
			KVStore.realSize--;
			return 0;
		}
	}
	strcpy(res,"Key not found!\n");
	return 1;
}

void list(char* res){
	strcpy(res, "");
	char buf[BUFSIZ];
	//int converts to char array

	snprintf(buf, sizeof(buf), "%d", KVStore.size);
	strcat(res, buf);
  strcat(res,"\n");
	if(KVStore.size==0){
		printf("Store is empty.\n");
		return;
	}
	int i;
	for(i=0;i<KVStore.size;i++){
		if(KVStore.delFlag[i]!=1){
			printf("Index:%i-Key:%s;Value:%s;\n", i, KVStore.key[i], KVStore.value[i]);
			strcat(res,KVStore.key[i]);
			strcat(res," | ");
			strcat(res,KVStore.value[i]);
			strcat(res,"\n");
		}
	}
	return;
}