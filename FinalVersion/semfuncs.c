//#include "semfuncs.h"
#include <stdio.h>
#include <sys/sem.h>

//struct sembuf for semaphore operations
struct sembuf semaphore;

int create_semaphore(){
	//id for semaphore group -semget@params:key(created by UNIX Kernel if IPC_PRIVATE),number of semaphores,flag
	int id = semget(IPC_PRIVATE,1,IPC_CREAT | 0777);
	if(id<0){
		printf("Error occured while creating semaphore");
	}
	unsigned short marker[1];
	marker[0] = 1;
	//-semctl@params:id,number of semaphores, command(SETALL sets values for semaphores), buffer(depends on cmd) 
	if(semctl(id, 0, SETALL, marker)  < 0){
		printf("Error occured while initializing Semaphore(CTL)");
		return -1;
	}
	printf("Semaphore ID: %d\n",id);
	return id;
}

int semaphore_op(int op, int sem){
	//value>0 executes UP-operation
	semaphore.sem_op = op;
	//SEM_UNDO automatically undoes when process terminates
	semaphore.sem_flg = SEM_UNDO;
	if(semop(sem, &semaphore, 1) == -1){
		perror("Error while executing semaphore operation");
		return -1;
	}
	return 1;
}