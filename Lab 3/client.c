#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <termios.h>
#include <limits.h>
#include <time.h>

// hide password while entering the login details
int getch()
{
    struct termios oldtc, newtc;
    int ch;
    tcgetattr(STDIN_FILENO, &oldtc);
    newtc = oldtc;
    newtc.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newtc);
    ch=getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
    return ch;
}

int main(int argc,char* argv[])
{
	if(argc<3)
	{
		printf("Enter server IP address and server PORT");
		return 0;
	}
	int sock=0,valread;
	struct sockaddr_in serv_addr;
	char buffer[1000] = {0};
	if((sock = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}
	valread = read(sock,buffer,1000);
	buffer[valread]='\0';
	printf("Welcome to the trading system\n");
	printf("%s",buffer);
	// Login part
	while(1)
	{
		char trade_id[100];
		char password[100];
		printf("Enter trading ID: ");
		scanf("%s",trade_id);
		char ch;
		ch=getch();
		printf("Enter your password: ");

    int p=0;
    for (;;)
    {
        ch = getch();
        password[p]=ch;
        p++;
        if(ch == '\n')
              break;
  	}
  	password[p-1]='\0';
		char response[1000];
		strcpy(response,trade_id);
		strcat(response," ");
		strcat(response,password);
		write(sock,response,strlen(response));

		valread = read(sock,buffer,1000);
		buffer[valread]='\0';
		if(strcmp(buffer,"Y")==0)
		{
			printf("Welcome, you are logged in\n");
			break;
		}
		else if(strcmp(buffer,"N")==0)
			printf("Please enter correct ID and password\n");
	}
	// Trading part
	while(1)
	{
		printf("Choose one of the following:\n");
		printf("Enter 1 to see all the buyable and sellable items\n");
		printf("Enter 2 to send buy request also enter item id <space> price <space> quantity\n");
		printf("Enter 3 to send sell request also enter item id <space> price <space> quantity\n");
		printf("Enter 4 to view order status\n");
		printf("Enter 5 to view trade status\n");
		printf("Enter 6 to exit\n");

		char choice[100];
		char t[100];
		fgets(choice,INT_MAX,stdin);
    	// printf("choice is %s\n",choice);
		if(choice[0]=='6')
		{
			printf("logging off\n");
			break;
		}
    if(choice[0]=='1'||choice[0]=='4'||choice[0]=='5')
    {
      	snprintf(t,100,"%c",choice[0]);
    }
    else if(choice[0]=='2'||choice[0]=='3')
    {
    	int choice_num,item,price,quantity;
    	choice_num=choice[0]-'0';
    	// printf("%d\n",choice_num);
      char t1[100],t2[100],t3[100],t4[100];
      int j=1,k=0,l=0,m=0;
      while(choice[j]==' '&&j<100)
      	j++;
      while(choice[j]>='0' && choice[j]<='9')
      {
        t1[k]=choice[j];
        j++;
        k++;
      }
      t1[k]='\0';
      item=atoi(t1);
      while(choice[j]==' '&&j<100)
      	j++;
      while(choice[j]>='0' && choice[j]<='9')
      {
        t3[l]=choice[j];
        l++;
        j++;
      }
      t3[l]='\0';
      price=atoi(t3);
      while(choice[j]==' '&&j<100)
      	j++;
      while(choice[j]>='0' && choice[j]<='9')
      {
        t4[m]=choice[j];
        m++;
        j++;
      }
      t4[m]='\0';
      quantity=atoi(t4);
      if(!(item>=1&&item<=10))
      {
      	printf("Enter item ID between 1 and 10\n");
      	continue;
      }
      if(price<=0)
      {
      	printf("Wrong Input, Please enter in correct format\n");
      	continue;
      }
      if(quantity<=0)
      {
      	printf("Wrong Input, Please enter in correct format\n");
      	continue;
      }
      snprintf(t,100,"%d %d %d %d",choice_num,item,price,quantity);
    }
    else
    	continue;

		write(sock,t,strlen(t));
		while(1)
		{
			char b[1000];
			bzero(b,1001);
			valread = read(sock,b,1000);
			if(valread<0)
			{
				printf("Read Error\n");
				break;
			}
			b[valread]='\0';
			int j=0;
			while(j<strlen(b) && b[j]!='^')
			{
				printf("%c",b[j]);
				j++;
			}
			if(j<strlen(b) && b[j]=='^')
			{
				break;
			}
		}
	}
	return 0;
}
