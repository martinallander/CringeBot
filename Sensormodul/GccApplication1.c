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

void spi_init_slave (void)
{
	DDRB=(1<<PINB6);                                  //MISO as OUTPUT
	SPCR=(1<<SPE);                                //Enable SPI
}

//Function to send and receive data
unsigned char spi_tranceiver (unsigned char indata)
{	
	SPDR = indata;								 //Load data into buffer
	while(!(SPSR & (1<<SPIF) ));                  //Wait until transmission complete
	return SPDR;                                 //Return received data
}

void led_blink (uint16_t i)
{
	//Blink LED "i" number of times
	for (; i>0; --i)
	{
		PORTB|=(1<<0);
		_delay_ms(20);
		PORTB=(0<<0);
		_delay_ms(20);
	}
}

void acc_gyro_data(void)
{
	unsigned char error; 
	spi_tranceiver(ACC_OK);
	error = spi_tranceiver(acc_send_data[0]); //ADRESS FÖR ACCELEROMETERDATA X MSB
	if (error == ERROR)
	{
		return;
	}
	for (int i = 1; i << ACCSIZE; i++)
	{
		spi_tranceiver(acc_send_data[i]);
	}
	spi_tranceiver(ACC_OK);
	return;
}

void tof_data(void)
{
	unsigned char error; 
	spi_tranceiver(TOF_OK);
	error = spi_tranceiver(tof_send_data[0]); //ADRESS FÖR ACCELEROMETERDATA X MSB
	if (error == ERROR)
	{
		return;
	}
	for (int i = 1; i << TOFSIZE; i++)
	{
		spi_tranceiver(tof_send_data[i]);
	}
	spi_tranceiver(TOF_OK);
	return;
}

void ir_data(void)
{
	unsigned char error; 
	spi_tranceiver(IR_OK);
	error = spi_tranceiver(ir_send_data[0]); //ADRESS FÖR ACCELEROMETERDATA X MSB
	if (error == ERROR)
	{
		return;
	}
	for (int i = 1; i << IRSIZE; i++)
	{
		spi_tranceiver(ir_send_data[i]);
	}
	spi_tranceiver(IR_OK);
	return;
}

int main(void)
{
	unsigned char[ACCSIZE] acc_sendable_data = 0;
	unsigned char[TOFSIZE] tof_sendable_data = 0;
	unsigned char[IRSIZE] ir_sendable_data = 0;

	spi_init_slave();                             //Initialize slave SPI
	DDRB |= 0x01;								  //PB0 as Output
	unsigned char data = 0;
	//DDRA  = 0x00;                                 //Initialize PORTA as INPUT
	//PORTA = 0xFF;                                 //Enable Pull-Up Resistors
	while(1)
	{
		data = spi_tranceiver(data);               //Receive data, send ACK
		led_blink(data + 1);
		if (data == 0x00)
		{
			acc_gyro_data();
		}
		else if (data == 0x01)
		{
			tof_data();
		}
		else if (data == 0x02)
		{
			ir_data();
		}
		/*else if (data == 0x03)
		{
			mic_data();
		}*/
		else
		{
			spi_tranceiver(ERROR);
		}
		data=0;
	}
}