/*
 * Sensor_main.c
 *
 * Created: 4/12/2018 11:23:35 AM
 *  Author: maral665
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdio.h>
#include "i2c.c"
#include "definitions.h"
#include "FIFO_Queue.c"
#include "sensor_data.c"

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

int16_t shift_data(int8_t high, int8_t low) //high och low är båda 2-komlement
{
	int16_t data = 0;
	int16_t shifted = (uint16_t)high << 8;
	data = (shifted | (uint16_t)low);
	return data;
}

int main(void)
{
	/*
	led_blinker(10);
   	volatile int8_t x_l_value;
	volatile int8_t x_h_value;
	volatile int8_t y_l_value;
	volatile int8_t y_h_value;
	volatile int8_t z_l_value;
	volatile int8_t z_h_value;
	DDRB = (1 << DDB0);
	PORTB = (0 << PORTB0);
	i2c_init();
	sei();
	i2c_write_reg(accel_addr, ctrl_reg_1, set_ctrl_reg_1_50, 1);
	while(1)
	{
		//led_blinker(1);
		_delay_ms(10);
		x_l_value = i2c_read_reg(accel_addr, acc_x_l_reg);
		x_h_value = i2c_read_reg(accel_addr, acc_x_h_reg);
		y_l_value = i2c_read_reg(accel_addr, acc_y_l_reg);
		y_h_value = i2c_read_reg(accel_addr, acc_y_h_reg);
		z_l_value = i2c_read_reg(accel_addr, acc_z_l_reg);
		z_h_value = i2c_read_reg(accel_addr, acc_z_h_reg);
		_delay_ms(10);
		volatile int16_t data_x = shift_data(x_h_value, x_l_value);
		volatile int16_t data_y = shift_data(y_h_value, y_l_value);
		volatile int16_t data_z = shift_data(z_h_value, z_l_value);
		
	}*/
	FIFO_test();
	return 0;
}