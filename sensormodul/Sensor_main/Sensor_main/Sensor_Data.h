#ifndef SENSOR_DATA
#define SENSOR_DATA
#endif

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
	bool has_data;
}; typedef struct Sensor_Data Sensor_Data;

#define PACKET_SIZE (sizeof(Sensor_Data))

union SPI_Packet
{
	Sensor_Data sd;
	unsigned char packet[sizeof(Sensor_Data)];
}; typedef union SPI_Packet SPI_Packet;

Sensor_Data create_empty_sensor(bool data);
