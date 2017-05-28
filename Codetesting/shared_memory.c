
#define NUM_OF_CHILDS 2
#define SEGSIZE sizeof(int)

void main(){
  id = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT|0600);
  shar_mem= (int*)shmat(id, 0, 0);
  *shar_mem= 0;

  for(i = 0; i < NUM_OF_CHILDS; i++){
    pid[i] = fork();
    if(pid[i] == -1){
      printf("Kindprozess konnte nicht erzeugt werden!\n");
      exit(1);
    }
  }
  for(i = 0; i < NUM_OF_CHILDS; i++){
    if(pid[i] == 0){
       /* Kindprozess-spezifischer Code */
      }
      exit(0);
    }
  }
  /* Der Vaterprozess wartet, bis alle Kindprozessefertig sind.  */
  for(i = 0; i < NUM_OF_CHILDS; i++){
    waitpid(pid[i], NULL, 0);
  }
  /* Das SharedMemory Segment wird abgekoppelt und freigegeben. */
  shmdt(shar_mem);
  shmctl(id, IPC_RMID, 0);
}
