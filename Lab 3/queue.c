#include<stdio.h>
#include<stdlib.h>
//#include "queue.h"

typedef struct node{

        int Item_Id;
        int Trader_Id;
        int Price;
        int count;
        struct node * next;

}node;

typedef struct queue{

        node * start;

}queue;


queue * create_queue(){

        queue * thelist;
        thelist = (queue *)malloc(sizeof(queue));
        thelist->start = NULL;
        return thelist;
}

node * create_node(int item, int trader, int price, int count){

        node * thenode;
        thenode = (node *)malloc(sizeof(node));
        thenode->Item_Id =item;
        thenode->Trader_Id =trader;
        thenode->Price =price;
        thenode->count = count;
        thenode->next = NULL;
        return thenode;

}

void pushs(queue * thelist, int item,int  trader,  int price, int count){
if(!thelist) return;
	node * file = create_node(item, trader, price, count);

        node *point = thelist->start;
        if(point == NULL){

                thelist->start = file;
                char f[100];
                snprintf(f,100,"s%d.txt",item);
                FILE *fp=fopen(f,"w");
                fprintf(fp,"Item Seller Price Quantity\n");
                point = thelist->start;
                while(point!=NULL){

                        fprintf(fp,"%02d %02d %04d %04d\n", point->Item_Id, point->Trader_Id, point->Price, point->count  );
                        point = point->next;
                }
                fclose(fp);
                return;
        }
        if(point->Price > price){
        	thelist->start = file;
        	file->next = point;
          char f[100];
          snprintf(f,100,"s%d.txt",item);
          FILE *fp=fopen(f,"w");
          fprintf(fp,"Item Seller Price Quantity\n");
          point = thelist->start;
          while(point!=NULL){

                  fprintf(fp,"%02d %02d %04d %04d\n", point->Item_Id, point->Trader_Id, point->Price, point->count  );
                  point = point->next;
          }
          fclose(fp);
        	return;
        }

        while(point->next!=NULL && point->next->Price < price){

                point = point->next;
        }
        node * temp = point->next;
        point->next = file;
        file->next = temp;
        char f[100];
        snprintf(f,100,"s%d.txt",item);
        FILE *fp=fopen(f,"w");
        fprintf(fp,"Item Seller Price Quantity\n");
        point = thelist->start;
        while(point!=NULL){

                fprintf(fp,"%02d %02d %04d %04d\n", point->Item_Id, point->Trader_Id, point->Price, point->count  );
                point = point->next;
        }
        fclose(fp);
	return;
}

void pushb(queue * thelist, int item, int trader,  int price, int count){
if(!thelist) return;
	node * file = create_node(item, trader, price, count);

        node *point = thelist->start;
        if(point == NULL){

                thelist->start = file;
                char f[100];
                snprintf(f,100,"b%d.txt",item);
                FILE *fp=fopen(f,"w");
                fprintf(fp,"Item Buyer Price Quantity\n");
                point = thelist->start;
                while(point!=NULL){

                        fprintf(fp,"%02d %02d %04d %04d\n", point->Item_Id, point->Trader_Id, point->Price, point->count  );
                        point = point->next;
                }
                fclose(fp);
                return;
        }
        if(point->Price < price){

        	thelist->start = file;
        	file->next = point;
          char f[100];
          snprintf(f,100,"b%d.txt",item);
          FILE *fp=fopen(f,"w");
          fprintf(fp,"Item Buyer Price Quantity\n");
          point = thelist->start;
          while(point!=NULL){

                  fprintf(fp,"%02d %02d %04d %04d\n", point->Item_Id, point->Trader_Id, point->Price, point->count  );
                  point = point->next;
          }
          fclose(fp);
        	return;
        }

        while(point->next != NULL && point->next->Price > price){

                point = point->next;
        }
        node * temp = point->next;
        point->next = file;
        file->next = temp;
        char f[100];
        snprintf(f,100,"b%d.txt",item);
        FILE *fp=fopen(f,"w");
        fprintf(fp,"Item Buyer Price Quantity\n");
        point = thelist->start;
        while(point!=NULL){

                fprintf(fp,"%02d %02d %04d %04d\n", point->Item_Id, point->Trader_Id, point->Price, point->count  );
                point = point->next;
        }
        fclose(fp);
        return;

}

void pops(queue * thelist){

	if(!thelist||thelist->start == NULL) return;
	node * temp = thelist->start;
  char f[100];
  snprintf(f,100,"s%d.txt",thelist->start->Item_Id);
 	thelist->start = thelist->start->next;
 	free(temp);
  if(!thelist||thelist->start == NULL)
  {

    FILE *fp=fopen(f,"w");
    fprintf(fp,"Item Seller Price Quantity\n");
    fclose(fp);
    return;
  }
  FILE *fp=fopen(f,"w");
  fprintf(fp,"Item Seller Price Quantity\n");
  node *point = thelist->start;
  while(point!=NULL){

          fprintf(fp,"%02d %02d %04d %04d\n", point->Item_Id, point->Trader_Id, point->Price, point->count  );
          point = point->next;
  }
  fclose(fp);
}

void popb(queue * thelist){

	if(!thelist||thelist->start == NULL) return;
	node * temp = thelist->start;
  char f[100];
  snprintf(f,100,"b%d.txt",thelist->start->Item_Id);
 	thelist->start = thelist->start->next;
 	free(temp);
  if(!thelist||thelist->start == NULL)
  {
    FILE *fp=fopen(f,"w");
    fprintf(fp,"Item Buyer Price Quantity\n");
    fclose(fp);
    return;
  }
  FILE *fp=fopen(f,"w");
  fprintf(fp,"Item Buyer Price Quantity\n");
  node *point = thelist->start;
  while(point!=NULL){

          fprintf(fp,"% 02d % 02d % 04d % 04d\n", point->Item_Id, point->Trader_Id, point->Price, point->count  );
          point = point->next;
  }
  fclose(fp);
}

void print_queue(queue * thelist){
        if(!thelist) return;
        node * point = thelist->start;
        while(point!=NULL){

                printf("Item: %d Trader: %d  Price: %d  Count: %d \n ", point->Item_Id, point->Trader_Id, point->Price, point->count  );
                point = point->next;
        }
        printf("\n");

}
