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
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#define F_SCL 100000UL
#define Prescaler 1
#define TWBR_value ((((F_CPU/F_SCL)/Prescaler)-16)/2)

//volatile unsigned char i2c_addr;

void i2c_init(void)
{	
	sei();
	TWSR = 0x00; //S�tt prescaler-bitar till noll.
	TWBR = 0x0C;
	TWCR = (1 << TWEN);
}

uint8_t i2c_start(uint8_t sad_addr) //Ta in slavens address + skriv/l�s. SAD + W/R SKA VARA W-bit!
{
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
	
	while(!(TWCR & (1 << TWINT)));
	
	if((TWSR & 0xF8) != TW_START)
	{
		return 1;
	}
	//laddar TWDR med slavadressen
	TWDR = sad_addr;
	//starta transmissionen av adressen
	TWCR = (1 << TWINT)|(1 << TWEN);
	//V�nta f�r att transmissionen ska bli klar
	while(!(TWCR & (1 << TWINT)));
	
	uint8_t twst = TW_STATUS & 0xF8;
	//kolla om en acknowledge f�tts
	if((twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK))
	{
		return 1;
	} 
	
	return 0;
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
		return 1;
	}
	
	return 0;
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

//kan vara fel att anv�nda dessa. De utkommenterade funk. l�ngst ned kan vara
//l�mpligare. problemet som uppst�r �r "data-pekaren". Antingen g�r om till vanlig
//int. eller anv�nda utkommenterade funktioner typ.
uint8_t i2c_write_register(uint8_t device_addr, uint8_t register_addr, uint8_t* data, uint16_t length)
{
	if (i2c_start(device_addr | 0x00))
	{
		return 1;
	}
	
	i2c_write(register_addr);

	for (uint16_t i = 0; i < length; i++)
	{
		if (i2c_write(data[i])) 
		{
			return 1;
		}
	}
	
	i2c_stop();

	return 0;
}

/*
vad ska egentligen "data"-parametern vara? d�r kommer den l�sta datan att hamna.
men om man tar TWDR som jag tror, s� skriver man ju TWDR till TWDR. Man kanske ska
skriva till n�n port vid test f�r att senare skriva till SPI kanalen f�r att skicka
vidare till Raspberry PI:n?
*/
uint8_t i2c_read_register(uint8_t device_addr, uint8_t register_addr, uint8_t* data, uint16_t length)
{
	if (i2c_start(device_addr))
	{
		return 1;
	}

	i2c_write(register_addr);

	if (i2c_start(device_addr | 0x01)) 
	{
		return 1;
	}

	for (uint16_t i = 0; i < (length-1); i++)
	{
		data[i] = i2c_ack_read();
	}
	
	data[(length-1)] = i2c_nack_read();

	i2c_stop();

	return 0;
}

uint8_t i2c_get_status(void)
{
	uint8_t status;
	status = (TWSR & 0xF8);
	return status;
}

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
	#define accel_addr_write	0x32
	#define accel_addr_read		0x33
	
	#define test_register_addr 0x20
	
	DDRB = (1 << DDB0); //output
	
	//initiering av I2C
	i2c_init();
	
	while (1)
	{
		i2c_write_register(accel_addr_write, test_register_addr, 0x47, 0x0008);
		
		i2c_read_register(accel_addr_read, test_register_addr, TWDR, 0x0008);
		
		if (TWDR == 0x47)
		{
			led_blink(10);
		} 
		else
		{
			led_blink(5);
		}
		//Beh�vs det en return h�r??
		return 0;
	}
	//return 0;
}




//uint8_t i2c_transmit(uint8_t addr, uint8_t* data, uint16_t length)
//{
	////Os�ker p� om en startsignal ska skickas innan skrivning. Kan fungera som kontroll
	//// av lyckad �verf�ring eller liknande???
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