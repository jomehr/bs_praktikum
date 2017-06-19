/*	
	If "Error occured, when creating semaphore!":
	"ipcs" gets SemaphoreID
	"ipcrm -s SemaphoreID" to delete it
	
	Semaphore with
	semaphore_operation (LOCK) and
	semaphore_operation (UNLOCK)
	
	Reading from File reading.csv
	Writing to File writing.csv
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
#include "kvs.c"

/*Semaphore*/
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
      printf("\nApplied Semaphore-ID: %d\n", semid);
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
	int create_socket, new_socket, shmid, pid, whilestop=0, semaphoreid, db, mutex, rc, i, k;
	socklen_t addrlen;
	ssize_t size;
	struct sockaddr_in address;
	struct Data *shmdata;
	char* buffer = malloc(BUF);
	char* token[100];
	char* separator = " ";
	char key[BUF], value[BUF], res[BUF], readingRow[BUF];
	FILE *fp;
  
//semaphoreid = init_semaphore ();
	if (semaphoreid < 0) return EXIT_FAILURE;

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

	/*attaches a shared memory segment identified by the variable shmid to the address space of the calling process*/
	shmdata = (struct Data *) shmat(shmid,0,0);
	shmdata->size = 0;
	shmdata->realSize = 0;
	/*
	db = semget(IPC_PRIVATE, 1, IPC_CREAT | 0777);
    if(db == -1){
      printf("Semaphore group couldn't be created\n");
      return -1;
    }

    mutex = semget(IPC_PRIVATE, 1, IPC_CREAT | 0777);
    if(mutex == -1){
      printf("Semaphore group couldn't be created\n");
      return -1;
    }
	semctl(db, 0, SETALL, (int) 1);
    semctl(mutex, 0, SETALL, (int) 1);
	*/
	/*Reading File Operation*/
	fp = fopen("writing.csv","r");
    if (fp == NULL) {
            printf("\nNo reading file found!\n");
            return 1;
    }else {
		/*Each line seperatly, first realSize, then size, then the dataset*/
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
		printf("\nDataset has been read from ./reading.csv\n");
		fclose(fp);
	}
	
	printf("\nFirst 7 datasets after File Reading Operation\n");
	printf("realSize: %i\tSize: %i\n",shmdata->realSize, shmdata->size);
	for (i=0; i<7; i++) {
		printf("ArrayIndex: %i\tDelFlag: %i\tKey: %s\tValue: %s\n",i , shmdata->delFlag[i], shmdata->key[i], shmdata->value[i]);
	}
	
	printf("\nList Function Operation\n");
	bzero(res, RES);
	list(res, shmdata);

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
			
			/*
			Menu cant be in doWhile bc it will get looped with Invalid Input
			but we -need- something that displays the menu after an input again
			*/
			printf("Client (%s) is connected!\n", inet_ntoa (address.sin_addr));
			char menu[] = "Select: put | get | del | list | disc | stop\n";
			write(new_socket, menu, strlen(menu));
			
			do {
				bzero(buffer, BUF);
				recv (new_socket, buffer, BUF, 0);
				printf("RawContent:%s.\n", buffer);
				//removes trailing line from buffer
				if ((strlen(buffer)>0) && (buffer[strlen (buffer) - 1] == '\n')) {
					buffer[strlen (buffer) - 1] = ' ';//\0 for telnet
				}
				printf("BufferContent:%s.\n", buffer);
				//adds a trailing whitespace to the buffer
				/*if ((buffer[strlen (buffer) - 2] != ' ')) {
					buffer[strlen (buffer) - 1] = ' ';
				}*///uncomment for telnet
				printf("BufferContent:%s.\n", buffer);
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
					//semop(db,&down,1);
					printf("Executing put function...\n");
					put(token[1], token[2], res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
					//sleep(5000);
					//semop(db,&up,1);
				}else if (strcmp(token[0], "get") == 0) {
					//semop(mutex,&down,1);
					//rc = rc + 1;
					//if(rc == 1) semop(db, &down, 1);
					//semop(mutex, &up, 1);
					printf("Executing get function...\n");
					get(token[1], res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
					//semop(mutex, &down, 1);
                    //rc = rc - 1;
                    //if(rc == 0) semop(db, &up, 1);
                    //semop(mutex, &up, 1);
				}else if (strcmp(token[0], "del") == 0) {
					//semop(db, &down, 1);
					printf("Executing del function...\n");
					del(token[1], res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
					//semop(db, &up, 1);
				}else if (strcmp(token[0], "list") == 0) {
					//semop(mutex,&down,1);
					//rc = rc + 1;
					//if(rc == 1) semop(db, &down, 1);
					//semop(mutex, &up, 1);
					printf("Executing list function...\n");
					bzero(res, RES);
					list(res, shmdata);
					write(new_socket, res, RES);
					write(new_socket, "\n", 2);
					//semop(mutex, &down, 1);
                    //rc = rc - 1;
                    //if(rc == 0) semop(db, &up, 1);
                    //semop(mutex, &up, 1);
				/*Just adds one more doWhile - Invalid input run*/
				}else if (strcmp(token[0], "disc") == 0) {
					printf("Disconnecting Client!\n");
					char menu[] = "Disconnected!\n";
					write(new_socket, menu, strlen(menu));
					/*Second argument: 0 disables receive, 1 disables send, 2 disables both*/
					shutdown(new_socket, 2);
				}					
				bzero(res, RES);
					
			}while(strstr(buffer, "stop") == 0);
			printf("\nExecuting Stop - Stopping Server Socket\n");
			whilestop=1;
			close(new_socket);
		}
	}
		
	/*Writing File Operation*/
	fp = fopen("writing.csv","w");
	
	if (fp == NULL) {
		printf("\nNo file found! - Creating File named: ./writing.csv\n");
	}
	fprintf(fp, "%i;\n", shmdata->realSize);
	fprintf(fp, "%i;\n", shmdata->size);
	for (i=0; i<shmdata->realSize; i++) {
		fprintf(fp, "%i;%s;%s;\n", shmdata->delFlag[i], shmdata->key[i], shmdata->value[i]);
	}
	printf("\nWrote Dataset to File ./writing.csv\n");
	fclose(fp);
	
	/*ServerSocket closed*/
	close(create_socket);
	
	/*Detach Shared Memory*/
	shmdt(shmdata);
	shmctl(shmid, IPC_RMID, 0);
	/*Detach Semaphore*/
	/*Optional, if "semctl (semid, 0, IPC_RMID, 0);" doesnt work*/
	printf("\nOptional, if \"semctl (semid, 0, IPC_RMID, 0)\" doesnt detach semaphore\nDelete semaphore with shell command \"$ ipcrm -s %d\"\n", semid);
//semctl (semid, 0, IPC_RMID, 0);
	/*Kills Child*/
	kill(pid, SIGTERM);
	
	return EXIT_SUCCESS;
}
