#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BUF 1024
#define RES 1024

int main(int argc , char *argv[]) {
  int create_socket;
  char *buffer = malloc (BUF);
  char *res = malloc (RES);
  struct sockaddr_in address;
  int size;

  //Create socket
	create_socket = socket(AF_INET , SOCK_STREAM , 0);
	if (create_socket < 0) {
		printf("Could not create socket");
    return 1;
	}
	puts("Socket created");

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons (1500);

  if (connect (create_socket, (struct sockaddr *) &address, sizeof (address)) < 0) {
    printf("Could not connect to Server\n");
    return 1;
  }
  printf("Connected with Server (%s) \n", inet_ntoa(address.sin_addr));

  while(1) {
    /* TODO
    hier input als switch create_socket
    */
    //test write Data
    printf("write a string: \n");
    fgets(buffer, BUF, stdin);
    //Send Data
    if (send(create_socket, buffer, BUF, 0) < 0) {
      puts("Send failed");
      return 1;
    }

    if (recv(create_socket, res, RES, 0) < 0) {
      puts("Receive failed");
      break;
    }
    puts("Serverreply: ")
    puts(res);
  }

  close(create_socket);
  return 0;
}
