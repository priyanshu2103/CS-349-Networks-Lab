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
	printf("Choose one of the following:\n");
    printf("Enter 1 to see all the buyable items\n");
    printf("Enter 2 to see all the sellable items\n");
    printf("Enter 3 to send buy request\n");
    printf("Enter 4 to send sell request\n");
    printf("Enter 5 to view order status\n");
    printf("Enter 6 to view trade status\n");

    int choice;
    scanf("%d",&choice);

    // switch choice
    // {
    // 	case 1:
    // 		break;
    // 	case 2:
    // 		break;
    // 	case 3:
    // 		break;
    // 	case 4:
    // 		break;
    // 	case 5:
    // 		break;
    // 	case 6:
    // 		break;
    // 	default:
    // 		break;
    // }
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
    char buffer[1024] = {0};
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

    printf("Welcome to the trading system\n\n");
    while(2)
    {
    	char trade_id[10];
    	char password[100];
    	printf("Enter trading ID: ");
	    scanf("%s",trade_id);
	    printf("Enter your password: ");
	    scanf("%s",password);

	    send(sock,trade_id,strlen(trade_id),0);
	    send(sock,password,strlen(password),0);
	    valread = read(sock,buffer,1024); 
	    if(strcmp(buffer,"Y")==0)
	    {
	    	printf("Welcome you are authenticated\n");
	    	break;
	    }
	    else
	    	printf("Please enter correct ID and password\n");
    }

    trade();
    
    return 0;
}