/*
 * GccApplication2.c
 *
 * Created: 3/25/2018 10:28:29 AM
 *  Author: alfhu925
 */

/*=====================================================================================
 Bra grejer att ha koll på:
 * Man kan få ut värdet på en variabel i realtid genom att:
	- antingen genom att tixa med breakpoint
	- eller högerklicka på variabeln
	- eller en kombination av båda.....
=======================================================================================*/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdio.h>

#define F_SCL 100000UL
#define Prescaler 1
#define TWBR_value ((((F_CPU/F_SCL)/Prescaler)-16)/2)

/*=====================================================================================
							Variable definitions
=======================================================================================*/

#define ERROR 1
#define SUCCESS 0

#define I2C_WRITE	0
#define I2C_READ	1

#define accel_addr 0x32
#define test_reg_addr 0x20

//volatile unsigned char i2c_addr;

/*=====================================================================================
						I2C configuration and initation
=======================================================================================*/


void i2c_init(void)
{	
	sei();
	TWSR = 0x00; //Set prescaler-bitar to zero.
	TWBR = 0x0C;
	TWCR = (1 << TWEN);
}

uint8_t i2c_start(uint8_t addr) //Takes the slave address and write/read bit. SAD + W/R.
{
	//reset I2C control register
	TWCR = 0;
	
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
	
	while(!(TWCR & (1 << TWINT)));
	
	if((TWSR & 0xF8) != TW_START)
	{
		return ERROR;
	}
	//load TWDR with slave address
	TWDR = addr;
	//start transmission of address
	TWCR = (1 << TWINT)|(1 << TWEN);
	//Wait for the transmission to finish
	while(!(TWCR & (1 << TWINT)));
	
	uint8_t twst = TW_STATUS & 0xF8;
	//check for acknowledge from slave
	if((twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK))
	{
		return ERROR;
	} 
	
	return SUCCESS;
}

uint8_t i2c_repeated_start(uint8_t addr)
{
	return i2c_start(addr);
}

void i2c_stop(void)
{
	TWCR = (1 << TWINT)|(1 << TWSTO)|(1 << TWEN);
}

uint8_t i2c_write(uint8_t data)
{
	TWDR = data;
	//start transmission of data
	TWCR = (1 << TWINT)|(1 << TWEN);
	//wait for the transmission to finish
	while(!(TWCR & (1 << TWINT)));
	//Check for data-transmit-acknowledge 
	if((TWSR & 0xF8) != TW_MT_DATA_ACK)
	{
		return ERROR;
	}
	
	return SUCCESS;
}

uint8_t i2c_ack_read(void)
{
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA);
	while(!(TWCR & (1 << TWINT)));
	return TWDR;
}

uint8_t i2c_nack_read(void)
{
	TWCR = (1 << TWINT)|(1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
	return TWDR;
}

//uint8_t i2c_get_status(void)
//{
//uint8_t status;
//status = (TWSR & 0xF8);
//return status;
//}

/*=====================================================================================
						Help functions for tests and controls
=======================================================================================*/


void led_blinker(uint8_t times)
{
	//Blink LED "times" number of times
	for (uint8_t i = 0; i < times; i++)
	{
		PORTB |= (1<<0);
		_delay_ms(10);
		PORTB = (0<<0);
		_delay_ms(10);
	}
}


/*=====================================================================================
		The following functions are specific to each unit on the I2C-bus
=======================================================================================*/

/*=====================================================================================
--------------------------------Accelerometer - LSM303DLHC-----------------------------
=======================================================================================*/
void accel_i2c_write_reg(uint8_t reg_addr, uint8_t data)
{
	i2c_start(accel_addr + I2C_WRITE);
	i2c_write(reg_addr);
	i2c_write(data);
	i2c_stop();
}

//Data är där data sparas
uint8_t accel_i2c_read_reg(uint8_t reg_addr, uint8_t data)
{
	i2c_start(accel_addr + I2C_WRITE);
	i2c_write(reg_addr);
	i2c_repeated_start(accel_addr + I2C_READ);
	//osäker på om man ska ha och hur data = i2c_nack_read() fungerar...
	data = i2c_nack_read();
	i2c_stop();	
	return data;
}

/*
OBS! Måste skapa en array-variabel för att spara undan datan. för att sedan tillkalla
funktionen med. Kom ihåg längd på arrayen. Finns dynamisk variant. MALLOC google är är
din vän.

KOLLA UPP OM MAN MÅSTE SÄTTA EN VISS BIT I REGISTRET FÖR ATT UTÖKA DET!!!!
*/
void accel_i2c_multiple_read_reg(uint8_t reg_addr, uint8_t* data, uint16_t length)
{
	i2c_start(accel_addr + I2C_WRITE);
	i2c_write(reg_addr);
	i2c_repeated_start(accel_addr + I2C_READ);
	
	for (uint16_t i = 0; i < (length -1); i++)
	{

		data[i] = i2c_ack_read();
	}
	data[(length - 1)] = i2c_nack_read();
	
	i2c_stop();
}

/*
hur man "Castar" till ny datatyp ex: av Oscar!
uint8_t sparad_data_16_B
(float) sparad_data_16_B
*/







int main(void)
{
	
	DDRB = (1 << DDB0);
	PORTB = (0 << PORTB0);
	_delay_ms(500);
    //PORTB = 0x00;
	//kanske behöver en array?
	uint8_t saved_data = 0x00;
	i2c_init();
	
	accel_i2c_write_reg(test_reg_addr,0x55);
	
	//while (1)
	//{
		//accel_i2c_read_reg(test_reg_addr, saved_data);
	//}
	uint8_t final_data = accel_i2c_read_reg(test_reg_addr, saved_data);
	//printf("Detta är värdet på sparad data%d\n", saved_data);	
	
	//led_blinker(final_data);
	led_blinker(saved_data);
	if (final_data == 0x55)
	{
		led_blinker(50);
	}
	else
	{
		led_blinker(1);
	}
	return SUCCESS;
}




//uint8_t i2c_transmit(uint8_t addr, uint8_t* data, uint16_t length)
//{
	////Osäker på om en startsignal ska skickas innan skrivning. Kan fungera som kontroll
	//// av lyckad överföring eller liknande???
	//if (i2c_start(addr | I2C_WRITE))
	//{
		//return 1;
	//}
	//
	//for (uint16_t i = 0; i < length; i++)
	//{
		//if (i2c_write(data[i])) return 1;
	//}
	//
	//i2c_stop();
	//
	//return 0;
//}
//
//uint8_t i2c_receive(uint8_t addr, uint8_t* data, uint16_t length)
//{
	//if (i2c_start(addr | I2C_READ))
	//{
		//return 1;
	//}
	//
	//for (uint16_t i = 0; i < (length-1); i++)
	//{
		//data[i] = i2c_ack_read();
	//}
	//
	//data[(length-1)] = i2c_nack_read();
	//
	//i2c_stop();
	//
	//return 0;
//}

////kan vara fel att använda dessa. De utkommenterade funk. längst ned kan vara
////lämpligare. problemet som uppstår är "data-pekaren". Antingen gör om till vanlig
////int. eller använda utkommenterade funktioner typ.
//uint8_t i2c_write_register(uint8_t device_addr, uint8_t register_addr, uint8_t* data, uint16_t length)
//{
	//if (i2c_start(device_addr | 0x00))
	//{
		//return 1;
	//}
	//
	//i2c_write(register_addr);
//
	//for (uint16_t i = 0; i < length; i++)
	//{
		//if (i2c_write(data[i])) 
		//{
			//return 1;
		//}
	//}
	//
	//i2c_stop();
//
	//return 0;
//}
//
///*
//vad ska egentligen "data"-parametern vara? där kommer den lästa datan att hamna.
//men om man tar TWDR som jag tror, så skriver man ju TWDR till TWDR. Man kanske ska
//skriva till nån port vid test för att senare skriva till SPI kanalen för att skicka
//vidare till Raspberry PI:n?
//*/
//uint8_t i2c_read_register(uint8_t device_addr, uint8_t register_addr, uint8_t* data, uint16_t length)
//{
	//if (i2c_start(device_addr))
	//{
		//return 1;
	//}
//
	//i2c_write(register_addr);
//
	//if (i2c_start(device_addr | 0x01)) 
	//{
		//return 1;
	//}
//
	//for (uint16_t i = 0; i < (length-1); i++)
	//{
		//data[i] = i2c_ack_read();
	//}
	//
	//data[(length-1)] = i2c_nack_read();
//
	//i2c_stop();
//
	//return 0;
//}