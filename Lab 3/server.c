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
#define AVAILABLE "1"
#define BUY_REQUEST "2"
#define SELL_REQUEST "3"
#define VIEW_ORDER_STATUS "4"
#define VIEW_TRADE_STATUS "5"
queue * sell[11];
queue * buy[11];
tqueue * trans;

void available(int sd)
{
  char buffer[1000];
  bzero(buffer,1001);
  snprintf(buffer,1000,"These items are available for buying\n");
  write(sd,buffer,strlen(buffer));
  bool flag = false;
  for(int i = 1; i  < 11; i++)
  {
    if(sell[i] && sell[i]->start!=NULL)
    {
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d    ", i);
      write(sd,buffer,strlen(buffer));
      flag = true;
    }
  }
  if(flag)
    snprintf(buffer,1000,"\n");
  if(!flag)
  {

    bzero(buffer,1001);
    snprintf(buffer,1000,"<-- No items available -->\n");
    write(sd,buffer,strlen(buffer));
    // write(sd,buffer,strlen(buffer);
  }
  flag  = false;
  bzero(buffer,1001);
  snprintf(buffer,1000,"These items are available for selling\n");
  write(sd,buffer,strlen(buffer));
  // write(sd,buffer,strlen(buffer);
  for(int i = 1; i  < 11; i++)
  {
    if(buy[i] && buy[i]->start!=NULL)
    {
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d     ", i);
      write(sd,buffer,strlen(buffer));
      flag = true;
    }
  }
  if(flag)
    snprintf(buffer,1000,"\n");
  if(!flag)
  {
    bzero(buffer,1001);
    snprintf(buffer,1000,"<-- No items available -->\n");
    write(sd,buffer,strlen(buffer));
    //for(int i= 0 ; i < 30000; i++){}
    // write(sd,buffer,strlen(buffer);
  }
  bzero(buffer,1001);
  snprintf(buffer,1000,"\nOn sending a buy/sell request for any of the items not listed in the respective fields, your request will be stored and served asap^\n");
  write(sd,buffer,strlen(buffer));
  // write(sd,buffer,strlen(buffer);
 // printf("%s\n",buffer);
  //return buffer;
  // bzero(buffer,1001);
  // snprintf(buffer,1000,".");
  // write(sd,"^",1);
}

void serve_buy_request(int sd,int item, int trader, int price, int quantity)
{
  char buffer[1000];
  bool flag=false;
  // if(sell[item] && sell[item]->start)
  // {
  //   printf("exists\n");
  // }
  while (sell[item] && sell[item]->start != NULL && sell[item]->start->Price <= price && quantity > 0)
  {
    flag=true;
    if(sell[item]->start->count > quantity)
    {
      sell[item]->start->count = sell[item]->start->count - quantity;
      char f[100];
      snprintf(f,100,"s%d.txt",item);
      FILE *fp=fopen(f,"w");
      fprintf(fp,"Item Seller Price Quantity\n");
      node *point = sell[item]->start;
      while(point!=NULL){

              fprintf(fp,"%02d %02d %04d %04d\n", point->Item_Id, point->Trader_Id, point->Price, point->count  );
              point = point->next;
      }
      fclose(fp);
      push(trans, item, sell[item]->start->Trader_Id, trader, sell[item]->start->Price, quantity);
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d number of Item %d bought from %d at Rs. %d\n", quantity, item, sell[item]->start->Trader_Id, sell[item]->start->Price);
      write(sd,buffer,strlen(buffer));
      quantity = 0;
    }
    else
    {
      quantity = quantity - sell[item]->start->count;
      push(trans, item, sell[item]->start->Trader_Id , trader, sell[item]->start->Price, sell[item]->start->count);
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d number of Item %d bought from %d at Rs. %d\n", sell[item]->start->count, item, sell[item]->start->Trader_Id, sell[item]->start->Price);
      write(sd,buffer,strlen(buffer));
      sell[item]->start->count = 0;
      pops(sell[item]);
    }
  }
  if(flag==false)
  {
    bzero(buffer,1001);
    snprintf(buffer,1000,"No current seller, your request will be stored and served asap \n");
    write(sd,buffer,strlen(buffer));
  }
  if(quantity > 0)
  {
    // printf("bef buy push\n");
    pushb(buy[item], item, trader, price, quantity);
    // printf("aft buy push\n");
  }
  write(sd,"^",1);
  //return buffer;
}

void serve_sell_request(int sd,int item, int trader, int price, int quantity)
{
  char buffer[1000];
  bool flag=false;
  while (buy[item] && buy[item]->start != NULL && buy[item]->start->Price >= price && quantity > 0)
  {
    flag=true;
    if(buy[item]->start->count > quantity)
    {
      buy[item]->start->count = buy[item]->start->count - quantity;
      char f[100];
      snprintf(f,100,"b%d.txt",item);
      FILE *fp=fopen(f,"w");
      fprintf(fp,"Item Seller Price Quantity\n");
      node *point = buy[item]->start;
      while(point!=NULL){

              fprintf(fp,"%02d %02d %04d %04d\n", point->Item_Id, point->Trader_Id, point->Price, point->count  );
              point = point->next;
      }
      fclose(fp);
      push(trans, item, trader,  buy[item]->start->Trader_Id, buy[item]->start->Price, quantity);
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d number of Item %d sold to %d at Rs. %d\n", quantity, item, buy[item]->start->Trader_Id, buy[item]->start->Price);
      write(sd,buffer,strlen(buffer));
      quantity = 0;
    }
    else
    {
      quantity = quantity - buy[item]->start->count;
      push(trans, item, trader, buy[item]->start->Trader_Id , buy[item]->start->Price, buy[item]->start->count);
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d number of Item %d sold to %d at Rs. %d\n", buy[item]->start->count, item, buy[item]->start->Trader_Id, buy[item]->start->Price);
      write(sd,buffer,strlen(buffer));
      buy[item]->start->count = 0;
      popb(buy[item]);
    }
  }
  if(flag==false)
  {
    bzero(buffer,1001);
    snprintf(buffer,1000,"No current buyer, your request will be stored and served asap \n");
    write(sd,buffer,strlen(buffer));
  }
  if(quantity > 0)
  {
    // printf("bef sell push\n");
    pushs(sell[item], item, trader, price, quantity);
    // printf("aft sell push\n");
  }
  write(sd,"^",1);
//  return buffer;
}

void view_order(int sd)
{
  char buffer[1000];
  for(int i = 1; i < 11; i++)
  {
    bzero(buffer,1001);
    snprintf(buffer,1000,"ItemID: %02d     ",  i);
    write(sd,buffer,strlen(buffer));
    if(sell[i]->start != NULL)
    {
      bzero(buffer,1001);
      snprintf(buffer,1000,"Best Buy: %d     ",  sell[i]->start->Price );
      write(sd,buffer,strlen(buffer));
    }
    else
    {
      bzero(buffer,1001);
      snprintf(buffer,1000,"<-- No buyers -->  ");
      write(sd,buffer,strlen(buffer));
    }
    if(buy[i]->start!=NULL )
    {
      bzero(buffer,1001);
      snprintf(buffer,1000,"Best Sell: %d \n", buy[i]->start->Price);
      write(sd,buffer,strlen(buffer));
    }
    else
    {
      bzero(buffer,1001);
      snprintf(buffer,1000,"<-- No sellers -->\n");
      write(sd,buffer,strlen(buffer));
    }
  }
  write(sd,"^",1);
  // return buffer;
}

void trade_status(int sd,int trader)
{
  char buffer[1000];
  bzero(buffer,1001);
  snprintf(buffer,1000,"Settled Transactions: \n");
  write(sd,buffer,strlen(buffer));
  tnode * temp = trans->start;
  while(temp != NULL)
  {
    if(temp->buyer == trader)
    {
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d number of Item %d bought from %d at Rs. %d\n", temp->quantity, temp->item, temp->seller, temp->price);
      write(sd,buffer,strlen(buffer));
    }
    if(temp->seller == trader)
    {
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d number of Item %d sold to %d\n at Rs. %d\n", temp->quantity, temp->item, temp->buyer, temp->price);
      write(sd,buffer,strlen(buffer));
    }
    temp = temp->next;
  }
  bzero(buffer,1001);
  snprintf(buffer,1000,"\nPending Transactions: \n");
  write(sd,buffer,strlen(buffer));
  for(int i = 1; i < 11; i++)
  {
    node * temp = buy[i]->start;
    while(temp!=NULL)
    {
      if(temp->Trader_Id == trader)
      {
        bzero(buffer,1001);
        snprintf(buffer,1000,"%d number of Item %d are queued for buying at price Rs. %d\n", temp->count, temp->Item_Id, temp->Price);
        write(sd,buffer,strlen(buffer));
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
        bzero(buffer,1001);
        snprintf(buffer,1000,"%d number of Item %d are queued for selling at price %d\n", temp->count, temp->Item_Id, temp->Price);
        write(sd,buffer,strlen(buffer));
      }
      temp = temp->next;
    }
  }
  write(sd,"^",1);
  // return buffer;
}

// searching in auth.txt file
bool search(char buffer[])
{
  strcat(buffer,"\n");
  FILE * fp=fopen("auth.txt","r");
  if(!fp)
  {
    return false;
  }
  char temp[1024];
  bool flag=false;
  while(fgets(temp, INT_MAX, fp) != NULL)
  {
    if(strcmp(temp, buffer) == 0)
    {
      flag=true;
      break;
    }
  }
  fclose(fp);
  return flag;
}

int main(int argc, char *argv[])
{
  sell[0] = NULL;
  buy[0] = NULL;
  for(int i = 1 ; i <11; i++)
  {
    sell[i] = create_queue();
    char f[100];
    snprintf(f,100,"s%d.txt",i);
    FILE *fp=fopen(f,"r");
    // For recovering if server crashes
    if(fp)
    {
      char temp[1024];
      if(fgets(temp, INT_MAX, fp))
      {
        // printf("%s",temp);
        while(fgets(temp, INT_MAX, fp) != NULL)
        {
          // printf("%s",temp);
          if(temp[0]>='0' && temp[0]<='9')
          {
            int item,trader,price,quantity;
            char t1[100],t2[100],t3[100],t4[100];
            int j=0,k=0,l=0,m=0;
            while(temp[j]>='0' && temp[j]<='9')
            {
              t1[j]=temp[j];
              j++;
            }
            t1[j]='\0';
            item=atoi(t1);
            j++;
            while(temp[j]>='0' && temp[j]<='9')
            {
              t2[k]=temp[j];
              k++;
              j++;
            }
            t2[k]='\0';
            trader=atoi(t2);
            j++;
            while(temp[j]>='0' && temp[j]<='9')
            {
              t3[l]=temp[j];
              l++;
              j++;
            }
            t3[l]='\0';
            price=atoi(t3);
            j++;
            while(temp[j]>='0' && temp[j]<='9')
            {
              t4[m]=temp[j];
              m++;
              j++;
            }
            t4[m]='\0';
            quantity=atoi(t4);
            queue *thelist=sell[i];
            // pushs
            node * file = create_node(item, trader, price, quantity);
            node *point = thelist->start;
            if(point == NULL)
            {
              thelist->start = file;
            }
            else if(point->Price > price)
            {
              thelist->start = file;
              file->next = point;
            }
            else
            {
              while(point->next!=NULL && point->next->Price < price)
              {
                point = point->next;
              }
              node * temp = point->next;
              point->next = file;
              file->next = temp;
            }
          }
        }
      }
      fclose(fp);
    }
    buy[i] = create_queue();
    char g[100];
    snprintf(g,100,"b%d.txt",i);
    fp=fopen(g,"r");
    if(fp)
    {
      char temp[1024];
      if(fgets(temp, INT_MAX, fp))
      {
        // printf("%s",temp);
        while(fgets(temp, INT_MAX, fp) != NULL)
        {
          // printf("%s",temp);
          if(temp[0]>='0' && temp[0]<='9')
          {
            int item,trader,price,quantity;
            char t1[100],t2[100],t3[100],t4[100];
            int j=0,k=0,l=0,m=0;
            while(temp[j]>='0' && temp[j]<='9')
            {
              t1[j]=temp[j];
              j++;
            }
            t1[j]='\0';
            item=atoi(t1);
            j++;
            while(temp[j]>='0' && temp[j]<='9')
            {
              t2[k]=temp[j];
              k++;
              j++;
            }
            t2[k]='\0';
            trader=atoi(t2);
            j++;
            while(temp[j]>='0' && temp[j]<='9')
            {
              t3[l]=temp[j];
              l++;
              j++;
            }
            t3[l]='\0';
            price=atoi(t3);
            j++;
            while(temp[j]>='0' && temp[j]<='9')
            {
              t4[m]=temp[j];
              m++;
              j++;
            }
            t4[m]='\0';
            quantity=atoi(t4);
            queue *thelist=buy[i];
            // pushb
            node * file = create_node(item, trader, price, quantity);
            node *point = thelist->start;
            if(point == NULL)
            {
              thelist->start = file;
            }
            else if(point->Price > price)
            {
              thelist->start = file;
              file->next = point;
            }
            else
            {
              while(point->next!=NULL && point->next->Price < price)
              {
                point = point->next;
              }
              node * temp = point->next;
              point->next = file;
              file->next = temp;
            }
          }
        }
      }
      fclose(fp);
    }
  }

  trans=create_tqueue();
  if(argc<2)
  {
    printf("Please enter server port number\n");
    return 0;
  }
  //printf("%s",argv[1]);
  int port=atoi(argv[1]);
  // printf("%d\n",port);
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
  //set master socket to allow multiple connections
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
  listen(master_socket,5);
  // if (listen(master_socket, 5) < 0)
  // {
  //   perror("listen");
  //   exit(EXIT_FAILURE);
  // }

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
          buffer[valread]='\0';
          // printf("%s buff\n",buffer);
          if(login[i]!=-1)       // logged in
          {
            int t_id=login[i];

            if(strcmp(buffer,AVAILABLE)==0)
            {
              available(sd);
              //send(sd,resp,strlen(resp),0);
            }
            else if(buffer[0]=='2')
            {
              int item,trader,price,quantity;
              trader=t_id;
              int j=0,m=0,k=0,l=0;
              while(buffer[j]>='0' && buffer[j]<='9')
              {
                j++;
              }
              while(buffer[j]<'0' || buffer[j]>'9') j++;
              char t1[100],t2[100],t3[100];
              while(buffer[j]>='0' && buffer[j]<='9')
              {
                t1[m]=buffer[j];
                m++;
                j++;
              }
              t1[m]='\0';
              // printf("item is %s\n",t1);
              item=atoi(t1);
              while(buffer[j]<'0' || buffer[j]>'9') j++;
              while(buffer[j]>='0' && buffer[j]<='9')
              {
                t2[k]=buffer[j];
                k++;
                j++;
              }
              t2[k]='\0';
              // printf("price is %s\n",t2);
              price=atoi(t2);
              while(buffer[j]<'0' || buffer[j]>'9') j++;
              while(buffer[j]>='0' && buffer[j]<='9')
              {
                t3[l]=buffer[j];
                l++;
                j++;
              }
              t3[l]='\0';
              // printf("quantity is %s\n",t3);
              quantity=atoi(t3);
              // printf("buy det %d ,, %d ,, %d ,, %d\n",item,trader,price,quantity);
              serve_buy_request(sd,item,trader,price,quantity);
              //send(sd,resp,strlen(resp),0);
            }
            else if(buffer[0]=='3')
            {
              int item,trader,price,quantity;
              trader=t_id;
              int j=0,m=0,k=0,l=0;
              while(buffer[j]>='0' && buffer[j]<='9')
              {
                j++;
              }
              while(buffer[j]<'0' || buffer[j]>'9') j++;
              char t1[100],t2[100],t3[100];
              while(buffer[j]>='0' && buffer[j]<='9')
              {
                t1[m]=buffer[j];
                m++;
                j++;
              }
              t1[m]='\0';
              item=atoi(t1);
              while(buffer[j]<'0' || buffer[j]>'9') j++;
              while(buffer[j]>='0' && buffer[j]<='9')
              {
                t2[k]=buffer[j];
                k++;
                j++;
              }
              t2[k]='\0';
              price=atoi(t2);
              while(buffer[j]<'0' || buffer[j]>'9') j++;
              while(buffer[j]>='0' && buffer[j]<='9')
              {
                t3[l]=buffer[j];
                l++;
                j++;
              }
              t3[l]='\0';
              quantity=atoi(t3);
              // printf("sell det %d ,, %d ,, %d ,, %d\n",item,trader,price,quantity);
              serve_sell_request(sd,item,trader,price,quantity);
              //send(sd,resp,strlen(resp),0);
            }
            else if(strcmp(buffer,VIEW_ORDER_STATUS)==0)
            {
              view_order(sd);
              //send(sd,resp,strlen(resp),0);
            }
            else if(strcmp(buffer,VIEW_TRADE_STATUS)==0)
            {
              trade_status(sd,t_id);
              //send(sd,resp,strlen(resp),0);
            }
          }
          else         // login
          {
            // printf("login module\n");
            // char id[100],pass[100];
            // valread=read(sd,pass,1024);
            // pass[valread]='\0';
            //printf("%s\n",pass);
            if(search(buffer))  // buffer is of the form 1 abc
            {
              int t_id;
              t_id=atoi(buffer);
              //printf("%d\n",t_id);
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
