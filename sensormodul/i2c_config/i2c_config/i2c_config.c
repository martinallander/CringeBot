/*
 * GccApplication2.c
 *
 * Created: 3/25/2018 10:28:29 AM
 *  Author: alfhu925
 */

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#define F_SCL 100000UL
#define Prescaler 1
#define TWBR_value ((((F_CPU/F_SCL)/Prescaler)-16)/2)

#define ERROR 1
#define SUCCESS 0

#define I2C_WRITE	0
#define I2C_READ	1

#define accel_addr 0x32

//volatile unsigned char i2c_addr;

void i2c_init(void)
{	
	sei();
	TWSR = 0x00; //Sätt prescaler-bitar till noll.
	TWBR = 0x0C;
	TWCR = (1 << TWEN);
}

uint8_t i2c_start(uint8_t addr) //Ta in slavens address + skriv/läs. SAD + W/R SKA VARA W-bit!
{
	//reset I2C control register
	//TWCR = 0;
	
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
	
	while(!(TWCR & (1 << TWINT)));
	
	if((TWSR & 0xF8) != TW_START)
	{
		return ERROR;
	}
	//laddar TWDR med slavadressen
	TWDR = addr;
	//starta transmissionen av adressen
	TWCR = (1 << TWINT)|(1 << TWEN);
	//Vänta för att transmissionen ska bli klar
	while(!(TWCR & (1 << TWINT)));
	
	uint8_t twst = TW_STATUS & 0xF8;
	//kolla om en acknowledge fåtts
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
	TWCR = (1 << TWINT)|(1 << TWEN);
	
	while(!(TWCR & (1 << TWINT)));
	
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


void accel_i2c_write_reg(uint8_t reg_addr, uint8_t data)
{
	i2c_start(accel_addr + I2C_WRITE);
	i2c_write(reg_addr);
	i2c_write(data);
	i2c_stop();
}

//Data är där data sparas
void accel_i2c_read_reg(uint8_t reg_addr, uint8_t data)
{
	i2c_start(accel_addr + I2C_WRITE);
	i2c_write(reg_addr);
	i2c_repeated_start(accel_addr + I2C_READ);
	//osäker på om man ska ha och hur data = i2c_nack_read() fungerar...
	data = i2c_nack_read();
	i2c_stop();	
}

/*
OBS! Måste skapa en array-variabel för att spara undan datan. för att sedan tillkalla
funktionen med. Kom ihåg längd på arrayen. Finns dynamisk variant. MALLOC google är är
din vän.
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


//uint8_t i2c_get_status(void)
//{
	//uint8_t status;
	//status = (TWSR & 0xF8);
	//return status;
//}

void led_blink (uint16_t i)
{
	//Blink LED "i" number of times
	for (; i>0; --i)
	{
		PORTB |= (1<<0);
		_delay_ms(100);
		PORTB = (0<<0);
		_delay_ms(100);
	}
}


int main(void)
{
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