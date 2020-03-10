#include<stdio.h>
#include<stdlib.h>
//#include "transactions.h"

typedef struct tnode{

        int buyer;
        int seller;
        int item;
        int quantity;
        int price;
        struct tnode * next;

}tnode;

typedef struct tqueue{

        tnode * start;

}tqueue;


tqueue * create_tqueue(){

        tqueue * thelist;
        thelist = (tqueue *)malloc(sizeof(tqueue));
        thelist->start = NULL;
        return thelist;
}

tnode * create_tnode(int item, int buyer, int seller,  int price, int count){

        tnode * thenode;
        thenode = (tnode *)malloc(sizeof(tnode));
        thenode->item =item;
        thenode->buyer =buyer;
        thenode->seller =seller;
        thenode->price =price;
        thenode->quantity = count;
        thenode->next = NULL;
        return thenode;

}

void push(tqueue * thelist, int item,int  seller, int buyer,   int price, int count){

	tnode * file = create_tnode(item, buyer, seller, price, count);
  FILE *fp=fopen("transac.txt","a");
  fprintf(fp,"%02d %02d %02d %04d %04d\n",item,buyer,seller,price,count);
  fclose(fp);
        tnode * point = thelist->start;
        if(point == NULL){

                thelist->start = file;
                return;
        }
        // if(point->price > price){
        // 	thelist->start = file;
        // 	file->next = point;
        // 	return;
        // }

        while(point->next!=NULL){

                point = point->next;
        }
        tnode * temp = point->next;
        point->next = file;
        file->next = temp;
	return;
}



void popt(tqueue * thelist){

	if(thelist->start == NULL) return;
	tnode * temp = thelist->start;
 	thelist->start = thelist->start->next;
 	free(temp);

}

void print_tqueue(tqueue * thelist){

        tnode * point = thelist->start;
        while(point!=NULL){

                printf("Item: %d Buyer: %d Seller: %d   Price: %d  Count: %d \n ", point->item, point->buyer, point->seller,  point->price, point->quantity  );
                point = point->next;
        }
        printf("\n");

}
