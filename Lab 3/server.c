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
//.....................................
//...................................
//........................................
//.......................................
//.....................................
//...........................................
//........................................
//Change input method to file based rather than command based
//Trade_id may be string, not clearly mentioned in problem
// ALL THIS MAY STILL NOT WORK WHILE TAKING INPUT FROM FILE AS THE SPEED OF INPUT WILL BE VERY HIGH
// MAY USE SLEEP FUNCTION FOR THIS
// MAKE SERVER CODE ROBUST, CURRENTLY IT STOPS IF WRONG INPUT FROM CLIENT,
// IT SHOULD NEVER STOP, SEE HOW TO SUPPRESS ERRORS ................................
//...................................................
//...........VERY VERY IMPORTANT...............
//...........................................
//...................................................
//ALSO TRY TO ENSURE THAT IF SERVER GOES DOWN CLIENT SHOULD BE INFORMED AND CLOSED
//SHOULD KEEP DATA IN FILE AS IF SERVER GOES DOWN FOR ANY REASON, ALL DATA IS LOST
//.....................................................
//TAKE CARE TO TRIM THE STRINGS NEEDED TO BE SENT OR RECEIVED IN SPECIAL FORMAT
//.....................................................
// void send_packet(int sd,char buffer[],int k)
// {
//   char temp[1000];
//   int l=strlen(buffer);
//   char len[10];
//   snprintf(len,1000,"%05d",l);
//   strcpy(temp,len);
//   strcat(temp,buffer);
//   write(sd,temp,strlen(temp));
//   printf("$$$$$%s\n",temp);
// }
void available(int sd)
{
  // printf("hello\n");
  char buffer[1000];
  bzero(buffer,1001);
  snprintf(buffer,1000,"These items are available for buying\n");
  write(sd,buffer,strlen(buffer));
  bool flag = false;
  for(int i = 1; i  < 11; i++)
  {
    if(sell[i] && sell[i]->start!=NULL)
    {
      //printf("%d sell\n",i);
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d    ", i);
      write(sd,buffer,strlen(buffer));
      flag = true;
    }
  }
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
  if(!flag)
  {
    bzero(buffer,1001);
    snprintf(buffer,1000,"<-- No items available -->\n");
    write(sd,buffer,strlen(buffer));
    //for(int i= 0 ; i < 30000; i++){}
    // write(sd,buffer,strlen(buffer);
  }
  bzero(buffer,1001);
  snprintf(buffer,1000,"\nOn sending a buy/sell request for any of the items not listed in the respective fields, your request will be stored and served asap\n");
  write(sd,buffer,strlen(buffer));
  // write(sd,buffer,strlen(buffer);
 // printf("%s\n",buffer);
  //return buffer;
  // bzero(buffer,1001);
  // snprintf(buffer,1000,".");
  write(sd,"^",1);
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
      push(trans, item, sell[item]->start->Trader_Id, trader, sell[item]->start->Price, quantity);
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d number of Item %d bought from %d at Rs. %d\n", quantity, item, sell[item]->start->Trader_Id, price);
      write(sd,buffer,strlen(buffer));
      quantity = 0;
    }
    else
    {
      quantity = quantity - sell[item]->start->count;
      push(trans, item, sell[item]->start->Trader_Id , trader, sell[item]->start->Price, sell[item]->start->count);
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d number of Item %d bought from %d at Rs. %d\n", sell[item]->start->count, item, sell[item]->start->Trader_Id, price);
      write(sd,buffer,strlen(buffer));
      sell[item]->start->count = 0;
      pop(sell[item]);
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
    // printf("bef push\n");
    pushb(buy[item], item, trader, price, quantity);
    // printf("aft push\n");
  }
  write(sd,"^",1);
  //return buffer;
}

void serve_sell_request(int sd,int item, int trader, int price, int quantity)
{
  char buffer[1000];
  bool flag=false;
  while (buy[item]->start != NULL && buy[item]->start->Price >= price && quantity > 0)
  {
    flag=true;
    if(buy[item]->start->count > quantity)
    {
      buy[item]->start->count = buy[item]->start->count - quantity;
      push(trans, item, trader,  buy[item]->start->Trader_Id, buy[item]->start->Price, quantity);
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d number of Item %d sold to %d at Rs. %d\n", quantity, item, buy[item]->start->Trader_Id, price);
      write(sd,buffer,strlen(buffer));
      quantity = 0;
    }
    else
    {
      quantity = quantity - buy[item]->start->count;
      push(trans, item, trader, buy[item]->start->Trader_Id , sell[item]->start->Price, sell[item]->start->count);
      bzero(buffer,1001);
      snprintf(buffer,1000,"%d number of Item %d sold to %d at Rs. %d\n", buy[item]->start->count, item, buy[item]->start->Trader_Id, price);
      write(sd,buffer,strlen(buffer));
      buy[item]->start->count = 0;
      pop(buy[item]);
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
    pushs(sell[item], item, trader, price, quantity);
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
    snprintf(buffer,1000,"ItemID: %d     ",  i);
    write(sd,buffer,strlen(buffer));
    if(buy[i]->start != NULL)
    {
      bzero(buffer,1001);
      snprintf(buffer,1000,"Best Buy: %d     ",  buy[i]->start->Price );
      write(sd,buffer,strlen(buffer));
    }
    else
    {
      bzero(buffer,1001);
      snprintf(buffer,1000,"<-- No buyers -->");
      write(sd,buffer,strlen(buffer));
    }
    if(sell[i]->start!=NULL )
    {
      bzero(buffer,1001);
      snprintf(buffer,1000,"Best Sell: %d \n", sell[i]->start->Price);
      write(sd,buffer,strlen(buffer));
    }
    else
    {
      bzero(buffer,1001);
      snprintf(buffer,1000,"<-- No sellers --> \n");
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

bool search(char buffer[])      // ...............change this function
{
  //return true;
  //printf("%s .... %s\n",id,pass);
  // int l=strlen(id);
  // id[l]=' ';
  // l++;
  // int m=0;
  // while(pass[m]!='\0')
  // {
  //   id[l+m]=pass[m];
  //   m++;
  // }
  // id[l+m]='\0';
  // // id[l+m+1]='\0';
  // printf("%s\n",id);
  strcat(buffer,"\n");
  FILE * fp=fopen("auth.txt","r");
  if(!fp)
  {
    return false;  // ..............can write file not available if needed for closure in error handling
  }
  char temp[1024];
  bool flag=false;
  // printf("buff-0000-%s",buffer);
  while(fgets(temp, INT_MAX, fp) != NULL)
  {
    // int len=strlen(temp);
    //temp[len-]='\0';
     // printf("temp+0000+%s",temp);
    // char a[100],b[100];
    // int j=0,k=0;
    // while(temp[j]!=' ')
    // {
    //   a[j++]=temp[j++];
    // }
    // a[j]='\0';
    // j++;
    // while(temp[j]!='\n')
    // {
    //   b[k++]=temp[j++];
    // }
    // b[k]='\0';
    // printf("%s +++ %s \n",a,b);
    if(strcmp(temp, buffer) == 0)
    {
      //if(strcmp(b, pass) == 0)  //............may be wrong not sure about strtok
      //{
        flag=true;
      //}
      break;
    }
  }
  fclose(fp);
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
  trans=create_tqueue();
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
          buffer[valread]='\0';
          // printf("%s buff\n",buffer);
          if(login[i]!=-1)       // logged in
          {
            int t_id=login[i];
            // printf("%d tid\n",t_id);
            // valread=read(sd,buffer,1024);
            // buffer[valread]='\0';
            // printf("%s\n",buffer);
            if(strcmp(buffer,AVAILABLE)==0)      // ................not sure if this strcmp works or not
            {
              //printf("%s.,.,.,\n",buffer);
              // char *t;
              available(sd);
              //send(sd,resp,strlen(resp),0);
            }
            else if(strcmp(buffer,BUY_REQUEST)==0)
            {
              char *mess="Send details for buying\n";
              write(sd,mess,strlen(mess));
              int item,trader,price,quantity;
              trader=t_id;
              valread=read(sd,buffer,1000);
              buffer[valread]='\0';
              // printf("buffer is %s\n",buffer);
              int j=0,k=0,l=0;
              char t1[100],t2[100],t3[100];
              while(buffer[j]!='#')
              {
                t1[j]=buffer[j];
                j++;
              }
              t1[j]='\0';
              // printf("item is %s\n",t1);
              item=atoi(t1);
              j++;
              while(buffer[j]!='#')
              {
                t2[k]=buffer[j];
                k++;
                j++;
              }
              t2[k]='\0';
              // printf("price is %s\n",t2);
              price=atoi(t2);
              j++;
              while(buffer[j]!='#')
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
            else if(strcmp(buffer,SELL_REQUEST)==0)
            {
              char *mess="Send details for selling\n";
              write(sd,mess,strlen(mess));
              int item,trader,price,quantity;
              trader=t_id;
              valread=read(sd,buffer,1024);
              buffer[valread]='\0';
              int j=0,k=0,l=0;
              char t1[100],t2[100],t3[100];
              while(buffer[j]!='#')
              {
                t1[j]=buffer[j];
                j++;
              }
              t1[j]='\0';
              item=atoi(t1);
              j++;
              while(buffer[j]!='#')
              {
                t2[k]=buffer[j];
                k++;
                j++;
              }
              t2[k]='\0';
              price=atoi(t2);
              j++;
              while(buffer[j]!='#')
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
              // int j=0;
              // while(buffer[j]!=' ' && buffer[j]!='\0')  //.............................. may be errenous as maybe buffer is not null terminated
              // {
              //   id[j++]=buffer[j++];
              // }
              // id[j]='\0';
              //printf("%s\n",buffer);
              // for(int l=j-1;l>=0;l--)
              // {
              //   t_id+=(int)(id[l])*pow(10,l);
              // }
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
