#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int compa(const void* x, const void* y)
{
	return *((int *) y) - *((int *) x);
}

int main(int argc, char* argv[])
{
	int port = atoi(argv[1]);
	int sockfd, newsockfd;
	int buffer[1000];
	struct sockaddr_in servaddr, cliaddr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Socket creation failed \n");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if (bind(sockfd, (const struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
	{
		servaddr.sin_port = 0;

		if (bind(sockfd, (const struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
		{
			perror("bind failed");
			exit(1);
		}
	}

	if (listen(sockfd, 1) < 0)
	{
		perror("Listen failed \n");
		close(sockfd);
		exit(1);
	}

        socklen_t server_len=sizeof(servaddr);
        getsockname(sockfd, (struct sockaddr *)&servaddr, &server_len);
	printf("Server start on port: %d\n", ntohs(servaddr.sin_port));

	while (1)
	{
		socklen_t clilen = sizeof(cliaddr);
		if ((newsockfd = accept(sockfd, (struct sockaddr*) & cliaddr, &clilen)) < 0)
		{
			perror("Accept failed");
			close(sockfd);
			exit(1);
		}

		int n;
		n = read(newsockfd, (int*)buffer, 1000);

		qsort(buffer, n / sizeof(int), sizeof(int), compa);

		write(newsockfd, (int *)buffer,  n * sizeof(int));
		printf("Sort sent.\n");

		close(newsockfd);
		return 0;
	}
}
