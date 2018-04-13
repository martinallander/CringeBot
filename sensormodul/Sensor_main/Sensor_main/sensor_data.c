#include <stdio.h>

struct Sensor_Data
{
	short acc_x = 0.0;
	short acc_y = 0.0;
	short acc_z = 0.0;
	short gyro_x = 0.0;
	short gyro_y = 0.0;
	short gyro_z = 0.0;
	float tof_distance = 0.0;
	float ir[64] = {0.0};
};

char* data_to_byte(Sensor_Data sd)
{
	char* p = (char *) & sd;

	for(int i = 0; i < sizeof(sd) ; i++) 
	{
		*p++;
	}
	return p;
}

int main()
{
	char* p;
	p = data_to_byte(Sensor_Data());
	printf("%s\n", p);
}
/*
char[] to_string(Sensor_Data)
{
	//TODO?
}
*/