#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd. h.>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BUF 1024
int main (void) {
  int create_socket, new_socket;
    socklen_t addrlen;
      char *buffer = malloc (BUF);
        ssize_t size;
	  struct sockaddr_in address;
	    const int y = 1;
	      printf ("\e[2J");
	        if ((create_socket=socket (AF_INET, SOCK_STREAM, 0)) > 0)
		    printf ("Socket wurde angelegt\n");
		      setsockopt( create_socket, SOL_SOCKET,
		                    SO_REUSEADDR, &y, sizeof(int));
				      address.sin_family = AF_INET;
				        address.sin_addr.s_addr = INADDR_ANY;
					  address.sin_port = htons (15000);
					    if (bind ( create_socket,
					                 (struct sockaddr *) &address,
							              sizeof (address)) != 0) {
								          printf( "Der Port ist nicht frei – belegt!\n");
									    }
									      listen (create_socket, 5);
									        addrlen = sizeof (struct sockaddr_in);
										  while (1) {
										       new_socket = accept ( create_socket,
										                                  (struct sockaddr *) &address,
														                             &addrlen );
																	          if (new_socket > 0)
																		        printf ("Ein Client (%s) ist verbunden ...\n",
																			         inet_ntoa (address.sin_addr));
																				      do {
																				              printf ("Nachricht zum Versenden: ");
																					              fgets (buffer, BUF, stdin);
																						              send (new_socket, buffer, strlen (buffer), 0);
																							              size = recv (new_socket, buffer, BUF-1, 0);
																								              if( size > 0)
																									                 buffer[size] = '\0';
																											         printf ("Nachricht empfangen: %s\n", buffer);
																												      } while (strcmp (buffer, "quit\n") != 0);
																												           close (new_socket);
																													     }
																													       close (create_socket);
																													         return EXIT_SUCCESS;
																														 }
