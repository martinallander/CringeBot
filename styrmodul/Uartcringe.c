/*
 * UART_styrmodul.c
 *
 * Created: 2018-03-21 08:44:45
 * Author : Ludviglol
 */ 

#ifndef F_CPU
#define F_CPU 8000000UL                    // Sätt CPU-klockan till 8 MHz
#endif

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#define BAUD 9600                           
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)    //Sätt baudrate för UBRR enligt formel i databladet
											//UBRR = Uart Baud Rate Register

//Initiera UART
void uart_init (void)
{
	UBRR0H = (unsigned char)(BAUDRATE>>8);			//Skifta registret åt höger med 8 bitar
	UBRR0L = (unsigned char)BAUDRATE;				//Sätt baudrate
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);					//Enable receiver and transmitter
	UCSR0C = (0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);	//Sätter 1 stoppbit, samt 8bit dataformat
}

//Funktion för att skicka data
void uart_transmit (unsigned char data)
{
	while(!( UCSR0A & (1<<UDRE0)));		//Väntar på att "transmit buffern" ska bli tom genom att kolla UDRE-flaggan
	UDR0 = data;                        //Ladda in datan i registret (UDR = Uart Data Register)
}

//Funktion för att ta emot data
unsigned char uart_recieve (void)
{
	while(!(UCSR0A & (1<<RXC0)));           //Väntar på att data ska finnas "receive buffern" genom att kolla RXCn-flaggan
	return UDR0;                             // return 8-bit data
}

//main
int main (void)
{
	//unsigned char a;
	uart_init();                            //Initiera UART
	while(1)
	{
		//a = uart_recieve();
		uart_transmit(0x00);
		_delay_ms(20);
		
	}
	return 0;
}