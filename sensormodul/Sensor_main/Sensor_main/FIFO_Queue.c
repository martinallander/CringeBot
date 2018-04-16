struct Node
{
	int data;
	struct Node* next;
};

typedef struct Node Node;
struct FIFO_Queue
{
	Node* head;
	Node* tail;
};
typedef struct FIFO_Queue FIFO;

FIFO* new_queue(void)
{
	FIFO* p = malloc(1 * sizeof(*p));  
	if(p==NULL)
	{
		fprintf(stderr, "malloc() failed \n");
	}
	p->head = p->tail = NULL;
}

void FIFO_test()
{
	FIFO* my_q = new_queue();
	free(my_q);
}