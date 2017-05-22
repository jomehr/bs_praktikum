#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BUF 1024
#define ROW 100
#define COL 3
char value[20];
char key[20];
int x=0;
int j=0;
int index_int=1;
int itemNum=0;
char *kv[ROW][COL];
int i=0;

int inputKey();
char* get(char *key, char *keyval[ROW][COL]);
int put(char *key, char *value, char *keyval[ROW][COL]);
char* del(char *key, char *keyval[ROW][COL]);

int main (void) {
	int create_socket, new_socket;
	socklen_t addrlen;
	char *buffer = malloc (BUF);
	ssize_t size;
	struct sockaddr_in address;
	const int y = 1;
	char res[20];

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

		do {
			char string[50];
			char* token;
			size = recv (new_socket, buffer, BUF-1, 0);
			printf("%s\n", buffer);

			char delimiter[] = " ";
			token = strtok(string, delimiter);

			while(token != NULL) {
				printf("Abschnitt gefunden: %s\n", token);
				// naechsten Abschnitt erstellen
 				token = strtok(NULL, delimiter);
			}

			if( size > 0)
				buffer[size] = '\0';

		while (strstr (buffer, "quit") != 0);

		close (new_socket);
	}

	close (create_socket);

	return EXIT_SUCCESS;
}
}

int inputKey(){
    printf("Enter key: ");
    scanf("%s",key);
    return 0;
	}

char* get(char* key, char* keyval[ROW][COL], char* res){
  strcpy(res,"-1");
  int i;
  for (i=0;i<ROW;i++) {
    if(strcmp(keyval[i][0],key)==0){
      strcpy(res,keyval[i][1]);
      return res;
		}
  }
  return res;
}

int put(char* key, char* value, char* keyval[ROW][COL]){
  int i;
  /*for (i=0;i<ROW;i++) {
    if(strcmp(keyval[i][0],key)==0){
      strcpy(keyval[i][1],value);
      return 0;
    }
  }*/
  for(i=0;i<ROW;i++) {
    if(keyval[i][0]==NULL){
      printf("%s found at %i\n",keyval[i][0],i);
      strcpy(keyval[i][0],key);
      printf("Yes1\n");
      strcpy(keyval[i][1],value);
      return 0;
    }
  }
  return -1;
}

char* del(char* key, char* keyval[ROW][COL], char* res){
  strcpy(res,"-1");
  int i;
  for (i=0;i<ROW;i++) {
    if(strcmp(keyval[i][0],key)==0){
      strcpy(res,keyval[i][0]);
      keyval[i][0]=NULL;
      keyval[i][1]=NULL;
      break;
    }
  }
  return res;
}

// while(1){
// 	printf("Select 1:Put 2:Get 3:Delete 4:Exit\n");
// 	scanf("%i",&itemNum);
// 	switch(itemNum){
// 		case 1:
// 			printf("1.Put selected.\n");
// 			inputKey(key);
// 			printf("Enter value: ");
// 			scanf("%s",value);
// 			put(key, value, kv);
// 			break;
// 		case 2:
// 			printf("2.Get selected.\n");
// 			inputKey(key);
// 			printf("Value: %s\n", get(key, kv, res));
// 			break;
// 		case 3:
// 			printf("3.Delete selected.\n");
// 			inputKey(key);
// 			del(key, kv, res);
// 			break;
// 		default:
// 			printf("Program ended.");
// 			return 0;
// 		}
// 	}
