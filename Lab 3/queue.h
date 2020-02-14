#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED


typedef struct _node node;

typedef struct _queue queue;

queue * create_queue(void);
node * create_node(int, int, int, int);
void pushs(queue * , int, int, int, int  );
void pushb(queue * , int, int, int, int  );
void pop(queue * );
void print_queue(queue * );

#endif
