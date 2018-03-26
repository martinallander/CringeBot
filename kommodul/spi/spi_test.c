#include <wiringPiSPI.h>

#define STYR 0
#define SENSOR 1

#define ACCSIZE 12
#define TOFSIZE 2
#define IRSIZE 64

#define ERROR 0x37
#define ACC_OK 0x59
#define TOF_OK 0xBE
#define IR_OK 0xA2
#define MIC_OK 0xE6


void SPI_init(void)
{
    wiringPiSPISetup(STYR, 500000); //Setup channel for styrmodul
    wiringPiSPISetup(SENSOR, 500000); //Setup channel for sensormodul
}

bool get_acc(unsigned char[]* acc_data)
{
    wiringPiSPIDataRW(SENSOR, 0x00, 1); //00000000 = ACC + GYRO
	for (int i = 0; i < ACCSIZE + 2; i++)
	{
		acc_data[i] = wiringPiSPIDataRW(SENSOR, 0x00, 1); //Send whateva
		
		if(i = 0)
		{
			if (acc_data[0] != ACC_OK)
			{
				wiringPiSPIDataRW(SENSOR, ERROR, 1); //If not ACC_OK send error
				return false;
			}
		}
	}
	if (acc_data[ACCSIZE + 1] != ACC_OK)
	{
		return false;
	}
	return true;
}

bool get_tof(unsigned char[]* tof_data)
{
	wiringPiSPIDataRW(SENSOR, 0x01, 1); //00000001 = TOF
	for (int i = 0; i < TOFSIZE + 2; i++)
	{
		tof_data[i] = wiringPiSPIDataRW(SENSOR, 0x00, 1); //Send whateva
		
		if(i = 0)
		{
			if (tof_data[0] != TOF_OK)
			{
				wiringPiSPIDataRW(SENSOR, ERROR, 1); //If not TOF_OK send error
				return false;
			}
		}
	}
	if (tof_data[TOFSIZE + 1] != TOF_OK)
	{
		return false;
	}
	return true;
}

bool get_ir(unsigned char[]* ir_data)
{
	wiringPiSPIDataRW(SENSOR, 0x02, 1); //00000010 = IR
	for (int i = 0; i < IRSIZE + 2; i++)
	{
		ir_data[i] = wiringPiSPIDataRW(SENSOR, 0x00, 1); //Send whateva
		
		if(i = 0)
		{
			if (ir_data[0] != IR_OK)
			{
				wiringPiSPIDataRW(SENSOR, ERROR, 1); //If not IR_OK send error
				return false;+
			}
		}
	}
	if (acc_data[IRSIZE + 1] != IR_OK)
	{
		return false;
	}
	return true;
}

int main()
{
    unsigned char buffer[100];
    SPI_init();
    buffer[0] = 0x77;
    wiringPiSPIDataRW(STYR, buffer[0], 1);
    return 0;
}


