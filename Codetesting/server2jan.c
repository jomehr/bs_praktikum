#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#define BUF 1024
#define BUFFER_SIZE 1024
#define KV_STRING 50
#define RES 100

int put(char* key, char* value, char* res);
int get(char* key, char* res);
int del(char* key, char* res);
void list(char* res);

struct Data{
  char key[BUFFER_SIZE][KV_STRING];
  char value[BUFFER_SIZE][KV_STRING];
  int delFlag[BUFFER_SIZE];           //1 = geloescht
  int size;                           //letzter Index vom Array
  int realSize;                       //Menge der Eintraege im Array
};
//Hier wird nur die Variable KVStore um Funktionalitaet erweitert
//und nicht das struct selbst!
struct Data KVStore;

int main (void) {
	int create_socket, new_socket;
	socklen_t addrlen;
	char *buffer = malloc (BUF);
	ssize_t size;
	struct sockaddr_in address;
	const int y = 1;
	char *str[100];
	char key[BUFFER_SIZE];
	char value[BUFFER_SIZE];
	char res[BUFFER_SIZE];

	if ((create_socket=socket (AF_INET, SOCK_STREAM, 0)) > 0)
		printf ("Socket wurde angelegt\n");

	setsockopt( create_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons (1500);

	if (bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0) {
		printf( "Der Port ist nicht frei – belegt!\n");
	}

	listen (create_socket, 5);
	addrlen = sizeof (struct sockaddr_in);

	while (1) {
		new_socket = accept ( create_socket, (struct sockaddr *) &address, &addrlen );

		if (new_socket > 0)
			printf ("Ein Client (%s) ist verbunden ...\n", inet_ntoa (address.sin_addr));

    write(new_socket, "Geben Sie die Funktion put / get / del mit den benötigten Parametern ein: \n", 80);

    do {
    bzero(buffer,BUF);
    recv (new_socket, buffer, BUF, 0);

		strtoken(buffer, " ", str, 3);    //Clienteingabe wird in 3 Parameter geteilt
		printf("%s %s %s\n", str[0], str[1], str[2]);

  		if( strcmp(str[0], "put")==0){
  			printf("Jetzt wird die put Funktion ausgeführt\n");
  			put(str[1], str[2], res);
        write(new_socket, res, RES);
      } else { if (strcmp(str[0], "get")==0){
    			printf("Jetzt wird die get Funktion ausgeführt\n");
    			get(str[1], res);
    			//printf("Ergebnis: %s\n", res);
          write(new_socket, res, RES);
        } else { if( strcmp(str[0], "del")==0){
            printf("Jetzt wird die del Funktion ausgeführt\n");
            del(str[1], res);
            //printf("Ergebnis: %s\n", res);
            write(new_socket, res, RES);
          } else{
            printf("Die Eingabe war fehlerhaft\n");
            write(new_socket, "Fehlerhafte Eingabe\n", 20);
          }
        }
      }
    } while (strstr(*str, "quit!")==0);

  close(new_socket);
  close(create_socket);
  return EXIT_SUCCESS;
  }
}

int strtoken(char *str, char *separator, char **token, int size) {
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
  snprintf(buf, sizeof(buf), "%d", KVStore.size);//int wird zum char array konvertiert
  strcpy(res, buf);
  if(KVStore.size==0){
    printf("No element found!\n");
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
