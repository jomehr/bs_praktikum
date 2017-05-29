#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

/*sharedmemory-spezifisch*/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#define NUM_OF_CHILDS 5
#define SEGSIZE 1024

#define BUF 1024
#define BUFFER_SIZE 1024
#define KV_STRING 50

int strtoken(char *str, char *separator, char **token, int size);
int put(char* key, char* value, char* res);
int get(char* key, char* res);
int del(char* key, char* res);

struct Data{
  char key[BUFFER_SIZE][KV_STRING];
  char value[BUFFER_SIZE][KV_STRING];
  int delFlag[BUFFER_SIZE];           
  int size;                           
  int realSize;                       
};
struct Data KVStore;

int main (void) {
    /*sharedmemory-spezifisch*/
    int i, smid, *shar_mem;
    int pid[NUM_OF_CHILDS];

    int create_socket, new_socket;
    socklen_t addrlen;
    ssize_t size;
    struct sockaddr_in address;
    const int y = 1;
    const char s[2] = "-";
    char* buffer = malloc (BUF);
    char* str[100];
	char* token;
    char key[BUFFER_SIZE];
    char value[BUFFER_SIZE];
    char res[BUFFER_SIZE];
	KVStore.size=0;
    KVStore.realSize=0;
	
    /*SharedMemory erstellen*/
    smid = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT|0600);
    shar_mem= (int*)shmat(smid, 0, 0);
    *shar_mem= 0;
	
    
    if ((create_socket=socket (AF_INET, SOCK_STREAM, 0)) > 0){
		printf ("Socket wurde angelegt\n");
	}
	setsockopt( create_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons (1500);
    if (bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0) {
        printf( "Der Port ist nicht frei!\n");
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
	
    while (1) {
		new_socket = accept ( create_socket, (struct sockaddr *) &address, &addrlen );			
		/*Sharedmemory-spezifisch*/
		for(i = 0; i < NUM_OF_CHILDS; i++){
			if(pid[i] == 0){
				if (new_socket > 0){
					printf ("Ein Client (%s) ist verbunden!\n", inet_ntoa (address.sin_addr));
				}
				do{
					bzero(buffer,BUF);
					size = recv (new_socket, buffer, BUF, 0);
					if ((strlen(buffer)>0) && (buffer[strlen (buffer) - 1] == '\n'))
						buffer[strlen (buffer) - 1] = '\0';
						printf("%s\n", buffer);
						strtoken(buffer, " ", str, 3);
						printf("%s\n", str[0]);
						printf("KEY:%s\n", str[1]);
						printf("KeyLength:%i\n", strlen(str[1]));
						printf("VALUE:%s\n", str[2]);
						printf("ValueLength:%i\n", strlen(str[2]));
						if( strcmp(str[0], "put")==0){
							printf("Jetzt wird die put Funktion ausgeführt\n");
							put(str[1], str[2], res);
							printf("Ergebnis: %s\n", res);
						}
						if( strcmp(str[0], "get")==0){
							printf("Jetzt wird die get Funktion ausgeführt\n");
							get(str[1], res);
							printf("Ergebnis: %s\n", res);
						}
						if( strcmp(str[0], "del")==0){
							printf("Jetzt wird die del Funktion ausgeführt\n");
							del(str[1], res);
							printf("Ergebnis: %s\n", res);
						}
				} while (strcmp (buffer, "quit\n") != 0);
			exit(0);
			}
		}
    }
    /* Der Vaterprozess wartet, bis alle Kindprozessefertig sind.  */
    for(i = 0; i < NUM_OF_CHILDS; i++){
      waitpid(pid[i], NULL, 0);
    }
    /* Das SharedMemory Segment wird abgekoppelt und freigegeben. */
    shmdt(shar_mem);
    shmctl(smid, IPC_RMID, 0);

    return EXIT_SUCCESS;
}

int strtoken(char *str, char *separator, char **token, int size) {

    int i=0;
    token[0] = strtok(str, separator);
    while(i < size && token!=NULL){
      i++;
      token[i] = strtok(NULL, separator);
    }
    return 0;
}

int put(char* key, char* value, char* res){
  int i;
  for(i=0;i<KVStore.size;i++){
    if(strcmp(KVStore.key[i],key)==0){
      strcpy(KVStore.value[i],value);
      strcpy(res,"Put successful! Existing entry replaced.");
      return 1;
    }
  }
  for(i=0;i<KVStore.size;i++){
    if(KVStore.delFlag[i]==1){
        strcpy(KVStore.key[i],key);
        strcpy(KVStore.value[i],value);
        KVStore.delFlag[i]=0;
        KVStore.realSize++;
        strcpy(res,"Put successful! Deleted entry replaced.");
        return 2;
    }
  }
  strcpy(KVStore.key[KVStore.size],key);
  strcpy(KVStore.value[KVStore.size],value);
  //printf("Key: %s this is a test.\n", KVStore.key[KVStore.size]);
  //printf("Value: %s also a test.\n", KVStore.value[KVStore.size]);
  KVStore.size++;
  KVStore.realSize++;
  strcpy(res,"Put successful!");
  return 0;
}

int get(char* key, char* res){
  strcpy(res, "");
  int i;
  for (i=0; i<KVStore.size;i++) {
    //STRCMP!!!!!!!!!!!!
    if(strstr(KVStore.key[i],key)==0){
      strcpy(res,KVStore.value[i]);
      return 0;
    }
  }
  strcpy(res,"Key not found!");
  return 1;
}

int del(char* key, char* res){
  strcpy(res, "");
  int i;
  for(i=0;i<KVStore.size;i++){
    //printf("index %i,strukey %s, parakey %s ,value %s",i, KVStore.key[i], key, KVStore.value[i]);
    //STRCMP!!!!!!!!!!!!
    if(strstr(KVStore.key[i],key)==0){
      strcpy(res,KVStore.value[i]);
      KVStore.delFlag[i]=1;
      strcpy(KVStore.key[i],"");
      strcpy(KVStore.value[i],"");
      KVStore.realSize--;
      return 0;
    }
  }
  strcpy(res,"Key not found!");
  return 1;
}
/*
void list(char* res){
  strcpy(res, "");
  char buf[BUFSIZ];
  snprintf(buf, sizeof(buf), "%d", KVStore.size);//int wird zum char array konvertiert
  strcpy(res, buf);
  if(KVStore.size==0){
    printf("No element found!");
    return;
  }
  int i;
  for(i=0;i<KVStore.size;i++){
    if(KVStore.delFlag[i]!=1){
      printf("Index : %i - Key : %s ; Value : %s ;\n", i, KVStore.key[i], KVStore.value[i]);
    }
  }
  return;
}
*/
