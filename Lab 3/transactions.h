#ifndef TRANSACTIONS_H_INCLUDED
#define TRANSACTIONS_H_INCLUDED


typedef struct tnode tnode;

typedef struct tqueue tqueue;

queue * create_tqueue(void);
node * create_tnode(int, int, int, int, int);
void push(tqueue * , int, int, int, int,  int  );
void popt(tqueue * );
void print_tqueue(tqueue * );

#endif
