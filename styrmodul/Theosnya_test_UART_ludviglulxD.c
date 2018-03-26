#ifndef F_CPU
#define F_CPU 16000000UL                    // Sätt CPU-klockan till 16 MHz
#endif

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#define BAUD 1000000
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)    //Sätt baudrate för UBRR enligt formel i databladet


typedef enum ServoCommand
{
	PING = 1,
	READ = 2,
	WRITE = 3,
	REG = 4,
	ACTION = 5
} ServoCommand;

#define MAX_SPEED           0x20
#define GOAL_ANGLE          0x1e
#define TORQUE              0x22

void uart_init (void)
{
	UBRR0H = (unsigned char)(BAUDRATE>>8);			//Skifta registret åt höger med 8 bitar
	UBRR0L = (unsigned char)BAUDRATE;				//Sätt baudrate
	UCSR0B = (1<<TXEN0)|(0<<RXEN0);					//disable receiver and enable transmitter
	UCSR0C = (0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);	//Sätter 1 stoppbit, samt 8bit dataformat
}

void uart_transmit (unsigned char data)
{
	while(!( UCSR0A & (1<<UDRE0)));		//Väntar på att "transmit buffern" ska bli tom genom att kolla UDRE-flaggan
	UDR0 = data;                        //Ladda in datan i registret (UDR = Uart Data Register)
}

unsigned char uart_recieve (void)
{
	while(!(UCSR0A & (1<<RXC0)));           //Väntar på att data ska finnas "receive buffern" genom att kolla RXCn-flaggan
	return UDR0;                             // return 8-bit data
}

void send_servo_command (const uint8_t servoId, const ServoCommand commandByte, const uint8_t numParams, const uint8_t *params)
{
	uart_transmit(0xff);
	uart_transmit(0xff);  // command header
	
	uart_transmit(servoId);  // servo ID
	uint8_t checksum = servoId;
	
	uart_transmit(numParams + 2);  // number of following bytes
	uart_transmit((uint8_t)commandByte);  // command
	
	checksum += numParams + 2 + commandByte;
	
	for (uint8_t i = 0; i < numParams; i++)
	{
		uart_transmit(params[i]);  // parameters
		checksum += params[i];
	}
	
	uart_transmit(~checksum);  // checksum
}


void set_servo_angle (const uint8_t servoId, const float angle)
{
	//if (angle < 0 || angle > 300)
	//return false;
	
	// angle values go from 0 to 0x3ff (1023)
	const uint16_t angleValue = (uint16_t)(angle * (1023.0 / 300.0));
	
	const uint8_t highByte = (uint8_t)((angleValue >> 8) & 0xff);
	const uint8_t lowByte = (uint8_t)(angleValue & 0xff);
	
	const uint8_t params[3] = {GOAL_ANGLE, lowByte, highByte};
	
	send_servo_command (servoId, WRITE, 3, params);
}

void set_servo_max_speed (const uint8_t servoId, const uint16_t speedValue)
{
	const uint8_t highByte = (uint8_t)((speedValue >> 8) & 0xff);
	const uint8_t lowByte = (uint8_t)(speedValue & 0xff);
	
	//if (speedValue > 1023)
	//return false;
	
	const uint8_t params[3] = {MAX_SPEED, lowByte, highByte};
	
	send_servo_command(servoId, WRITE, 3, params);
	
	//return true;
}

void set_servo_torque (const uint8_t servoId,
const uint16_t torqueValue)
{
	const uint8_t highByte = (uint8_t)((torqueValue >> 8) & 0xff);
	const uint8_t lowByte = (uint8_t)(torqueValue & 0xff);
	
	//if (torqueValue > 1023)
	//return false;
	
	const uint8_t params[3] = {TORQUE, lowByte, highByte};
	
	send_servo_command(servoId, WRITE, 3, params);
	
	//if (!getAndCheckResponse (servoId))
	//return false;
	
	//return true;
}

void reg_servo_max_speed (const uint8_t servoId, const uint16_t speedValue)
{
	const uint8_t highByte = (uint8_t)((speedValue >> 8) & 0xff);
	const uint8_t lowByte = (uint8_t)(speedValue & 0xff);
	
	//if (speedValue > 1023)
	//return false;
	
	const uint8_t params[3] = {MAX_SPEED, lowByte, highByte};
	
	send_servo_command(servoId, REG, 3, params);
	
	//return true;
}

void reg_servo_angle (const uint8_t servoId, const float angle)
{
	//if (angle < 0 || angle > 300)
	//return false;
	
	// angle values go from 0 to 0x3ff (1023)
	const uint16_t angleValue = (uint16_t)(angle * (1023.0 / 300.0));
	
	const uint8_t highByte = (uint8_t)((angleValue >> 8) & 0xff);
	const uint8_t lowByte = (uint8_t)(angleValue & 0xff);
	
	const uint8_t params[3] = {GOAL_ANGLE, lowByte, highByte};
	
	send_servo_command (servoId, REG, 3, params);
}

void action(void)
{
	const uint8_t params[1] = {0};
	send_servo_command (0xfe, ACTION, 0, params);
}

void stand(void)
{
	//reg_servo_max_speed(0xFE, 100);
	//action();
	
	reg_servo_angle(0x00, 150);
	reg_servo_angle(0x03, 150);
	reg_servo_angle(0x06, 150);
	reg_servo_angle(0x09, 150);
	reg_servo_angle(0x0C, 150);
	reg_servo_angle(0x0F, 150);
	
	reg_servo_angle(0x01, 90);
	reg_servo_angle(0x04, 90);
	reg_servo_angle(0x07, 90);
	reg_servo_angle(0x0A, 210);
	reg_servo_angle(0x0D, 210);
	reg_servo_angle(0x10, 210);
	action();
	
	reg_servo_angle(0x02, 230);
	reg_servo_angle(0x0E, 70);
	reg_servo_angle(0x08, 230);
	reg_servo_angle(0x0B, 70);
	reg_servo_angle(0x011, 70);
	reg_servo_angle(0x05, 230);
	action();
	
	_delay_ms(500);
	/*
	set_servo_angle(0x00, 150);
	_delay_ms(800);
	set_servo_angle(0x03, 150);
	_delay_ms(800);
	set_servo_angle(0x06, 150);
	_delay_ms(800);
	set_servo_angle(0x09, 150);
	_delay_ms(800);
	set_servo_angle(0x0C, 150);
	_delay_ms(800);
	set_servo_angle(0x0F, 150);
	_delay_ms(800);
	
	set_servo_angle(0x01, 90);
	_delay_ms(800);
	set_servo_angle(0x04, 90);
	_delay_ms(800);
	set_servo_angle(0x07, 90);
	_delay_ms(800);
	set_servo_angle(0x0A, 210);
	_delay_ms(800);
	set_servo_angle(0x0D, 210);
	_delay_ms(800);
	set_servo_angle(0x10, 210);
	_delay_ms(800);
	
	set_servo_angle(0x02, 240);
	_delay_ms(800);
	set_servo_angle(0x0E, 60);
	_delay_ms(800);
	set_servo_angle(0x08, 240);
	_delay_ms(800);
	set_servo_angle(0x0B, 60);
	_delay_ms(800);
	set_servo_angle(0x11, 60);
	_delay_ms(800);
	set_servo_angle(0x05, 240);
	_delay_ms(800);
	*/
}

//Funktioner för att förflytta det vänstra främre benet
void front_left_forward_start(void)
{
	reg_servo_angle(0, 155);
	reg_servo_angle(1, 70); // 80 innan
	reg_servo_angle(2, 260);
}

void front_left_forward_end(void)
{
	reg_servo_angle(0, 145);
	reg_servo_angle(1, 100);
	reg_servo_angle(2, 220);
}

//Funktioner för att förflytta det vänstra mellersta benet
void middle_left_forward_start(void)
{
	reg_servo_angle(3, 145);
	reg_servo_angle(4, 100);
	reg_servo_angle(5, 230);
}

void middle_left_forward_end(void)
{
	reg_servo_angle(3, 155);
	reg_servo_angle(4, 70);
	reg_servo_angle(5, 260); //tidigare 230
}

//Funktioner för att förflytta det högra vänstra benet
void back_left_forward_start(void)
{
	reg_servo_angle(6, 155);
	reg_servo_angle(7, 100);
	reg_servo_angle(8, 220);
}

void back_left_forward_end(void)
{
	reg_servo_angle(6, 145);
	reg_servo_angle(7, 80);
	reg_servo_angle(8, 240);
}

//Funktioner för att förflytta det högra främre benet
void front_right_forward_start(void)
{
	reg_servo_angle(15, 155);
	reg_servo_angle(16, 200);
	reg_servo_angle(17, 80);
}

void front_right_forward_end(void)
{
	reg_servo_angle(15, 145);
	reg_servo_angle(16, 230);
	reg_servo_angle(17, 40);
}

//Funktioner för att förflytta det högra mellersta benet
void middle_right_forward_start(void)
{
	reg_servo_angle(12, 145);
	reg_servo_angle(13, 230);
	reg_servo_angle(14, 60);
}

void middle_right_forward_end(void)
{
	reg_servo_angle(12, 155);
	reg_servo_angle(13, 200);
	reg_servo_angle(14, 80);
}

//Funktioner för att förflytta det högra bakre benet
void back_right_forward_start(void)
{
	reg_servo_angle(9, 155);
	reg_servo_angle(10, 230);
	reg_servo_angle(11, 60);
}

void back_right_forward_end(void)
{
	reg_servo_angle(9, 145);
	reg_servo_angle(10, 200);
	reg_servo_angle(11, 80);
}

void theos_forward_walk_start(void)
{
	back_left_forward_start();
	middle_right_forward_start();
	front_left_forward_start();
	
	back_right_forward_start();
	middle_left_forward_start();
	front_right_forward_start();
}

void theos_forward_walk_end(void)
{
	back_left_forward_end();
	middle_right_forward_end();
	front_left_forward_end();
	
	back_right_forward_end();
	middle_left_forward_end();
	front_right_forward_end();
}






void walk(void)
{
	//reg_servo_max_speed(0xFE, 400);
	//action();
	_delay_ms(500);
	
	//Stå: 150, (90,210), (240,60)
	//Lyft: 13,7,1		Rotera: 12,6,0
	//Lyft: 16,10,4		Rotera: 15,9,3
	
	reg_servo_angle(0x08, 230);
	reg_servo_angle(0x02, 230);
	
	//Lyft 13,7,1
	reg_servo_angle(0x01, 60);
	reg_servo_angle(0x07, 60);
	reg_servo_angle(0x0D, 240);
	action();
	_delay_ms(1500);
	
	//Rotera 12,6,0
	reg_servo_angle(0x00, 130);
	reg_servo_angle(0x06, 130);
	reg_servo_angle(0x0C, 170);
	//Rotera 3,15,9
	reg_servo_angle(0x03, 170);
	reg_servo_angle(0x0F, 130);
	reg_servo_angle(0x09, 130);
	
	//Tryck ifrån 11
	reg_servo_angle(0x0B, 80);
	//Skopa 17
	reg_servo_angle(0x11, 50);
	action();
	_delay_ms(1500);
	
	//Ställ ned 13,7,1
	reg_servo_angle(0x01, 90);
	reg_servo_angle(0x07, 90);
	reg_servo_angle(0x0D, 210);
	action();
	_delay_ms(1500);
	
	//Lyft 16,10,4
	reg_servo_angle(0x10, 240);
	reg_servo_angle(0x0A, 240);
	reg_servo_angle(0x04, 60);
	
	reg_servo_angle(0x0B, 70);
	reg_servo_angle(0x11, 70);
	action();
	_delay_ms(1500);
	 
	//Rotera alla
	reg_servo_angle(0x00, 150);
	reg_servo_angle(0x06, 150);
	reg_servo_angle(0x0C, 150);
	reg_servo_angle(0x03, 150);
	reg_servo_angle(0x0F, 150);
	reg_servo_angle(0x09, 150);
	//Tryck ifrån 8
	reg_servo_angle(0x08, 220);
	//Skopa 2
	reg_servo_angle(0x02, 250);
	action();
	_delay_ms(1500);

	//Ställ ned 16,10,4
	reg_servo_angle(0x10, 210);
	reg_servo_angle(0x0A, 210);
	reg_servo_angle(0x04, 90);
	action();
	_delay_ms(1500);
}

int main (void)
{
	uart_init();
	//set_servo_torque(0xfe, 1023);
	set_servo_max_speed(0xfe, 100);
	stand();
	_delay_ms(2000);
	
	while(1)
	{	/*
		middle_left_forward_start();
		front_right_forward_start();
		back_right_forward_start();
		action();
		_delay_ms(500);
		middle_left_forward_end();
		front_right_forward_end();
		back_right_forward_end();
		action();
		_delay_ms(500);*/
		theos_forward_walk_start();
		action();
		_delay_ms(500);
		theos_forward_walk_end();
		action();
		_delay_ms(500);
	}
	return 0;
}