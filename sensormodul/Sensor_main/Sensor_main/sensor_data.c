#include "Sensor_Data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//typedef unsigned char byte;

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