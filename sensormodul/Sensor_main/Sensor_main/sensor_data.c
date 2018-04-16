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
/*
char[] to_string(Sensor_Data)
{
	//TODO?
}
*/