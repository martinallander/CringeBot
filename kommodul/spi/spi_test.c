#include <wiringPiSPI.h>

#define STYR 0
#define SENSOR 1

void SPI_init(void)
{
    wiringPiSPISetup(STYR, 500000); //Setup channel for styrmodul
    wiringPiSPISetup(SENSOR, 500000); //Setup channel for sensormodul
}

unsigned char[] set_instruction(int len, unsigned char instruction)
{
    if (len < 1)
    	unsigned char buffer[1] {"null"};
    	return buffer;
    else
    	unsigned char buffer[len] {instruction};
	return buffer;
}	

unsigned char[] get_acc()
{
    
}

int main()
{
    unsigned char buffer[100];
    SPI_init();
    buffer[0] = 0x77;
    wiringPiSPIDataRW(STYR, buffer[0], 1);
    return 0;
}


