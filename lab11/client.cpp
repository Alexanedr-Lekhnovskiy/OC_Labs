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
#include <time.h>

int main(int argc, char* argv[])
{
	char* addr = argv[1];
	int port = atoi(argv[2]);
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;

	int Size, Max, Min;
	int array[Size];
	printf("Введите минимальное, максимальное значение и размер массива: ");
	scanf("%d %d %d", &Min, &Max, &Size);

	srand(time(NULL));
	for (int i = 0; i < Size; ++i)
	{
		array[i] = Min + rand() % (Max - Min + 1);
	}
	printf("Сгенерированый массив случайных чиссел: ");
	for (int i = 0; i < Size; ++i)
	{
		printf("%d ", array[i]);
	}
	printf("\n");



	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Socket creation failed \n");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	bzero(&cliaddr, sizeof(cliaddr));

	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(0);
	cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
       
        servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(addr);

	if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
	{
		perror("Connect faild \n");
		close(sockfd);
		exit(1);
	}

	
	printf("Connection sucsess\n");

	
	time_t startTime = time(NULL);
	write(sockfd, (const int *) array, sizeof(int) * (Size));
	printf("Array sent. \n");
	read(sockfd, (int *) array, sizeof(int) * (Size));
	time_t endTime = time(NULL);

	printf("Отсортированный массив чисел:\n");
	for (size_t i = 0; i < Size; ++i)
	{
		printf("%d ", array[i]);
	}
	printf("\n");
	if (endTime - startTime > 0)
	{
		printf("На работу затрачено %ld секунд\n", endTime - startTime);
	}
	else
	{
		printf("На работу затрачено менее 1 секунды\n");
	}
	close(sockfd);
	return 0;
}
