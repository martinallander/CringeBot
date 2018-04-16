/*
 * Kandidatmodul1.c
 *
 * Created: 2018-03-20 13:37:44
 * Author : theod
 */ 



#define ERROR 0x37
#define ACC_OK 0x59
#define TOF_OK 0xBE
#define IR_OK 0xA2
#define MIC_OK 0xE6

#define ACCSIZE 12
#define TOFSIZE 2
#define IRSIZE 64


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void spi_init(void)
{
	SPCR=(1<<SPE);                                //Enable SPI
}

//Function to send and receive data
unsigned char spi_tranceiver (unsigned char indata)
{	
	SPDR = indata;								 //Load data into buffer
	while(!(SPSR & (1<<SPIF) ));                  //Wait until transmission complete
	return SPDR;                                 //Return received data
}

void miso_test (uint16_t i)
{
	//Blink LED "i" number of times
	for (; i>0; --i)
	{
		PORTB|=(1<<PORTB6);
		_delay_ms(500);
		PORTB=(0<<PORTB6);
		_delay_ms(500);
	}
}

void mosi_test (uint16_t i)
{
	//Blink LED "i" number of times
	for (; i>0; --i)
	{
		PORTB|=(1<<PORTB5);
		_delay_ms(500);
		PORTB=(0<<PORTB5);
		_delay_ms(500);
	}
}
void data_direction_init()
{
	DDRB = (1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB6); //Set PB0, PB1, PB2 and PB6 (MISO) as outputs
}

void spi_test(unsigned int spi_data)
{
	//spi_data = spi_tranceiver(spi_data);
	if (spi_data == 0xAA)
	{
		spi_tranceiver(0x01);
		led_blink_red(1);
	}
	else if (spi_data == 0x05)
	{
		spi_tranceiver(0x02);
		led_blink_green(1);
	}
	else if (spi_data == 0x0f)
	{
		spi_tranceiver(0x03);
		led_blink_yellow(1);
	}
	else
	{
		led_blink_yellow(2);
	}
}

/*
int main(void)
{
	
	data_direction_init();							//Set data direction 
	spi_init();										//Initialize SPI
	unsigned char data = 0;
	led_blink_yellow(1);
	led_blink_green(1);
	led_blink_red(1);
	
	while(1)
	{	
		data = spi_tranceiver(data);				 //Receive data, send ACK
		if (data == 0xAA)
		{
			spi_tranceiver(0x01);
			led_blink_red(1);
		}
		else if (data == 0x05)
		{
			spi_tranceiver(0x02);
			led_blink_green(1);
		}
		else if (data == 0x0f)
		{
			spi_tranceiver(0x03);
			led_blink_yellow(1);
		}
		else
		{
			led_blink_yellow(2);
		}
		data=0;
		
	}
}

*/