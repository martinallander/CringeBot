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
	
	void enqueue(Sensor_Data data)
	{
		if(!is_full())
		{
			if(rear == MAX_SIZE-1) 
			{
			rear = -1;            
			}
			array[++rear] = data;
			length++;
		}
		else
		{
			fprintf(stderr, "Queue is full, can not enqueue! \n");
		}
	}

	Sensor_Data dequeue()
	{
		if(length == 0)
		{
			fprintf(stderr, "Queue is empty, can not dequeue! \n");
			return Sensor_Data();
		}
		Sensor_Data data = array[front++];
		if (front == MAX_SIZE)
		{
			front = 0;
		};
		length--;
		return data;
	}
};
typedef FIFO_Queue FIFO;



int main()
{
	FIFO f;
  	int i = 0;
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

	
	return 0;
}

	/*
	printf("Rear %d \n", f.rear);
	enqueue(f, 1);
	printf("Rear %d \n", f.rear);
	enqueue(f, 2);
	printf("Rear %d \n", f.rear);
	enqueue(f, 3);
	printf("Rear %d \n", f.rear);
	printf("Front %d \n", f.front);
	printf("Int %d \n", dequeue(f));
	printf("Front %d \n", f.front);
  	printf("Int %d \n", dequeue(f));
  	printf("Front %d \n", f.front);
  	printf("Int %d \n", dequeue(f));
  	printf("Front %d \n", f.front);
  	printf("Int %d \n", dequeue(f));
  	printf("Front %d \n", f.front);
  	printf("Rear %d \n", f.rear);
  	*/