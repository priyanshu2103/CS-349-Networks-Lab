#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED


typedef struct node node;

typedef struct queue queue;

queue * create_queue(void);
node * create_node(int, int, int, int);
void pushs(queue * , int, int, int, int  );
void pushb(queue * , int, int, int, int  );
void pops(queue * );
void popb(queue * );
void print_queue(queue * );

#endif
