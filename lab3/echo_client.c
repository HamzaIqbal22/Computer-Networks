/* A simple echo client using TCP */
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <errno.h>



#define SERVER_TCP_PORT 3000	/* well-known port */
#define BUFLEN		100	/* buffer length */

int main(int argc, char **argv)
{
	int 	n, i, bytes_to_read;
	int 	sd, port;
	struct	hostent		*hp;
	struct	sockaddr_in server;
	char	*host, *bp, rbuf[BUFLEN], sbuf[BUFLEN];

	switch(argc){
	case 2:
		host = argv[1];
		port = SERVER_TCP_PORT;
		break;
	case 3:
		host = argv[1];
		port = atoi(argv[2]);
		break;
	default:
		fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
		exit(1);
	}

	/* Create a stream socket	*/
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Can't creat a socket\n");
		exit(1);
	}

	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (hp = gethostbyname(host))
	  bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	else if ( inet_aton(host, (struct in_addr *) &server.sin_addr) ){
	  fprintf(stderr, "Can't get server's address\n");
	  exit(1);
	}

	/* Connecting to the server */
	if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
	  fprintf(stderr, "Can't connect \n");
	  exit(1);
	}

	printf("Enter the file you wish to recieve from the server: \n");
	n=read(0, sbuf, BUFLEN);	/* get user message */
	write(sd, sbuf, n);		/* send it out */
	bp = rbuf;
	bytes_to_read = n;

	/* Create file where data will be stored */
	FILE *fp;
	int receivedBytes = 0;
	char fileName[100];
	read(sd, fileName, BUFLEN);
	if (strcmp(fileName,"File not found") == 0){
		printf("File not found, closing channel..\n");
		close(sd);
		return 0;
	}
	printf("Name of File: %s\n",fileName);
	printf("Attempting to receiving file...\n");
   	fp = fopen(fileName, "wb");
    	if(fp == NULL)
    	{
       	 printf("Error opening file\n");
       	 perror("Error");
         return 1;
    	}
		int packetNumber = 0;
		int totalNumofBytes=0;


    /* Retrieve data in blocks of 100 bytes */
    while((receivedBytes = read(sd, sbuf, BUFLEN)) > 0){
        packetNumber++;
        totalNumofBytes += receivedBytes;
        fwrite(sbuf, 1,receivedBytes,fp);
        printf("Number of bytes recieved on packet %d are: %d\n",packetNumber, receivedBytes);
        if (receivedBytes < BUFLEN){
        printf("Total number of bytes recieved: %d\n", totalNumofBytes);
        break;
        }

    }

    if(receivedBytes < 0){
        printf("\n Read Error \n");
    }

    printf("\nComplete\n");
	 fclose(fp);
	 close(sd);
	 return(0);
}
