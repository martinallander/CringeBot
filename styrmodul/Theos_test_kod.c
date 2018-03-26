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
	WRITE = 3
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


int main (void)
{
	uart_init();
	set_servo_max_speed(0xfe, 1023);                            
	while(1)
	{	set_servo_angle(0xfe, 120);
		_delay_ms(20);
		set_servo_angle(0xfe, 30);
		_delay_ms(20);
	}
	return 0;
}