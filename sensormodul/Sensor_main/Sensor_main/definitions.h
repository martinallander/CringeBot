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

//Accelerometer registers
#define accel_addr 0x32

#define ctrl_reg_1 0x20
#define acc_x_l 0x28
#define acc_x_h 0x29
#define acc_y_l 0x2A
#define acc_y_h 0x2B
#define acc_z_l 0x2C
#define acc_z_h 0x2D
#define set_ctrl_1_10 = 0b00100111 //10Hz
#define set_ctrl_1_50 0b01000111 //50Hz
#define set_ctrl_1_100 0b01010111 //100Hz

//IR cam registers
//#define temp_addr 0b1101000
#define temp_addr 0xD2

#define start_pixel 0x81 //0x80 + 1
#define end_pixel 0xFF
#define set_frame_rate 0x02 