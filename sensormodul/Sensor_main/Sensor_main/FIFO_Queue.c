#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "FIFO_Queue.h"

Sensor_Data create_empty_sensor(bool data)
{
	Sensor_Data sd;
	sd.acc_x = 0;
	sd.acc_y = 0;
	sd.acc_z = 0;
	sd.gyro_x = 0;
	sd.gyro_y = 0;
	sd.gyro_z = 0;
	sd.tof_distance = 0;
	sd.has_data = data;
	return sd;
}

FIFO* create_empty_FIFO()
{
	FIFO f;
	FIFO* fp = malloc(sizeof(f));
	fp->front = 0;
	fp->rear = -1;
	fp->length = 0;
	return fp;
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
	if(!is_full(*f))
	{
		if(f->rear == MAX_SIZE-1) 
		{
			f->rear = -1;            
		}
		printf("Rear: %d \n", f->rear);
		f->array[++(f->rear)] = data;
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
		return create_empty_sensor(false);
	}
	Sensor_Data data = f->array[(f->front)++];
	if (f->front == MAX_SIZE)
	{
		f->front = 0;
	};
	f->length--;
	return data;
}



int main()
{
	
	FIFO* fp = create_empty_FIFO();
  	int i = 0;
  	while(i < 14)
  	{
  		Sensor_Data sd = create_empty_sensor(true);
  		sd.acc_x = i;
  		enqueue(fp, sd);
  		i++;
  	}
  	printf("Length: %d \n", fp->length);
  	while(i > 0)
  	{
  		printf("Value: %d \n", dequeue(fp).acc_x);
  		i--;
  	}
  	free(fp);
//	printf("Int %d", dequeue(f));
	return 0;
}