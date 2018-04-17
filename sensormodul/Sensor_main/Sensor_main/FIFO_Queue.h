#ifndef FIFO_QUEUE
#define FIFO_QUEUE

#include "Sensor_Data.h"

#define MAX_SIZE 10

struct FIFO_Queue
{
	Sensor_Data array[MAX_SIZE];
	int front;
	int rear;
	int length;
}; typedef struct FIFO_Queue FIFO;

FIFO* create_empty_FIFO();
Sensor_Data get_front(FIFO);
Sensor_Data get_rear(FIFO);
bool is_full(FIFO);
void enqueue(FIFO*, Sensor_Data);
Sensor_Data dequeue(FIFO*);
Sensor_Data create_empty_sensor(bool data);

#endif