#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#define BUF 1024
#define RES 1024

int main(int argc , char *argv[]) {
  int clientsocket;
  char *buffer = malloc (BUF);
  char *res = malloc (RES);
  struct sockaddr_in address;
  int size;

  //Create socket
	clientsocket = socket(AF_INET , SOCK_STREAM , 0);
	if (clientsocket < 0) {
		printf("Could not create socket");
    return 1;
	}
	puts("Socket created");

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons (1500);

  if (connect (clientsocket, (struct sockaddr *) &address, sizeof (address)) < 0) {
    printf("Could not connect to Server\n");
    return 1;
  }
  printf("Connected with Server (%s) \n", inet_ntoa(address.sin_addr));

  while(1) {

    if (recv(clientsocket, res, RES, 0) < 0) {
      puts("Receive failed!");
      break;
    }

    //printf("\nSelect: put | get | del | list | disc | stop\n");
    puts(res);
    printf("Input your command: \n");
    fgets(buffer, BUF, stdin);

    //Send Data
    if (send(clientsocket, buffer, BUF, 0) < 0) {
      puts("Send failed!");
      return 1;
    }else {
      puts("Send success!");
      //puts(buffer);
    }

    //Receive Data
    if (recv(clientsocket, res, RES, 0) < 0) {
      puts("Receive failed!");
      break;
    }
    puts("Server reply:");
    if ((strlen(res)>0) && (res[strlen (res) - 1] == '\n')) {
      res[strlen (res) - 1] = ' ';//\0 for telnet
    }
    printf("%s", res);

    //Cleanse Buffer
    bzero(buffer, BUF);
    bzero(res, RES);
  }

  close(clientsocket);
  return 0;
}
