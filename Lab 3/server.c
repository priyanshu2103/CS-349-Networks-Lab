#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include "queue.c"
#include "transactions.c"
#define AVAILABLE "1"         // .............may need to make these strings
#define BUY_REQUEST "2"
#define SELL_REQUEST "3"
#define VIEW_ORDER_STATUS "4"
#define VIEW_TRADE_STATUS "5"
queue * sell[11];
queue * buy[11];
tqueue * trans;

//......................write all these printfs into messages to send to server  ......using snprintf for this
//......................add print stmts in serve_buy_request and serve_sell_request
//......................may need to terminate buffer by \0 when receiving
//......................may need to change functions involving char * as parameter or return type
//......................warning: function returns address of local variable [-Wreturn-local-addr]
//......................         in case of return buffer;
//......................can resolve if it works as for available()
//......................make the auth.txt file
char * available(char *buffer)
{
  // char buffer[5000];
  snprintf(buffer,1000,"These items are available for buying\n");
  bool flag = false;
  for(int i = 1; i  < 11; i++)
  {
    if(sell[i]->start!=NULL)
    {
      snprintf(buffer,1000,"%d    ", i);
      flag = true;
    }
  }
  if(!flag)
  {
    snprintf(buffer,1000,"<-- No items available --> \n");
  }
  flag  = false;
  snprintf(buffer,1000,"These items are available for selling\n");
  for(int i = 1; i  < 11; i++)
  {
    if(buy[i]->start!=NULL)
    {
      snprintf(buffer,1000,"%d     ", i);
      flag = true;
    }
  }
  if(!flag)
  {
    snprintf(buffer,1000,"<-- No items available --> \n");
  }
  snprintf(buffer,1000,"On sending a buy/sell request for any of the items not listed in the respective fields, your request will be stored and served asap \n");
  return buffer;
}

char * serve_buy_request(int item, int trader, int price, int quantity)
{
  char buffer[5000];
  bool flag=false;
  while (sell[item]->start != NULL && sell[item]->start->Price <= price && quantity > 0)
  {
    flag=true;
    if(sell[item]->start->count > quantity)
    {
      sell[item]->start->count = sell[item]->start->count - quantity;
      push(trans, item, sell[item]->start->Trader_Id, trader, sell[item]->start->Price, quantity);
      snprintf(buffer,1000,"%d number of Item %d bought from %d at %dRupees \n", quantity, item, sell[item]->start->Trader_Id, price);
      quantity = 0;
    }
    else
    {
      quantity = quantity - sell[item]->start->count;
      push(trans, item, sell[item]->start->Trader_Id , trader, sell[item]->start->Price, sell[item]->start->count);
      snprintf(buffer,1000,"%d number of Item %d bought from %d at %dRupees \n", sell[item]->start->count, item, sell[item]->start->Trader_Id, price);
      sell[item]->start->count = 0;
      pop(sell[item]);
    }
  }
  if(flag==false)
  {
    snprintf(buffer,1000,"No current seller, your request will be stored and served asap \n");
  }
  if(quantity > 0)
  {
    pushb(buy[item], item, trader, price, quantity);
  }
  return buffer;
}

char * serve_sell_request(int item, int trader, int price, int quantity)
{
  char buffer[5000];
  bool flag=false;
  while (buy[item]->start != NULL && buy[item]->start->Price >= price && quantity > 0)
  {
    flag=true;
    if(buy[item]->start->count > quantity)
    {
      buy[item]->start->count = buy[item]->start->count - quantity;
      push(trans, item, trader,  buy[item]->start->Trader_Id, buy[item]->start->Price, quantity);
      snprintf(buffer,1000,"%d number of Item %d sold to %d at %dRupees \n", quantity, item, buy[item]->start->Trader_Id, price);
      quantity = 0;
    }
    else
    {
      quantity = quantity - buy[item]->start->count;
      push(trans, item, trader, buy[item]->start->Trader_Id , sell[item]->start->Price, sell[item]->start->count);
      snprintf(buffer,1000,"%d number of Item %d sold to %d at %dRupees \n", buy[item]->start->count, item, buy[item]->start->Trader_Id, price);
      buy[item]->start->count = 0;
      pop(buy[item]);
    }
  }
  if(flag==false)
  {
    snprintf(buffer,1000,"No current buyer, your request will be stored and served asap \n");
  }
  if(quantity > 0)
  {
    pushs(sell[item], item, trader, price, quantity);
  }
  return buffer;
}

char * view_order()
{
  char buffer[5000];
  for(int i = 1; i < 11; i++)
  {
    snprintf(buffer,1000,"ItemID: %d     ",  i);
    if(buy[i]->start != NULL)
    {
      snprintf(buffer,1000,"Best Buy: %d     ",  buy[i]->start->Price );
    }
    else
    {
      snprintf(buffer,1000,"<-- No buyers -->");
    }
    if(sell[i]->start!=NULL )
    {
      snprintf(buffer,1000,"Best Sell: %d \n", sell[i]->start->Price);
    }
    else
    {
      snprintf(buffer,1000,"<-- No sellers --> \n");
    }
  }
  return buffer;
}

char * trade_status(int trader)
{
  char buffer[5000];
  snprintf(buffer,1000,"Settled Transactions: \n");
  tnode * temp = trans->start;
  while(temp != NULL)
  {
    if(temp->buyer == trader)
    {
      snprintf(buffer,1000,"%d number of Item %d bought from %d at %dRupees \n", temp->quantity, temp->item, temp->seller, temp->price);
    }
    if(temp->seller == trader)
    {
      snprintf(buffer,1000,"%d number of Item %d sold to %d\n at %dRupees\n", temp->quantity, temp->item, temp->buyer, temp->price);
    }
    temp = temp->next;
  }
  snprintf(buffer,1000,"\nPending Transactions: \n");
  for(int i = 1; i < 11; i++)
  {
    node * temp = buy[i]->start;
    while(temp!=NULL)
    {
      if(temp->Trader_Id == trader)
      {
        snprintf(buffer,1000,"%d number of Item %d are queued for buying at price %d\n", temp->count, temp->Item_Id, temp->Price);
      }
      temp = temp->next;
    }
  }
  for(int i = 1; i < 11; i++)
  {
    node * temp = sell[i]->start;
    while(temp!=NULL)
    {
      if(temp->Trader_Id == trader)
      {
        snprintf(buffer,1000,"%d number of Item %d are queued for selling at price %d\n", temp->count, temp->Item_Id, temp->Price);
      }
      temp = temp->next;
    }
  }
  return buffer;
}

bool search(char id[],char pass[])
{
  FILE * fp=fopen("auth.txt","r");
  if(!fp)
  {
    return false;  // ..............can write file not available if needed for closure in error handling
  }
  char temp[1024];
  bool flag=false;
  while(fgets(temp, INT_MAX, fp) != NULL)
  {
    if(strcmp(strtok(temp, " "), id) == 0)
    {
      if(strcmp(strtok(temp, " "), pass) == 0)  //............may be wrong not sure about strtok
      {
        flag=true;
      }
      break;
    }
  }
  return flag;
}

int main(int argc, char *argv[])
{
  //printf("jojojojojojojojo\n");
  sell[0] = NULL;
  buy[0] = NULL;
  for(int i = 1 ; i <11; i++)
  {
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
  int opt = 1;
  int master_socket , addrlen , new_socket , client_socket[5] , max_clients = 5 , activity, i , valread , sd ,login[5]; // login[i]=trader_id for client_socket[i]
  int max_sd;
  struct sockaddr_in address;
  char buffer[1025];  //data buffer of 1K
  //set of socket descriptors
  fd_set readfds;
  //initialise all client_socket[] to 0 so not checked
  for (i = 0; i < max_clients; i++)
  {
    client_socket[i] = 0;
    login[i]=-1;
  }

  //create a master socket
  if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  //set master socket to allow multiple connections ,
  //this is just a good habit, it will work without this
  if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,sizeof(opt)) < 0 )
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  //type of socket created
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( port );

  //bind the socket to port
  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  printf("Listener on port %d \n", port);

  //try to specify maximum of 5 pending connections for the master socket
  if (listen(master_socket, 5) < 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  //accept the incoming connection
  addrlen = sizeof(address);
  puts("Waiting for connections ...");

  while(1)
  {
    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(master_socket, &readfds);
    max_sd = master_socket;

    //add child sockets to set
    for ( i = 0 ; i < max_clients ; i++)
    {
      //socket descriptor
      sd = client_socket[i];

      //if valid socket descriptor then add to read list
      if(sd > 0)
      FD_SET( sd , &readfds);

      //highest file descriptor number, need it for the select function
      if(sd > max_sd)
      max_sd = sd;
    }

    //wait for an activity on one of the sockets , timeout is NULL ,
    //so wait indefinitely
    activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

    if ((activity < 0) && (errno!=EINTR))
    {
      printf("select error\n");
    }

    //If something happened on the master socket ,
    //then its an incoming connection
    if (FD_ISSET(master_socket, &readfds))
    {
      if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
      {
        perror("accept");
        exit(EXIT_FAILURE);
      }

      //inform user of socket number - used in send and receive commands
      printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
      char *message = "Please login\n";

      //send new connection greeting message
      if( send(new_socket, message, strlen(message), 0) != strlen(message) )
      {
        perror("send");
      }

      puts("Welcome message sent successfully");

      //add new socket to array of sockets
      for (i = 0; i < max_clients; i++)
      {
        //if position is empty
        if( client_socket[i] == 0 )
        {
          client_socket[i] = new_socket;
          printf("Adding to list of sockets as %d\n" , i);
          break;
        }
      }
    }

    //else its some IO operation on some other socket
    for (i = 0; i < max_clients; i++)
    {
      sd = client_socket[i];
      if (FD_ISSET( sd , &readfds))
      {
        //Check if it was for closing , and also read the
        //incoming message
        if ((valread = read( sd , buffer, 1024)) == 0)
        {
          //Somebody disconnected , get his details and print
          getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
          printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

          //Close the socket and mark as 0 in list for reuse
          close( sd );
          client_socket[i] = 0;
          login[i]=-1;
        }
        else
        {
          if(login[i]!=-1)       // logged in
          {
            int t_id=login[i];
            valread=read(sd,buffer,1024);
            if(strcmp(buffer,AVAILABLE)==0)      // ................not sure if this strcmp works or not
            {
              char *t;
              char *resp=available(t);
              send(sd,resp,strlen(resp),0);
            }
            else if(strcmp(buffer,BUY_REQUEST)==0)
            {
              char *mess="Send details for buying\n";
              send(sd,mess,strlen(mess),0);
              int item,trader,price,quantity;
              valread=read(sd,buffer,1024);
              item=atoi(buffer);
              trader=t_id;
              valread=read(sd,buffer,1024);
              price=atoi(buffer);
              valread=read(sd,buffer,1024);
              quantity=atoi(buffer);
              char *resp=serve_buy_request(item,trader,price,quantity);
              send(sd,resp,strlen(resp),0);
            }
            else if(strcmp(buffer,SELL_REQUEST)==0)
            {
              char *mess="Send details for selling\n";
              send(sd,mess,strlen(mess),0);
              int item,trader,price,quantity;
              valread=read(sd,buffer,1024);
              item=atoi(buffer);
              trader=t_id;
              valread=read(sd,buffer,1024);
              price=atoi(buffer);
              valread=read(sd,buffer,1024);
              quantity=atoi(buffer);
              char *resp=serve_sell_request(item,trader,price,quantity);
              send(sd,resp,strlen(resp),0);
            }
            else if(strcmp(buffer,VIEW_ORDER_STATUS)==0)
            {
              char *resp=view_order();
              send(sd,resp,strlen(resp),0);
            }
            else if(strcmp(buffer,VIEW_TRADE_STATUS)==0)
            {
              char *resp=trade_status(t_id);
              send(sd,resp,strlen(resp),0);
            }
          }
          else         // login
          {
            char id[100],pass[100];
            valread=read(sd,pass,1024);
            if(search(buffer,pass))
            {
              int t_id=0;
              int j=0;
              while(buffer[j]!=' ' || buffer[j]!='\0')  //.............................. may be errenous as maybe buffer is not null terminated
              {
                id[j++]=buffer[j++];
              }
              id[j]='\0';
              // for(int l=j-1;l>=0;l--)
              // {
              //   t_id+=(int)(id[l])*pow(10,l);
              // }
              t_id=atoi(id);
              login[i]=t_id;
              char *mess="Y";
              send(sd , mess , strlen(mess) , 0 );
            }
            else
            {
              char *mess="N";
              send(sd , mess , strlen(mess) , 0 );
            }
          }
        }
      }
    }
  }
}
