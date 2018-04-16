/*
 * Kandidatmodul1.c
 *
 * Created: 2018-03-20 13:37:44
 * Author : theod
 */ 
/*
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define ACC 0x00
#define TOF 0x01
#define IR 0x02
#define MIC 0x03
#define LONG_TIME 100

#define ERROR 0x37
#define ACC_OK 0x59
#define TOF_OK 0xBE
#define IR_OK 0xA2
#define MIC_OK 0xE6

//Initialize SPI Master Device
void spi_init_master (void)
{
	DDRB = (1<<PINB5)|(1<<PINB7)|(1<<PINB4);              //Set MOSI, SCK, SS as Output
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);			//Enable SPI, Set as Master
	PORTB =(1<<PINB4);
	//Prescaler: Fosc/16, Enable Interrupts
}

//Function to send and receive data
unsigned char spi_tranceiver (unsigned char indata)
{
	PORTB =(0<<PINB4);
	SPDR = indata;                       //Load data into the buffer
	while(!(SPSR & (1<<SPIF) ));       //Wait until transmission complete
	PORTB =(1<<PINB4);
	return SPDR;                      //Return received data
	
}

void led_blink (uint16_t i)
{
	//Blink LED "i" number of times
	for (; i>0; --i)
	{
		PORTB|=(1<<0);
		_delay_ms(12);
		PORTB=(0<<0);
		_delay_ms(12);
	}
}

//Main
int main(void)
{
	spi_init_master();                  //Initialize SPI Master
	DDRB |= 0x01;                       //PB0 as Output
	
	unsigned char data;                 //Received data stored here
	//uint8_t x = 0;                      //Counter value which is sent
	
	while(1)
	{
		spi_tranceiver(ACC);
		_delay_ms(200);
		data = spi_tranceiver(ACC);     //Send "x", receive ACK in "data"
		led_blink(data);
		data = 0;
		_delay_ms(400);                 //Wait
		
		spi_tranceiver(TOF);
		_delay_ms(200);
		data = spi_tranceiver(TOF);     //Send "x", receive ACK in "data"
		led_blink(data);
		data = 0;
		_delay_ms(400);                 //Wait
		
		spi_tranceiver(IR);
		_delay_ms(200);
		data = spi_tranceiver(IR);     //Send "x", receive ACK in "data"
		led_blink(data);
		data = 0;
		_delay_ms(400);                 //Wait
		
		spi_tranceiver(MIC);
		_delay_ms(200);
		data = spi_tranceiver(MIC);     //Send "x", receive ACK in "data"
		led_blink(data);
		data = 0;
		_delay_ms(400);                 //Wait
	}
}










/*
//#define F_CPU 8000000UL //Clock speed 8MHz

#include <avr/io.h>
#include <avr/interrupt.h>

void spi_init_master (void)
{
	DDRB=(1<<DDB4) | (1<<DDB5) | (1<<DDB7) | (1<<DDB0);								  //MISO as OUTPUT
	SPCR=(1<<SPE) | (1<<MSTR);							      //Enable SPI
}

//Function to send and receive data
unsigned char spi_transceiver (unsigned char data)
{
	SPDR = data;                                  //Load data into buffer
	while(!(SPSR & (1<<SPIF)));                  //Wait until transmission complete
	return(SPDR);                                 //Return received data
}



int main(void)
{
	unsigned char reg1;
	unsigned char reg2;
	spi_init_master();
	while (1)
	{
		reg1 = spi_transceiver(0x12);
		reg2 = spi_transceiver(reg1);
		if (reg2 == 0x12)
		{
			PINB = (1<<DDB1);
		}
	}
}


*/