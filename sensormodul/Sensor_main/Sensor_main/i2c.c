#include <util/twi.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "definitions.h"

volatile bool i2c_done = 1;
volatile bool write_to_slave = 1;
volatile uint8_t trans_data;
volatile uint8_t rec_data;
volatile uint8_t device_addr;
volatile uint8_t register_addr;
volatile int n_o_writes = 0;

//Initializer of i2c
void i2c_init(void)
{	
	TWSR = 0x00; //Set prescaler-bitar to zero.
	TWBR = 0x92; //från källe-man
	TWCR = (1 << TWEN)|(1 << TWIE);
}

void i2c_start(void)
{
	TWCR = (1 << TWINT)|(1 << TWSTA)|(0 << TWSTO)|(1 << TWEN)|(1 << TWIE);
	i2c_done = 0;
}

void i2c_stop(void)
{
	TWCR = (1 << TWINT)|(0 << TWSTA)|(1 << TWSTO)|(1 << TWEN)|(1 << TWIE);
	i2c_done = 1;
}

void i2c_send_data(uint8_t data)
{
		TWDR = data;
		TWCR = (1 << TWINT)|(0 << TWSTA)|(0 << TWSTO)|(1 << TWEN)|(1 << TWIE);
}



/*===========================================================================================
Interrupt for I2C
-Can only handle single write and read at the moment
-repeated start is "hard coded" only for read
-n_o_writes needs to be replace and probably remade, as for now it become negative
-Wondering if the i2c_stop should not set TWIE to 1, it should stop there.
===========================================================================================*/


ISR(TWI_vect)
{
	uint8_t status = (TWSR & 0xF8);
	switch (status)
	{
		case TW_START: //0x08
		i2c_send_data(temp_addr + I2C_WRITE);
		break;
		case TW_REP_START: //0x10
		i2c_send_data(temp_addr + I2C_READ);
		break;
		
		case TW_MT_SLA_ACK: //3
		i2c_send_data(register_addr); //load the register we want to handle
		break;
		case TW_MT_SLA_NACK: //4
		led_blinker(1);
		i2c_stop();
		break;
		case TW_MT_DATA_ACK: //5
		if(write_to_slave)
		{
			if(n_o_writes == 0)
			{
				i2c_stop();
				break;
			}
			else
			{
				i2c_send_data(trans_data);
			}
			n_o_writes = n_o_writes - 1; //placeringen är svår. vi minskade den innan vi hade använt den
		}
		else
		{
			i2c_start(); //repeated start
		}
		
		break;
		case TW_MR_SLA_ACK: //6
		TWCR = (1 << TWINT)|(0 << TWSTA)|(0 << TWSTO)|(0 << TWEA)|(1 << TWEN)|(1 << TWIE);
		break;
		case TW_MR_DATA_NACK: //7
		rec_data = TWDR;
		i2c_stop();
		break;
		case TW_MR_DATA_ACK: //8
		rec_data = TWDR;
		i2c_stop();

		break;
	}
}

void i2c_write_reg(uint8_t reg_addr, uint8_t data, int n)
{
	while(!i2c_done){};
	n_o_writes = n;
	register_addr = reg_addr;
	trans_data = data;
	write_to_slave = 1;
	i2c_start();
}

uint8_t i2c_read_reg(uint8_t reg_addr)
{
	while(!i2c_done){};
	register_addr = reg_addr;
	write_to_slave = 0;
	i2c_start();
	while(!i2c_done){};
	return rec_data;
}
