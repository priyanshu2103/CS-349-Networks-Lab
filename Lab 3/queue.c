#include<stdio.h>
#include<stdlib.h>
#include "queue.h"

typedef struct _node{

        int Item_Id;
        int Trader_Id;
        int Price;
        int count;
        struct _node * next;

}node;

typedef struct _queue{

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
	
	node * file = create_node(item, trader, price, count);
	
        node * point = thelist->start;
        if(point == NULL){
                
                thelist->start = file;
                return;
        }
        if(point->Price > price){
        	thelist->start = file;
        	file->next = point;
        	return;
        }
        
        while(point->next!=NULL && point->next->Price < price){
        
                point = point->next;
        }
        node * temp = point->next;
        point->next = file;
        file->next = temp;
	return;
}

void pushb(queue * thelist, int item, int trader,  int price, int count){
	
	node * file = create_node(item, trader, price, count);
	
        node * point = thelist->start;
        if(point == NULL){
                
                thelist->start = file;
                return;
        }
        if(point->Price < price){
        	
        	thelist->start = file;
        	file->next = point;
        	return;
        }
        
        while(point->next != NULL && point->next->Price > price){
        
                point = point->next;
        }
        node * temp = point->next;
        point->next = file;
        file->next = temp;
        return;

}

void pop(queue * thelist){
	
	if(thelist->start == NULL) return;
	node * temp = thelist->start;
 	thelist->start = thelist->start->next;
 	free(temp);

}

void print_queue(queue * thelist){

        node * point = thelist->start;
        while(point!=NULL){
                
                printf("Item: %d Trader: %d  Price: %d  Count: %d \n ", point->Item_Id, point->Trader_Id, point->Price, point->count  );
                point = point->next;
        }
        printf("\n");

}


