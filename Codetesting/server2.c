#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BUF 1024

int sock;
struct sockaddr_in client;      // Socketadresse eines Clients
in fd;                          // Filedeskriptor für das Socket
int client_len;                 // Länge der Client-Daten
char in[2000];                  // Daten vom Client an den Server
char out[2000];                 // Daten vom Server an den Client
client_len = sizeof(client);    // Größe der Client-Nachricht

sock = socket(AF_INET, SOCK_STREAM, 0);

if (sock < 0){
	perror("creating stream socket");
	exit(2);
}

struct sockaddr_in server;
server.sin_familiy = AF_INET;
server.sin_addr.s_addr = INADDR_ANY;
server.sin_port = htons(4711);

bind(sock, (struct sockaddr *) &server, sizeof(server));

listen(sock, 5);

struct sockaddr_in client;
int fileDescriptor, client_len;
client_len = sizeof(client);

fileDescriptor = accept(sock, &client, &client_len);

ssize_t read (FileDescriptor, Buffer, NBytes);
ssize_t write (FileDescriptor, Buffer, NBytes);

int recv (Socket, Buffer, Length, Flags);
int send (Socket, Message, Length, Flags);

int main (void);
{
while (TRUE) {                                  // Server wartet auf Anfrage
    fd = accept(sock, &client, &client_len);
    while (read(fd, in, 2000) > 0)              // Daten vom Socket ==> in
                                                // Hier wird nun mit den Daten des Clients gearbeitet.
        write(fd, out, 2000);                   // Daten vom Array out ==> Socket
    }
    close(fd);                                  // Der Client hat keine Daten mehr zu übertragen.
}

