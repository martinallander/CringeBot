#ifndef F_CPU
#define F_CPU 16000000UL                    // Sätt CPU-klockan till 16 MHz
#endif

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include "main.h"
#include "alfred_monkaS.h"

// -- USART definitioner
#define TXD0_READY bit_is_set(UCSR0A,5)
#define TXD0_FINISHED bit_is_set(UCSR0A,6)
#define RXD0_READY bit_is_set(UCSR0A,7)
#define TXD0_DATA (UDR0)
#define RXD0_DATA (UDR0)

#define BAUD 1000000
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)    //Sätt baudrate för UBRR enligt formel i databladet

//Olika kommandon för servona
typedef enum ServoCommand
{
	PING = 1,
	READ = 2,
	WRITE = 3,
	REG = 4,
	ACTION = 5
} ServoCommand;

/************************************************************************************************************
*******************************************  FUNKTIONER FÖR UART  *******************************************
************************************************************************************************************/

void UART0RecieveMode(void) 
{
	UCSR0B = (0<<TXEN0)|(1<<RXEN0);
}

void UART0SendMode(void)
{
	UCSR0B = (1<<TXEN0)|(0<<RXEN0);
}

//Initiera UART
void uart_init (void)
{
	UBRR0H = (unsigned char)(BAUDRATE>>8);			//Skifta registret åt höger med 8 bitar
	UBRR0L = (unsigned char)BAUDRATE;				//Sätt baudrate
	//UCSR0B = (1<<TXEN0)|(1<<RXEN0);					//Enable receiver and transmitter
	UCSR0C = (0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01)|(0<<UPM01);	//Sätter 1 stoppbit, samt 8bit dataformat
	UART0RecieveMode();	 
}

//Skicka med UART
void uart_transmit (unsigned char data)
{
	while(!( UCSR0A & (1<<UDRE0)));		//Väntar på att "transmit-buffern" ska bli tom genom att kolla UDRE-flaggan
	UDR0 = data;                        //Ladda in datan i registret (UDR = Uart Data Register)
}

//Ta emot med UART
unsigned char uart_recieve (void)
{
	while(!(UCSR0A & (1<<RXC0)));           //Väntar på att data ska finnas i "receive-buffern" genom att kolla RXCn-flaggan
	return UDR0;                             // return 8-bit data
}

/************************************************************************************************************
**************************************  FUNKTIONER FÖR GET-KOMMANDON  **************************************
************************************************************************************************************/

uint16_t servo_read_status_packet()
{
	uint16_t ValueOfParameters = 0;
	uint8_t Start1 = uart_recieve();
	uint8_t Start2 = uart_recieve();
	uint8_t ID = uart_recieve();
	uint8_t Length = uart_recieve();
	uint8_t Error = uart_recieve();
	uint8_t HelpVariable = 0;
	//Läser av parametervärdena och sparar värdet i ValueOfParameters
	while (Length > 2)
	{
		ValueOfParameters = ValueOfParameters + (uart_recieve() << (8*HelpVariable));
		HelpVariable++;
		Length--;
	}
	
	//uint8_t	data1 = uart_recieve();
	//uint8_t	data2 = uart_recieve();
	//uint16_t data = data1 + data2*256;
	uint8_t CheckSum = uart_recieve();
	led_blink(1);
	return ValueOfParameters;//*(uint16_t)300/1023;
}


/************************************************************************************************************
**************************************  FUNKTIONER FÖR SET-KOMMANDON  **************************************
************************************************************************************************************/
//Skicka ett kommando till servon
void send_servo_command (const uint8_t servoId, const ServoCommand commandByte, const uint8_t numParams, const uint8_t *params)
{
	UART0SendMode();
	UCSR0A = UCSR0A | (0 << 6); // Gjorde så att vi kunde skicka en instruktion efter en instruktion/read.
	uart_transmit(0xff);
	uart_transmit(0xff);  // 2 stycken startbytes
	
	uart_transmit(servoId);  // servo-ID
	uint8_t checksum = servoId;
	
	uart_transmit(numParams + 2);  //Antal parametrar + 2
	uart_transmit((uint8_t)commandByte);  //Kommandot
	checksum += numParams + 2 + commandByte;
	
	for (uint8_t i = 0; i < numParams; i++)
	{
		uart_transmit(params[i]);  //Parametrar
		checksum += params[i];
	}
	cli(); // Blockera avbrott
	uart_transmit(~checksum);  // checksum
	while(!TXD0_FINISHED) //TXD0 sätts till 1 då all data shiftats ut ifrån usarten
	{
		// Vänta tills den sänt klart det sista
	}
	UART0RecieveMode();
	sei(); // Tillåt interrupts igen
}

//Sätt vinkel för servon
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

//Välj när ett servo ska skicka tillbaka data.
void set_servo_status_return_level (const uint8_t servoId, const uint8_t returnlevel)
{
	/*  0 Do not respond to any instructions
		1 Respond only to READ_DATA instructions
		2 Respond to all instructions	*/
	const uint8_t params[2] = {RETURN_LEVEL, returnlevel};
	send_servo_command (servoId, WRITE, 2, params);
}

void set_servo_max_speed (const uint8_t servoId, const uint16_t speedValue)
{
	const uint8_t highByte = (uint8_t)((speedValue >> 8) & 0xff);
	const uint8_t lowByte = (uint8_t)(speedValue & 0xff);
	const uint8_t params[3] = {MAX_SPEED, lowByte, highByte};
	send_servo_command(servoId, WRITE, 3, params);
}

//Sätter gräns för tillåten belastning
void set_servo_torque (const uint8_t servoId,
const uint16_t torqueValue)
{
	const uint8_t highByte = (uint8_t)((torqueValue >> 8) & 0xff);
	const uint8_t lowByte = (uint8_t)(torqueValue & 0xff);
	const uint8_t params[3] = {TORQUE, lowByte, highByte};
	send_servo_command(servoId, WRITE, 3, params);
	//if (!getAndCheckResponse (servoId))
	//return false;
}

//Sätter gränser för tillåtna servovinklar
void set_servo_angle_limit (const uint8_t servoId, const uint16_t lowerLimit, 
							const uint16_t higherLimit)
{
	const uint8_t highByte1 = (uint8_t)((lowerLimit >> 8) & 0xff);
	const uint8_t lowByte1 = (uint8_t)(lowerLimit & 0xff);
	const uint8_t params1[3] = {LOWER_ANGLE_LIMIT, lowByte1, highByte1};
	send_servo_command(servoId, WRITE, 3, params1);

	const uint8_t highByte2 = (uint8_t)((higherLimit >> 8) & 0xff);
	const uint8_t lowByte2 = (uint8_t)(higherLimit & 0xff);
	const uint8_t params2[3] = {HIGHER_ANGLE_LIMIT, lowByte2, highByte2};
	send_servo_command(servoId, WRITE, 3, params2);
}

//Laddar ett servo med en vinkel som sedan utförs med "action();"
void reg_servo_angle (const uint8_t servoId, const float angle)
{
	const uint16_t angleValue = (uint16_t)(angle * (1023.0 / 300.0));
	const uint8_t highByte = (uint8_t)((angleValue >> 8) & 0xff);
	const uint8_t lowByte = (uint8_t)(angleValue & 0xff);
	const uint8_t params[3] = {GOAL_ANGLE, lowByte, highByte};
	send_servo_command (servoId, REG, 3, params);
}

//Utför de REG_WRITE-instruktioner som servona har laddats med
void action(void)
{
	const uint8_t params[1] = {0};
	send_servo_command (0xfe, ACTION, 0, params);
}

/************************************************************************************************************
*******************************************  INITIERING  ***************************************************
************************************************************************************************************/

void stand(void)
{
	reg_servo_angle(0, STAND_DEG_00);
	reg_servo_angle(3, STAND_DEG_03);
	reg_servo_angle(6, STAND_DEG_06);
	reg_servo_angle(9, STAND_DEG_09);
	reg_servo_angle(12, STAND_DEG_12);
	reg_servo_angle(15, STAND_DEG_15);
	
	reg_servo_angle(1, STAND_DEG_01);
	reg_servo_angle(4, STAND_DEG_04);
	reg_servo_angle(7, STAND_DEG_07);
	reg_servo_angle(10, STAND_DEG_10);
	reg_servo_angle(13, STAND_DEG_13);
	reg_servo_angle(16, STAND_DEG_16);
	action();
	
	reg_servo_angle(2, STAND_DEG_02);
	reg_servo_angle(5, STAND_DEG_05);
	reg_servo_angle(8, STAND_DEG_08);
	reg_servo_angle(11, STAND_DEG_11);
	reg_servo_angle(14, STAND_DEG_14);
	reg_servo_angle(17, STAND_DEG_17);
	action();
}

/************************************************************************************************************
*******************************************  FUNKTIONER FÖR ENKSKILDA BEN ***********************************
************************************************************************************************************/

//Funktioner för att förflytta det vänstra främre benet
void front_left_forward_start(void)
{
	reg_servo_angle(0, (STAND_DEG_00 + 5));
	reg_servo_angle(1, (STAND_DEG_01 - 20));
	reg_servo_angle(2, (STAND_DEG_02 + 30));
}

void front_left_forward_end(void)
{
	reg_servo_angle(0, (STAND_DEG_00 - 5));
	reg_servo_angle(1, (STAND_DEG_01 + 60));
	reg_servo_angle(2, (STAND_DEG_02 - 10));
}


//Funktioner för att förflytta det högra främre benet
void front_right_forward_start(void)
{
	reg_servo_angle(15, (STAND_DEG_15 + 5));
	reg_servo_angle(16, (STAND_DEG_16 - 20));
	reg_servo_angle(17, (STAND_DEG_17 + 30));
}
void front_right_forward_end(void)
{
	reg_servo_angle(15, (STAND_DEG_15 - 5));
	reg_servo_angle(16, (STAND_DEG_16 + 60));
	reg_servo_angle(17, (STAND_DEG_17 - 10));
}

//Funktioner för att förflytta det vänstra mellersta benet
void middle_left_forward_start(void)
{
	reg_servo_angle(3, (STAND_DEG_03 - 5));
	reg_servo_angle(4, (STAND_DEG_04 + 15));
	reg_servo_angle(5, STAND_DEG_05);
}

void middle_left_forward_end(void)
{
	reg_servo_angle(3, (STAND_DEG_03 + 5));
	reg_servo_angle(4, (STAND_DEG_04 - 15));
	reg_servo_angle(5, (STAND_DEG_05 + 30)); //tidigare 230
}

//Funktioner för att förflytta det högra mellersta benet
void middle_right_forward_start(void)
{
	reg_servo_angle(12, (STAND_DEG_12 - 5));
	reg_servo_angle(13, (STAND_DEG_13 + 15));
	reg_servo_angle(14, STAND_DEG_14);
}

void middle_right_forward_end(void)
{
	reg_servo_angle(12, (STAND_DEG_12 + 5));
	reg_servo_angle(13, (STAND_DEG_13 - 15));
	reg_servo_angle(14, (STAND_DEG_14 + 30));
}

//Funktioner för att förflytta det bakre vänstra benet
void back_left_forward_start(void)
{
	reg_servo_angle(6, (STAND_DEG_06 + 5));
	reg_servo_angle(7, (STAND_DEG_07 + 20));
	reg_servo_angle(8, (STAND_DEG_08 - 10));
}

void back_left_forward_end(void)
{
	reg_servo_angle(6, (STAND_DEG_06 - 5));
	reg_servo_angle(7, (STAND_DEG_07 - 5));
	reg_servo_angle(8, (STAND_DEG_08 + 10));
}

//Funktioner för att förflytta det högra bakre benet
void back_right_forward_start(void)
{
	reg_servo_angle(9, (STAND_DEG_09 + 5));
	reg_servo_angle(10, (STAND_DEG_10 + 20));
	reg_servo_angle(11, (STAND_DEG_11 - 10));
}

void back_right_forward_end(void)
{
	reg_servo_angle(9, (STAND_DEG_09 - 5));
	reg_servo_angle(10, (STAND_DEG_10 - 5));
	reg_servo_angle(11, (STAND_DEG_11 + 10));
}

/************************************************************************************************************
*******************************************  HÅRDKODAD GÅNG  ***********************************************
************************************************************************************************************/

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

/************************************************************************************************************
********************************************  ÖVRIGA FUNKTIONER  ********************************************
************************************************************************************************************/
void servo_init(void)
{
	set_servo_max_speed(0xfe, 100);
	set_servo_status_return_level(0xfe, 0x01); //Skicka bara return-packet när READ används
}

void led_blink(uint16_t i)
{
	if (i< 0)
	return;
	//Blink LED "i" number of times
	for (; i>0; --i)
	{
		PORTB|=(1<<1);
		_delay_ms(200);
		PORTB=(0<<1);
		_delay_ms(200);
	}
}

int main (void)
{
	DDRB |= 0x03;
	led_blink(3);
	uart_init();
	LCD_Init();

	set_servo_status_return_level(0xfe, 0x01);
	set_servo_torque(0xfe, 1023);
	set_servo_max_speed(0xfe, 130);
	_delay_ms(2000);
	
	const uint8_t params[2] = {PRESENT_ANGLE, 0x02};

	char *lcd_value = "0";
	uint16_t return_packet;
	LCD_Clear();
	LCD_String("Standing up");
	stand();
	_delay_ms(3000);
	LCD_Clear();
	LCD_String("Walking");
	
	
	while(1)
	{
		//set_servo_angle(0x06, 80);
		//_delay_ms(2000);
		theos_forward_walk_start();
		action();
		_delay_ms(500);
		theos_forward_walk_end();
		action();
		_delay_ms(500);
		
		//for(int i = 0; i <= 125; i+=5)
		//{
			//set_servo_angle(0x06, (80 + i));
			//_delay_ms(50);
			//send_servo_command (0x06, READ, 2, params);
			//return_packet = servo_read_status_packet();
			//_delay_ms(30);				
			//LCD_Clear();
			//LCD_String("PRESENT ANGLE:");
			//LCD_Command(0xc0);
			//itoa(return_packet*0.293255, lcd_value, 10);						//Konvertera return_packet till string. Spara i lcd_value. Talbas 10
			//LCD_String(lcd_value);									// Write string on 2nd line
			//_delay_ms(140);
		//}
	}
	return 0;
}



//theos_forward_walk_start();
//action();
//_delay_ms(500);
//theos_forward_walk_end();
//action();
//_delay_ms(500);