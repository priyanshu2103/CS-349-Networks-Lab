#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "queue.c"
#include "transactions.c"

queue * sell[11];
queue * buy[11];
tqueue * trans;

void available(){

	printf("These items are available for buying\n");
	bool flag = false;
	for(int i = 1; i  < 11; i++){

		if(sell[i]->start!=NULL){
			printf("%d     ", i);
			flag = true;
		}

	}
	if(!flag) printf("<-- No items available --> \n");
	flag  = false;
	printf("These items are available for selling\n");
	for(int i = 1; i  < 11; i++){

		if(buy[i]->start!=NULL){
			printf("%d     ", i);
			flag = true;
		}

	}
	if(!flag) printf("<-- No items available --> \n");
	printf("On sending a buy/sell request for any of tht items not listed in the respective fields, your request will be stored and served asap \n");
}


void serve_buy_request(int item, int trader, int price, int amount){

	 while (sell[item]->start != NULL && sell[item]->start->Price <= price && amount > 0){

	 	if(sell[item]->start->amount > amount){

	 		sell[item]->start->amount = sell[item]->start->amount - amount;
	 		push(trans, item, sell[item]->start->Trader_ID, trader, sell[item]->start->Price, amount);
	 		amount = 0;

	 	}else{

	 		amount = amount - sell[item]->start->amount;
	 		push(trans, item, sell[item]->start->Trader_Id , trader, sell[item]->start->Price, sell[item]->start->amount);
	 		sell[item]->start->amount = 0;

	 		pop(sell[item]);

	 	}

	 }
	 if(amount > 0) {

	 	pushb(buy[i], item, trader, price, count);

	 }
	 return;

}




void serve_sell_request(int item, int trader, int price, int amount){

	 while (buy[item]->start != NULL && buy[item]->start->Price >= price && amount > 0){

	 	if(buy[item]->start->amount > amount){

	 		buy[item]->start->amount = buy[item]->start->amount - amount;
	 		push(trans, item, trader,  buy[item]->start->Trader_ID, buy[item]->start->Price, amount);
	 		amount = 0;

	 	}else{

	 		amount = amount - buy[item]->start->amount;
	 		push(trans, item, trader, buy[item]->start->Trader_ID , sell[item]->start->Price, sell[item]->start->amount);
	 		buy[item]->start->amount = 0;
	 		pop(buy[item]);

	 	}

	 }
	 if(amount > 0) {

	 	pushs(sell[i], item, trader, price, count);

	 }
	 return;

}


void view_order(){


	for(int i = 1; i < 11; i++){

		printf("ItemID: %d     ",  i);

		if(buy[i]->start != NULL) printf("Best Buy: %d     ",  buy[i]->start->Price );
		else printf("<-- No buyers -->");

		if(sell[i]->start!=NULL ) printf("Best Sell: %d \n", sell[i]->start->Price);
		else printf("<-- No sellers --> \n");

	}
	return;

}

void trade_status(int trader){

	printf("Settled Transactions: \n");
	tnode * temp = trans->start;
	while(temp != NULL){

		if(temp->buyer == trader){
			printf("%d number of Item %d bought from %d at %dRupees \n", temp->amount, temp->item, temp->seller, temp->price);
		}else if{
			printf("%d number of Item %d sold to %d\n at %dRupees\n", temp->amount, temp->item, temp->buyer, temp->price);
		}
		temp = temp->next;
	}
	printf("\n\nPending Transactions: \n");
	for(int i = 1; i < 11; i++){

		node * temp = buy[i]->start;
		while(temp!=NULL){

			if(temp->Trader_Id == trader){
				printf("%d number of Item %d are queued for buying at price %d\n", temp->count, temp->Item_Id, temp->Price);
			}
			temp = temp->next;
		}

	}
	for(int i = 1; i < 11; i++){

		node * temp = sell[i]->start;
		while(temp!=NULL){

			if(temp->Trader_Id == trader){
				printf("%d number of Item %d are queued for selling at price %d\n", temp->count, temp->Item_Id, temp->Price);
			}
			temp = temp->next;
		}

	}


}


int main(int argc, char *argv[])
{
  //printf("jojojojojojojojo\n");
    sell[0] = NULL;
    buy[0] = NULL;
    for(int i = 1 ; i <11; i++){

    	sell[i] = create_queue();
    	buy[i] = create_queue();

    }



    if(argc<2)
    {
      printf("Please enter server port number\n");
      return 0;
    }
    //printf("%s",argv[1]);
    int port=atoi(argv[1]);
    printf("%d\n",port);
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // else
    // {
    //   printf("socket formed\n");
    // }
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // printf("bef lis\n");
    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // printf("aft lis\n");
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    // printf("aft new_socket\n");
    valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    send(new_socket , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    return 0;
}
