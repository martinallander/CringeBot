/*
 * GccApplication2.c
 *
 * Created: 3/25/2018 10:28:29 AM
 *  Author: alfhu925
 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>

void i2c_init(void)
{	
	sei();
	TWSR = 0x00; //Sätt prescaler-bitar till noll.
	TWBR = 0x0C;
	TWCR = (1 << TWEN);
}

void i2c_start(void)
{
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
}

void i2c_stop(void)
{
	TWCR = (1 << TWINT)|(1 << TWSTO)|(1 << TWEN);
}

void i2c_write(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWINT)|(1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
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

uint8_t i2c_get_status(void)
{
	uint8_t status;
	status = TWSR & 0xF8;
	return status;
}

void i2c_wait(void)
{
	//Kolla så att start-condition har skickats.
	if((TWSR & 0xF8) != 0x08)
		ERROR();
}

void ERROR(void)
{
	printf("error");
}

int SCL_freq(int freq, int prescaler)
{
	int SCL_frequency = F_CPU/(16 + 2*(TWBR*prescaler));
}
	//Table 26-7 for value of pull-up resistor
/*
uint8_t write_byte(uint16_t addr, uint8_t data)
{
	int gyro_addr = 0b10100000;
	i2c_start();
	if (i2c_get_status() != 0x08)
		return 0;
	i2c_write((gyro_addr)|(uint8_t)((addr & 0x0700) >> 7));
	if (i2c_get_status() != 0x18)
		return ERROR;
	i2c_write((data)(addr));
	if (i2c_get_status() != 0x28)
		return ERROR;
	i2c_write(data);
		if (i2c_get_status() != 0x28)
		return ERROR;
	i2c_stop();
	return SUCCESS;
}
*/

int main(void)
{
    i2c_init();
	i2c_start();
	i2c_wait();
	i2c_write(0x0C);
	i2c_stop();
	return 0;
}