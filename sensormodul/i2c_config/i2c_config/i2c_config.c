/*
 * i2c_config.c
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

#include <math.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#define F_SCL 50UL
#define Prescaler 1
#define TWBR_value ((((F_CPU/F_SCL)/Prescaler)-16)/2)

#define ERROR 1
#define SUCCESS 0

#define I2C_WRITE	0
#define I2C_READ	1

//#define writing_to_slave
//Kanske ska vara uint8_t
//#define writing_data

/*========================================================
					Sensor addresses
=========================================================*/
#define accel_addr 0x32
/*========================================================
						Registers
=========================================================*/
#define ctrl_reg_1 0x20
#define acc_x_l_reg 0x28
#define acc_x_h_reg 0x29
#define acc_y_l_reg 0x2A
#define acc_y_h_reg 0x2B
#define acc_z_l_reg 0x2C
#define acc_z_h_reg 0x2D

/*========================================================
				Measurement parameters
=========================================================*/
#define accel_MG_LSB  (0.001F)
#define gravity_value (9.82F)
/*========================================================
		Global variable and storage variable
=========================================================*/

volatile bool i2c_done = 1;
volatile bool write_to_slave = 1;
volatile uint8_t register_addr;
volatile uint8_t trans_data;
volatile uint8_t rec_data;
volatile int n_o_writes = 0;

//#define i2c_mode
//volatile unsigned char i2c_addr;

/*========================================================
				I2C Configuration
=========================================================*/



//Initializer of i2c
void i2c_init(void)
{	
	TWSR = 0x00; //Set prescaler-bitar to zero.
	TWBR = 0x0C;
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
	case TW_START: //1
		i2c_send_data(accel_addr + I2C_WRITE);
		break;
	case TW_REP_START: //2
		i2c_send_data(accel_addr + I2C_READ);
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
			led_blinker(3);
		i2c_stop();

		break;
	}
}


/*========================================================
			Data handling functions and others
=========================================================*/

void led_blinker(uint8_t times)
{
	_delay_ms(500);
	for (uint8_t i = 0; i < times; i++)
	{
		PORTB |= (1<<0);
		_delay_ms(500);
		PORTB = (0<<0);
		_delay_ms(500);
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
	
	return rec_data;
}

//adafruit skiftar 4 till höger i slutet... why??

int16_t shift_data(uint8_t high, uint8_t low) //high och low är båda 2-komlement
{
	int16_t shifted_data = 0;
	uint16_t shifted = (uint16_t)high << 8;
	shifted_data = (shifted | (uint16_t)low);
	return shifted_data;
}

int main(void)
{
	volatile uint8_t x_l_value;
	volatile uint8_t x_h_value;
	volatile int8_t y_l_value;
	volatile int8_t y_h_value;
	volatile int8_t z_l_value;
	volatile int8_t z_h_value;
	
	DDRB = (1 << DDB0);
	PORTB = (0 << PORTB0);
	i2c_init();
	sei();
	//samplerate = 10Hz
	uint8_t set_ctrl_reg_1_10 = 0b00100111;
	//samplerate = 50Hz
	uint8_t set_ctrl_reg_1_50 = 0b01000111;
	//samplerate = 100Hz
	uint8_t set_ctrl_reg_1_100 = 0b01010111;
	/*-----------------------------------------------------------------
	We maybe need to look into CTRL_REG4_A(0x23) to adjust the sensitivity
	CTRL_REG2_A(0x21) configurate a HP-filter
	-----------------------------------------------------------------*/
	i2c_write_reg(ctrl_reg_1, set_ctrl_reg_1_10, 1);
	while(1)
	{

		
		
		_delay_ms(10);
	//	i2c_read_reg(ctrl_reg_1);
		x_l_value = i2c_read_reg(acc_x_l_reg);
		x_h_value = i2c_read_reg(acc_x_h_reg);
		y_l_value = i2c_read_reg(acc_y_l_reg);
		y_h_value = i2c_read_reg(acc_y_h_reg);
		//z_l_value = i2c_read_reg(acc_z_l_reg);
		//z_h_value = i2c_read_reg(acc_z_h_reg);
		_delay_ms(10);
		//Möjligtvis lägga in detta i avbrottsrutinen?
		volatile int16_t data_x = shift_data(x_h_value, x_l_value);
		volatile int16_t data_y = shift_data(y_h_value, y_l_value);
		//volatile int16_t data_z = shift_data(z_h_value, z_l_value);
		
	}
	return 0;
}