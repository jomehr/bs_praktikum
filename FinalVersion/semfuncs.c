//#include "semfuncs.h"
#include <stdio.h>
#include <sys/sem.h>

struct sembuf semaphore;

int create_semaphore(){
	int id = semget(IPC_PRIVATE,1,IPC_CREAT | 0777);
	if(id<0){
		printf("Error occured while creating semaphore");
	}
	unsigned short marker[1];
	marker[0] = 1;
	if(semctl(id, 0, SETALL, marker)  < 0){
		printf("Error occured while initializing Semaphore(CTL)");
		return -1;
	}
	printf("Semaphore ID: %d\n",id);
	return id;
}

int semaphore_op(int op, int sem){
	semaphore.sem_op = op;
	semaphore.sem_flg = SEM_UNDO;
	if(semop(sem, &semaphore, 1) == -1){
		perror("Error while executing semaphore operation");
		return -1;
	}
	return 1;
}