#include <stdlib.h>
#include <stdio.h>
#include "FIFO_Queue.h"


FIFO create_empty_FIFO()
{
	FIFO f;
	f.front = 0;
	f.rear = -1;
	f.length = 0;
	return f;
}

Sensor_Data get_front(FIFO f)
{
	return f.array[f.front];
}

Sensor_Data get_rear(FIFO f)
{
	return f.array[f.rear];
}

bool is_full(FIFO f)
{
	return (f.length==MAX_SIZE);
}
	
void enqueue(FIFO* f, Sensor_Data data)
{
	if(is_full(*f))
	{
		if(f->rear == MAX_SIZE-1) 
		{
			f->rear = -1;            
		}
		f->array[++f->rear] = data;
		f->length++;
	}
	else
	{
		fprintf(stderr, "Queue is full, can not enqueue! \n");
	}
}

Sensor_Data dequeue(FIFO* f)
{
	if(f->length == 0)
	{
		fprintf(stderr, "Queue is empty, can not dequeue! \n");
		return null_ptr;
	}
	Sensor_Data data = f->array[f->front++];
	if (f->front == MAX_SIZE)
	{
		f->front = 0;
	};
	f->length--;
	return data;
}



int main()
{
	/*
	//f = (FIFO){,1,-1,0};
  	int i = 0;
  	while(i < 14)
  	{
  		Sensor_Data s;
  		s.acc_x = i;
  		enqueue(s);
  		i++;
  	}
  	while(i > 0)
  	{
  		printf("Value: %d \n", f.dequeue().acc_x);
  		i--;
  	}*/
//	printf("Int %d", dequeue(f));
	return 0;
}