#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//typedef unsigned char byte;

struct Sensor_Data
{
	short acc_x;
	short acc_y;
	short acc_z;
	short gyro_x;
	short gyro_y;
	short gyro_z;
	float tof_distance;
	float ir[64];
};

#define PACKET_SIZE (sizeof(Sensor_Data))

union SPI_Packet
{
	Sensor_Data sd;
	unsigned char packet[sizeof(Sensor_Data)];
}; typedef union SPI_Packet SPI_Packet;

/*
int main()
{
	SPI_Packet sp;
	SPI_Packet sp2;
	Sensor_Data sd1;
	sd1.acc_x = 1.0;
	sd1.acc_y = 2.0;
	sd1.acc_z = 3.0;
	sp.sd = sd1;
	for (int i = 0; i < PACKET_SIZE; ++i)
	{
		sp2.packet[i] = sp.packet[i];
	}
	printf("%d\n", sp2.sd.acc_x);
	printf("%d\n", sp2.sd.acc_y);
	printf("%d\n", sp2.sd.acc_z);
}
*/