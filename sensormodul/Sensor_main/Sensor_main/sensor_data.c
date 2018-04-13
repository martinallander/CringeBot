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
/*
char[] to_string(Sensor_Data)
{
	//TODO?
}
*/