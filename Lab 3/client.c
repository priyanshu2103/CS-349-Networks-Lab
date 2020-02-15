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

void trade()
{

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
    while(2)
    {
    	char trade_id[100];
    	char password[100];
    	printf("Enter trading ID: ");
	    scanf("%s",trade_id);
	    printf("Enter your password: ");
	    scanf("%s",password);
			char response[1000];
			strcpy(response,trade_id);
			strcat(response," ");
			strcat(response,password);
			// strcat(response,"^");
	    write(sock,response,strlen(response));
			// sleep(2.5);
	    // send(sock,password,strlen(password),0);


	    valread = read(sock,buffer,1000);
      	buffer[valread]='\0';
      	printf("%s\n",buffer);
	    if(strcmp(buffer,"Y")==0)
	    {
	    	printf("Welcome, you are logged in\n");
	    	break;
	    }
	    else if(strcmp(buffer,"N")==0)
			{
				printf("Please enter correct ID and password\n");
			}
    }

    // trade();
	while(2)
	{
		printf("Choose one of the following:\n");
		printf("Enter 1 to see all the buyable and sellable items\n");
		// printf("Enter 2 to see all the sellable items\n");
		printf("Enter 2 to send buy request\n");
		printf("Enter 3 to send sell request\n");
		printf("Enter 4 to view order status\n");
		printf("Enter 5 to view trade status\n");
		printf("Enter 6 to exit\n");

		int choice;
		char t[100];
		scanf("%d",&choice);
		if(choice==6)
		{
			printf("logging off\n");
			break;
		}
		snprintf(t,100,"%d",choice);
		write(sock,t,strlen(t));
		while(1)
		{
			char b[1000];
			bzero(b,1001);
			valread = read(sock,b,1000);
			b[valread]='\0';
			int j=0;
			if(choice==1 || choice==4 || choice==5)
			{
				while(j<strlen(b) && b[j]!='^')
				{
					printf("%c",b[j]);
					j++;
				}
				// printf("%s\n",b);
				if(j<strlen(b) && b[j]=='^')
				{
					//printf("ksjksd");
					break;
				}
				// printf("%s\n",buffer);
			}
			else if(choice==2 || choice==3)
			{
				while(j<strlen(b))
				{
					printf("%c",b[j]);
					j++;
				}
				char it[100],pri[100],quan[100],ans[1000];
				printf("\nEnter item id: ");
				scanf("%s",it);
				//write(sock,it,strlen(it));
				printf("\nEnter price: ");
				scanf("%s",pri);
				//write(sock,pri,strlen(pri));
				printf("\nEnter quantity: ");
				scanf("%s",quan);
				printf("\n");
				strcpy(ans,it);
				strcat(ans,"#");
				strcat(ans,pri);
				strcat(ans,"#");
				strcat(ans,quan);
				strcat(ans,"#");
				write(sock,ans,strlen(ans));
				bzero(b,1001);
				valread = read(sock,b,1000);
				b[valread]='\0';
				j=0;
				while(j<strlen(b) && b[j]!='^')
				{
					printf("%c",b[j]);
					j++;
				}
				// printf("%s\n",b);
				// if(j<strlen(b) && b[j]=='^')
				// {
				// 	//printf("ksjksd");
				// 	break;
				// }
				break;
			}
		}
	}
  return 0;
}
