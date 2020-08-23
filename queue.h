int full;
typedef struct node {
    char *s;
    struct node *next;
}node;

typedef struct queue {
    node *head;
    node *tail;
}queue;

queue q;
void enq(queue*, char*); 
char* deq(queue*);  
int qfull(queue*);
int qempty(queue*);
void qinit(queue*);
