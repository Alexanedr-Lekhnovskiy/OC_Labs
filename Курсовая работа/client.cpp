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
#include <string>
#include <iostream>

int count(char* str) {
	int i=0;
	while(str[i]!='\0')
	{
		i++;
	}
	return i;
}

void game(int sockfd) {
  while(1) 
  {  
    int check;
    read (sockfd, &check, sizeof(int));
    check = ntohs(check);
    if(check == 0)      //ход наш (загадываем слово)
	{ 
        char word[100];
	    std::cout << "Enter the word" << '\n';
        std::cin >> word;
        std::cout << word << '\n';
        int num =  count(word);
        num = htons(num);
		write (sockfd, &num, sizeof(int)); // передаем число букв в слове
		num = ntohs(num);
		write (sockfd, word, (num+1)*sizeof(char)); // передаем само слово
		if (word[0] == '/' && word[1] == 's')
		break;
		read (sockfd, &check, sizeof(int)); // получаем итог - 1 - пр 2-по
		check = ntohs(check);
		if (check == 1) {
			std::cout << "Your rival is lose " << '\n';
		}
		else std::cout << "Your rival is win!"<< '\n';
    }
    else  //ход не наш (угадываем слово)
	{
		int trying = 6;
		printf("Waiting for word..\n"); 
		int numLet;
		read (sockfd, &numLet, sizeof(int)); //получаем длинну слова
		numLet = ntohs(numLet);
		char word[100];
		for (int i=0; i<numLet; i++) //Инициализируем игровой массив
		{
			word[i]='*';
		}
		word[numLet]='\0';
		std::cout << word << '\n';
		bool fail = false;
		
		do {
		std::cout << "Enter letter!\n";
		char leter;
		int win = 0;
		for (int i=0; i<numLet; i++)
		{
			if (word[i]=='*') break;
			else win++;
		}
		if (win == (numLet))
		{
			leter='\0';
			write (sockfd, &leter, sizeof(char));
			std::cout<<"You win!" << '\n';
			break;
		}
		std::cin >> leter;
		write (sockfd, &leter, sizeof(char)); // отправляем букву
		read (sockfd, &check, sizeof(int)); // читаем число таких букв
		check = ntohs(check);
		if (check == 0) {
			trying--;
		}
		if (check == 9999) {
			fail = true;
			std::cout << "You lose!" << '\n';

			std::cout << "    ______       " << '\n';
			std::cout << "   |/     |      " << '\n';
			std::cout << "   |      O      " << '\n';
			std::cout << "   |     /|\\     " << '\n';
			std::cout << "   |     / \\     " << '\n';
			std::cout << "___|___          " << '\n';
			std::cout << "///////          " << '\n';
			std::cout << '\n';
			break;
		}
		for (int i=0; i<check; i++)
		{
			int pos;
			read (sockfd, &pos, sizeof(int)); // читаем координты буквы
			pos = ntohs(pos);
			word[pos] = leter;
		}
		std::cout << word << '\n';
		if (trying > 1)
		{
			std::cout << "You have " << trying << " attempts" << '\n';

			if (trying == 5)
			{
				std::cout << "    ______       " << '\n';
				std::cout << "   |/     |      " << '\n';
				std::cout << "   |      O      " << '\n';
				std::cout << "   |             " << '\n';
				std::cout << "   |             " << '\n';
				std::cout << "___|___          " << '\n';
				std::cout << "///////          " << '\n';
			}

			if (trying == 4)
			{
				std::cout << "    ______       " << '\n';
				std::cout << "   |/     |      " << '\n';
				std::cout << "   |      O      " << '\n';
				std::cout << "   |      |      " << '\n';
				std::cout << "   |             " << '\n';
				std::cout << "___|___          " << '\n';
				std::cout << "///////          " << '\n';
			}

			if (trying == 3)
			{
				std::cout << "    ______       " << '\n';
				std::cout << "   |/     |      " << '\n';
				std::cout << "   |      O      " << '\n';
				std::cout << "   |      |      " << '\n';
				std::cout << "   |     /       " << '\n';
				std::cout << "___|___          " << '\n';
				std::cout << "///////          " << '\n';
			}

			if (trying == 2)
			{
				std::cout << "    ______       " << '\n';
				std::cout << "   |/     |      " << '\n';
				std::cout << "   |      O      " << '\n';
				std::cout << "   |      |      " << '\n';
				std::cout << "   |     / \\     " << '\n';
				std::cout << "___|___          " << '\n';
				std::cout << "///////          " << '\n';
			}
			std::cout << '\n';
		}
		else
		{
			std::cout << "You have " << trying << " attempt" << '\n';

			if (trying == 1)
			{
				std::cout << "    ______       " << '\n';
				std::cout << "   |/     |      " << '\n';
				std::cout << "   |      O      " << '\n';
				std::cout << "   |      |\\     " << '\n';
				std::cout << "   |     / \\     " << '\n';
				std::cout << "___|___          " << '\n';
				std::cout << "///////          " << '\n';
			}
			std::cout << '\n';
		}
		} while (1);
    }
  }
}



int main(int argc, char *argv[])
{
	int sockfd = 0, n = 0;
	char recvBuff[1024];
	struct sockaddr_in serv_addr;
	if(argc != 3) {
	    printf("\n Usage: %s <ip of server> \n",argv[0]);
	    return 1;
	}

	memset(recvBuff, '0',sizeof(recvBuff));
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
	    printf("\n Error : Could not create socket \n");
	    return 1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
	{
	    printf("\n inet_pton error occured\n");
	    return 1;
	}
	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
	    printf("\n Error : Connect Failed \n");
	    return 1;
	}
    printf("Connection sucsess\n");
    printf("Wait..\n");

	printf("Choose game mode: 1-Pl VS Pl, 2-Pl VS Ps, 3-ConnectGame\n");
	int check;
	do {
	std::cin >> check;
	} while (check != 1 && check != 2 && check != 3);

	if (check !=3) {
		check = htons(check);
		write (sockfd, &check, sizeof(int));
	}
        game(sockfd);
    }
