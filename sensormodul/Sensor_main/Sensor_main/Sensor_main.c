/*
 * Sensor_main.c
 *
 * Created: 4/12/2018 11:23:35 AM
 *  Author: maral665
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "definitions.h"

float accel_MG_LSB = 0.001F;
float gravity_value = 9.821F;

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

float data_formater(uint8_t low, uint8_t high)
{
	//fattar inte varför de skiftar 4 steg åt höger...
	int16_t merged_data = (int16_t)(low | (high << 8)) >> 4;// kan behöva sätta int16_t på low och high
	//_delay_ms(10);

	float fromated_data = (float)merged_data * gravity_value * accel_MG_LSB;
	return fromated_data;
}

int main(void)
{
	volatile uint8_t x_l_value;
	volatile uint8_t x_h_value;
	volatile uint8_t y_l_value;
	volatile uint8_t y_h_value;
	volatile uint8_t z_l_value;
	volatile uint8_t z_h_value;
	
	volatile uint8_t ctrl_reg_data;
	volatile float data_x;
	volatile float data_y;
	volatile float data_z;
	
	DDRB = (1 << DDB0);
	PORTB = (0 << PORTB0);
	i2c_init();
	sei();
	/*-----------------------------------------------------------------
	We maybe need to look into CTRL_REG4_A(0x23) to adjust the sensitivity
	CTRL_REG2_A(0x21) configurate a HP-filter
	-----------------------------------------------------------------*/
	i2c_write_reg(ctrl_reg_1, set_ctrl_reg_1_100, 1);
	led_blinker(5);
	ctrl_reg_data = i2c_read_reg(ctrl_reg_1);
	while(1)
	{
		x_l_value = i2c_read_reg(acc_x_l_reg);
		_delay_ms(10);
		x_h_value = i2c_read_reg(acc_x_h_reg);
		//_delay_ms(1000);
		data_x = data_formater(x_l_value,x_h_value);
		
		y_l_value = i2c_read_reg(acc_y_l_reg);
		_delay_ms(10);
		y_h_value = i2c_read_reg(acc_y_h_reg);
		data_y = data_formater(y_l_value,y_h_value);
		
		z_l_value = i2c_read_reg(acc_z_l_reg);
		_delay_ms(10);
		z_h_value = i2c_read_reg(acc_z_h_reg);
		data_z = data_formater(z_l_value,z_h_value);
		_delay_ms(10);
		//Möjligtvis lägga in detta i avbrottsrutinen?
		//volatile int16_t data_x = shift_data(x_h_value, x_l_value);
		//volatile int16_t data_y = shift_data(y_h_value, y_l_value);
		//volatile int16_t data_z = shift_data(z_h_value, z_l_value);
		
	}
	return 0;
}