#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "Sensor_Data.c"

#define MAX_SIZE 10

struct FIFO_Queue
{
	Sensor_Data array[MAX_SIZE];
	int front = 0;
	int rear = -1;
	int length = 0;

	Sensor_Data get_front()
	{
		return array[front];
	}

	Sensor_Data get_rear()
	{
		return array[rear];
	}

	bool is_full()
	{
		return (length==MAX_SIZE);
	}
};
typedef struct FIFO_Queue FIFO;

Sensor_Data dequeue(FIFO* f)
{
	if(f.length == 0)
	{
		fprintf(stderr, "Queue is empty, can not dequeue! \n");
		return Sensor_Data();
	}
	Sensor_Data data = f.array[front++];
	if (f.front == MAX_SIZE)
	{
		f.front = 0;
	};
	length--;
	return f.data;
}

void enqueue(FIFO* f, Sensor_Data data)
{
	if(!f.is_full())
	{
		if(f.rear == MAX_SIZE-1)
		{
			f.rear = -1;
		}
		f.array[++f.rear] = f.data;
		f.length++;
	}
	else
	{
		fprintf(stderr, "Queue is full, can not enqueue! \n");
	}
}

int main()
{
	FIFO f;
  	int i = 0;
	  /*
  	while(i < 14)
  	{
  		Sensor_Data s;
  		s.acc_x = i;
  		f.enqueue(s);
  		i++;
  	}
  	while(i > 0)
  	{
  		printf("Value: %d \n", f.dequeue().acc_x);
  		i--;
  	}
//	printf("Int %d", dequeue(f));
*/
	return 0;
}