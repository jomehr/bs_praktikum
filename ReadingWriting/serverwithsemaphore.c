/*
	If "Error occured, when creating semaphore!":
	"ipcs" gets SemaphoreID
	"ipcrm -s SemaphoreID" to delete it

	Semaphore with
	semaphore_operation (LOCK) and
	semaphore_operation (UNLOCK)

	Reading from File reading.csv
	Writing to File writing.csv

	TODO
	Leser-Schreiber-Problem Alle dürfen lesen, wenn einer schreibt, darf keiner lesen
	mutex();
*/

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

/*Semaphore*/
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#define LOCK       -1
#define UNLOCK      1
#define PERM 0666      /* Zugriffsrechte */
#define KEY  123458L

#define BUF 1024
#define RES 1024

static struct sembuf semaphore;
static int semid;

static int init_semaphore (void) {
   /*Testing, if there is an existing semaphore*/
   semid = semget (KEY, 0, IPC_PRIVATE);
   if (semid < 0) {
      /* Getting all rights */
      umask(0);
      semid = semget (KEY, 1, IPC_CREAT | IPC_EXCL | PERM);
      if (semid < 0) {
         printf ("Error occured when creating semaphore!\n");
         return -1;
      }
      printf("Applied Semaphore-ID: %d\n", semid);
	    printf("Has to be deleted with shell command \"$ ipcrm -s %d\"\n", semid);
      /*initialize semaphore with 1*/
      if (semctl (semid, 0, SETVAL, (int) 1) == -1)
         return -1;
   }
   return 1;
}

static int semaphore_operation (int op) {
   semaphore.sem_op = op;
   semaphore.sem_flg = SEM_UNDO;
   if( semop (semid, &semaphore, 1) == -1) {
      perror(" semop ");
      exit (EXIT_FAILURE);
   }
   return 1;
}

int main (void) {
	const int y = 1;
	int create_socket, new_socket, shmid, pid, stop=0, semaphoreid, i, k;

	socklen_t addrlen;
	ssize_t size;
	semaphoreid = init_semaphore ();

	struct sockaddr_in address;
	struct Data *shmdata;

	char* buffer = malloc(BUF);
	char* token[100];
	char* separator = " ";
	char key[BUF], value[BUF], res[BUF], readingRow[BUF];
	FILE *fp;

	if (semaphoreid < 0) return EXIT_FAILURE;

	if ((create_socket=socket (AF_INET, SOCK_STREAM, 0)) > 0) {
		printf ("\nSocket created!\n");
	}

	setsockopt (create_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons (1500);

	if (bind (create_socket, (struct sockaddr *) &address, sizeof (address)) != 0) {
		printf("The port is not free - busy!\n");
	}

	if (listen(create_socket, 5)<0) {
		printf("Error on listening!\n");
	}

	/*0644 to create a new segment with rw-r--r rights
	0777 to create a new segment with unlimited access for all users*/
	shmid = shmget (IPC_PRIVATE, sizeof(struct Data), IPC_CREAT|0777);

	/*attaches a shared memory segment identified by the variable shmid to the address space of the calling process*/
	shmdata = (struct Data *) shmat(shmid,0,0);
	shmdata->size=0;
	shmdata->realSize=0;

	semaphore_operation ( LOCK );
	/*Reading File Operation*/
	fp = fopen("reading.csv","r");
    if (fp == NULL) {
            printf("No reading file found!\n");
            return 1;
    }else {
      /*Normalerweise ohne else*/
		  /*Each line seperatly*/
		  fscanf(fp,"%s",readingRow);
		  shmdata->realSize= atoi(strtok(readingRow,";"));

		  fscanf(fp,"%s",readingRow);
		  shmdata->size= atoi(strtok(readingRow,";"));

		  fscanf(fp,"%s",readingRow);
		  for (i=0;i<BUF;i++) {
  			shmdata->delFlag[i]= atoi(strtok(readingRow,";"));
  			strcpy(shmdata->key[i], strtok(NULL,";"));
  			strcpy(shmdata->value[i], strtok(NULL,";"));
  			fscanf(fp,"%s",readingRow);
      }
      fclose(fp);
    }

	printf("\nFirst 7 datasets after File Reading Operation\n");
	printf("realSize: %i\tSize: %i\n",shmdata->realSize, shmdata->size);

	for (i=0;i<7;i++) {
		printf("ArrayIndex: %i\tDelFlag: %i\tKey: %s\tValue: %s\n",i , shmdata->delFlag[i], shmdata->key[i], shmdata->value[i]);
	}

	printf("\nList Function Operation\n");
	bzero(res, RES);
	list(res, shmdata);

	semaphore_operation (UNLOCK);

	addrlen = sizeof (struct sockaddr_in);

	while (stop==0){
		new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen);
		pid=fork();
		if(pid<0){
			printf("Fork not successful!\n");
			exit(1);
		}else if(pid>0) {
			//parent process
			close (new_socket);
		}else if(pid == 0) {
			//child process
			printf("ChildId: %i\n", getpid());
			close(create_socket);
			printf ("Client (%s) is connected!\n", inet_ntoa (address.sin_addr));
			char menu[] = "Select: put | get | del | list | disc | stop\n";
			write(new_socket, menu, strlen(menu));

			do {
				semaphore_operation (LOCK);
				bzero(buffer,BUF);
				recv (new_socket, buffer, BUF, 0);
				//removes trailing line from buffer
				if ((strlen(buffer)>0) && (buffer[strlen (buffer) - 1] == '\n')) {
					buffer[strlen (buffer) - 1] = '\0';
				}
				printf("%s\n", buffer);
				//adds a trailing whitespace to the buffer
				if ((buffer[strlen (buffer) - 2] != ' ')) {
					buffer[strlen (buffer) - 1] = ' ';
				}
				strtoken(buffer, separator, token, 3);

				if (strcmp(token[0], "put")==0) {
					semaphore_operation (LOCK);
					printf("Executing put function...\n");
					put(token[1], token[2], res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
					semaphore_operation ( UNLOCK );
				}else if (strcmp(token[0], "get")==0) {
					semaphore_operation (LOCK);
					printf("Executing get function...\n");
					get(token[1], res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
					semaphore_operation (UNLOCK);
				}else if (strcmp(token[0], "del")==0) {
					semaphore_operation ( LOCK );
					printf("Executing del function...\n");
					del(token[1], res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
					semaphore_operation (UNLOCK);
				}else if (strcmp(token[0], "list")==0) {
					printf("Executing list function...\n");
					list(res, shmdata);
					write(new_socket, res, RES);
					semaphore_operation (UNLOCK);
				}else if (strcmp(token[0], "disc")==0) {
					printf("Disconnecting Client!\n");
					char disc[] =  "Disconnected!\n";
					write(new_socket, disc, strlen(disc));
					close(new_socket);
					semaphore_operation (UNLOCK);
				}else if ((strcmp(token[0], "put")!=0) &&
    						 (strcmp(token[0], "get")!=0)  &&
    						 (strcmp(token[0], "del")!=0)  &&
    						 (strcmp(token[0], "list")!=0) &&
    						 (strcmp(token[0], "disc")!=0) &&
    						 (strcmp(token[0], "stop")!=0)) {
					printf("Invalid input!\n");
					char invinp[] =  "Invalid input!\n";
					write(new_socket, invinp, strlen(invinp));
				}

				bzero(res, RES);

			}while (strstr(buffer, "stop") == 0);
			printf("Executing Stop - Stopping Server Socket\n");
			stop=1;
			close (create_socket);
			/*Kills Child*/
			kill(pid, SIGKILL);
		}
	}
	printf("Exiting Main-While-Loop with stop!");

	/*Writing File Operation*/
	semaphore_operation (LOCK);
	fp = fopen("writing.csv","w");

	if (fp == NULL) {
		printf("No file found! - Creating File named: writing.csv\n");
	}
	/*First Row realSize*/
	fprintf(fp, "%i;\n", shmdata->realSize);
	/*Second Row size*/
	fprintf(fp, "%i;\n", shmdata->size);
	/*Row 3 to realSize*/
	for(i=0;i<shmdata->realSize;i++){
		fprintf(fp, "%i;%s;%s;\n", shmdata->delFlag[i], shmdata->key[i], shmdata->value[i]);
	}

	fclose(fp);
	semaphore_operation (UNLOCK);

	/*Detach Semaphore*/
	printf("Has to be deleted with shell command $ ipcrm -s %d\n", semid);
	semctl (semid, 0, IPC_RMID, 0);

	//abort();
	return EXIT_SUCCESS;
}
