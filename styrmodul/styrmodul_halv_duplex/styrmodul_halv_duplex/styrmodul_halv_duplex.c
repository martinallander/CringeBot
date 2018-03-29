#ifndef F_CPU
#define F_CPU 16000000UL                    // Sätt CPU-klockan till 16 MHz
#endif

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>

// -- USART definitioner
#define TXD0_READY bit_is_set(UCSR0A,5)
#define TXD0_FINISHED bit_is_set(UCSR0A,6)
#define RXD0_READY bit_is_set(UCSR0A,7)
#define TXD0_DATA (UDR0)
#define RXD0_DATA (UDR0)

#define BAUD 1000000
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)    //Sätt baudrate för UBRR enligt formel i databladet


//Godtyckliga definitioner
#define REC_BUFFER_LEN 32
#define SERVO_MAX_PARAMS (REC_BUFFER_LEN - 5)

#define REC_WAIT_START_US    75
#define REC_WAIT_PARAMS_US   (SERVO_MAX_PARAMS * 5)
#define REC_WAIT_MAX_RETRIES 200

//uint8_t servoErrorCode = 0;

//ServoResponse response;

volatile uint8_t receiveBuffer[REC_BUFFER_LEN];
volatile uint8_t* volatile receiveBufferStart = receiveBuffer;
volatile uint8_t* volatile receiveBufferEnd = receiveBuffer;

//Olika kommandon för servona
typedef enum ServoCommand
{
	PING = 1,
	READ = 2,
	WRITE = 3,
	REG = 4,
	ACTION = 5
} ServoCommand;



//Adresser i EEPROM dit olika data skrivs
#define MAX_SPEED				   0x20
#define GOAL_ANGLE				   0x1e
#define TORQUE					   0x22
#define RETURN_LEVEL			   0x10 //När servot ska skicka tillbaka data
#define LOWER_ANGLE_LIMIT    	   0x06 //Nedre vinkelgräns
#define HIGHER_ANGLE_LIMIT         0x08 //Övre vinkelgräns

//Startvinklar då roboten står upp
#define STAND_DEG_00				150
#define STAND_DEG_03				150
#define STAND_DEG_06				150
#define STAND_DEG_09				150
#define STAND_DEG_12				150
#define STAND_DEG_15				150

#define STAND_DEG_01				 90
#define STAND_DEG_04				 90
#define STAND_DEG_07				 90
#define STAND_DEG_10				210
#define STAND_DEG_13				210
#define STAND_DEG_16				210

#define STAND_DEG_02				230
#define STAND_DEG_05				230
#define STAND_DEG_08				230
#define STAND_DEG_11				 70
#define STAND_DEG_14				 70
#define STAND_DEG_17				 70

/************************************************************************************************************
*******************************************  FUNKTIONER FÖR UART  *******************************************
************************************************************************************************************/

void UART0RecieveMode(void) 
{
	UCSR0B = (0<<TXEN0)|(1<<RXEN0);
    //PORTD = (0<<PORTD6);
}
/*
 * Styr tristatebuffern till sändningsläge med hjälp av portD4.
 */
void UART0SendMode(void)
{
	UCSR0B = (1<<TXEN0)|(0<<RXEN0);
    //PORTD = (1<<PORTD6);
}

//Initiera UART
void uart_init (void)
{
	UBRR0H = (unsigned char)(BAUDRATE>>8);			//Skifta registret åt höger med 8 bitar
	UBRR0L = (unsigned char)BAUDRATE;				//Sätt baudrate
	UCSR0B = (1<<TXEN0)|(0<<RXEN0);					//Enable receiver and transmitter
	UCSR0C = (0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01)|(0<<UPM01);	//Sätter 1 stoppbit, samt 8bit dataformat
	//DDRD = (1<<PORTD);								// Styrsignal för sändning/mottagning, PD2
	//DDRD |= 0x40;									//Sätter PD60 som utgång
	//PORTD = 0<<PORTD6;								//Skickar 0 på PORTD för att kunna skicka data
	UART0RecieveMode();
	 
}

//Skicka med UART
void uart_transmit (unsigned char data)
{
	//UCSR0B = (1<<TXEN0);
	while(!( UCSR0A & (1<<UDRE0)));		//Väntar på att "transmit buffern" ska bli tom genom att kolla UDRE-flaggan
	UDR0 = data;                        //Ladda in datan i registret (UDR = Uart Data Register)
}

//Ta emot med UART
unsigned char uart_recieve (void)
{
	//UCSR0B = (0<<TXEN0);
	while(!(UCSR0A & (1<<RXC0)));           //Väntar på att data ska finnas "receive buffern" genom att kolla RXCn-flaggan
	return UDR0;                             // return 8-bit data
}

void UARTSendInstruction(int ID, int instruction, int parameter0, int parameter1)
{
	// sätt USART till sändläge
	UART0SendMode();
	UCSR0A = UCSR0A | (0 << 6); // Gjorde så att vi kunde skicka en instruktion efter en instruktion/read.
	uart_transmit(0xFF);
	uart_transmit(0xFF);
	uart_transmit(ID);
	uart_transmit(4); // Paketets längd.
	uart_transmit(instruction);
	uart_transmit(parameter0);
	uart_transmit(parameter1);
	cli();
	uart_transmit(~(ID+4+instruction+parameter0+parameter1)); // Checksum
	while(!TXD0_FINISHED) //TXD0 sätts till 1 då all data shiftats ut ifrån usarten
	{
		// Vänta tills den sänt klart det sista
	}
	UART0RecieveMode();
	sei(); // Tillåt interrupts igen
	
}

uint8_t servo_read_status_packet()
{
	int ValueOfParameters = 0;
	unsigned char Start1 = uart_recieve();
	char Start2 = uart_recieve();
	uint8_t ID = uart_recieve();
	char Length = uart_recieve();
	char Error = uart_recieve();
	int HelpVariable = 0;
	// Läser av parametervärdena och sparar värdet i ValueOfParameters
	while (Length > 2)
	{
		ValueOfParameters = ValueOfParameters + (uart_recieve() << (8*HelpVariable));
		HelpVariable = HelpVariable + 1;
		Length = Length - 1;
	}
	char CheckSum = uart_recieve();
	return ID;
	//return ValueOfParameters;
	
}

/************************************************************************************************************
**************************************  FUNKTIONER FÖR GET-KOMMANDON  **************************************
************************************************************************************************************/

//hfgkjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjdgs

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

/*
unsigned char get_servo_data(const uint8_t servoId, const uint8_t dataSelect, const uint8_t dataLength)
{
	const uint8_t param[2] = {dataSelect, dataLength};
	send_servo_command(servoId, READ, 2, param);
	
	unsigned char startByte1 = uart_recieve();
	unsigned char startByte2 = uart_recieve();
	unsigned char ID = uart_recieve();
	unsigned char length = uart_recieve();
	unsigned char error = uart_recieve();
	uint8_t params[(int)length];
	for (uint8_t i = 0; i < (int)length; i++)
	{
		params[i] = uart_recieve();  //Parametrar
	}
	unsigned char checkSum = uart_recieve();
	return  startByte1;
	
	//param[1];
}
*/

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

/*
const float get_servo_angle (const uint8_t servoId)
{
	send_servo_command(servoId, READ, 2, const uint8_t param[2] = {GOAL_ANGLE, 0x02});
	const uint16_t angleValue = (uint16_t)(angle * (1023.0 / 300.0));
}*/

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
*******************************************  FUNKTIONER FÖR GÅNG  *******************************************
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
	reg_servo_angle(4, (STAND_DEG_04 + 10));
	reg_servo_angle(5, STAND_DEG_05);
}

void middle_left_forward_end(void)
{
	reg_servo_angle(3, (STAND_DEG_03 + 5));
	reg_servo_angle(4, (STAND_DEG_04 - 20));
	reg_servo_angle(5, (STAND_DEG_05 + 30)); //tidigare 230
}

//Funktioner för att förflytta det högra mellersta benet
void middle_right_forward_start(void)
{
	reg_servo_angle(12, (STAND_DEG_12 - 5));
	reg_servo_angle(13, (STAND_DEG_13 + 20));
	reg_servo_angle(14, STAND_DEG_14);
}

void middle_right_forward_end(void)
{
	reg_servo_angle(12, (STAND_DEG_12 + 5));
	reg_servo_angle(13, (STAND_DEG_13 - 10));
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
	reg_servo_angle(11, (STAND_DEG_11 - 5));
}

void back_right_forward_end(void)
{
	reg_servo_angle(9, (STAND_DEG_09 - 5));
	reg_servo_angle(10, (STAND_DEG_10 - 5));
	reg_servo_angle(11, (STAND_DEG_11 + 15));
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

/************************************************************************************************************
********************************************  ÖVRIGA FUNKTIONER  ********************************************
************************************************************************************************************/
void servo_init(void)
{
	set_servo_max_speed(0xfe, 100);
	set_servo_status_return_level(0xfe, 0x01); //Skicka bara return-packet när READ används
}

void led_blink (uint16_t i)
{
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
	led_blink(2);
	/*
	unsigned char data1 = 0;
	unsigned char data2 = 0;
	unsigned char data3 = 0;
	unsigned char data4 = 0;
	unsigned char data5 = 0;
	unsigned char data6 = 0;
	*/
	
	uart_init();
	//data = get_servo_data(0x01, GOAL_ANGLE, 0x02);
	//set_servo_status_return_level(0XFE, 0X02);
	set_servo_torque(0xfe, 1023);
	set_servo_max_speed(0xfe, 100);
	stand();
	_delay_ms(1000);
	
	/*
	data1 = uart_recieve();
	data2 = uart_recieve();
	data3 = uart_recieve();
	data4 = uart_recieve();
	data5 = uart_recieve();
	data6 = uart_recieve();
	led_blink(data1 + data2 + data3 + data4 + data5 + data6);
	*/
	
	while(1)
	{
		set_servo_angle(0x03, 200);
		unsigned char data = servo_read_status_packet();
		led_blink(data);
		_delay_ms(2000);
		set_servo_angle(0x03, 150);
		data = servo_read_status_packet();
		led_blink(data);
		_delay_ms(2000);
	}
	return 0;
}
