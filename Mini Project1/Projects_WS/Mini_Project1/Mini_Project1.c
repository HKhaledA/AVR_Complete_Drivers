/*
 * Mini_Project1.c
 *
 *  Created on: Jun 19, 2017
 *  Author: Mohamed Tarek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// variable to count the number of timer ticks (compare match)
unsigned char g_tick = 0;

// flag will be set when the timer count 1 second
unsigned char count_second_flag = 0;

// variables to hold the clock time
unsigned char seconds_count = 0;
unsigned char minutes_count = 0;
unsigned char hours_count = 0;

// Interrupt Service Routine for timer1 compare mode channel A
ISR(TIMER1_COMPA_vect)
{
	count_second_flag = 1;
}

void timer1_init_CTC_mode(unsigned short tick)
{
	TCNT1 = 0; //timer initial value
	OCR1A  = tick; //compare value
	TIMSK |= (1<<OCIE1A); //enable compare interrupt for channel A
	/* Configure timer1 control registers
	 * 1. Non PWM mode FOC1A=1 and FOC1B=1
	 * 2. No need for OC1A & OC1B in this example so COM1A0=0 & COM1A1=0 & COM1B0=0 & COM1B1=0
	 * 3. CTC Mode and compare value in OCR1A WGM10=0 & WGM11=0 & WGM12=1 & WGM13=0
	 */
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
	/*
	 * 4. Clock = F_CPU/1024 CS10=1 CS11=0 CS12=1
	 */
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
}

// External INT0 Interrupt Service Routine
ISR(INT0_vect)
{
	seconds_count = 0;
	minutes_count = 0;
	hours_count = 0;
}

// External INT0 enable and configuration function
void INT0_Init(void)
{
	// Enable external interrupt pin INT0
	GICR  |= (1<<INT0);
	// Trigger INT0 with the falling edge
	MCUCR &= ~(1<<ISC00);
	MCUCR |= (1<<ISC01);
}

int main(void)
{
	// configure INT0/PD2 as input pin
	DDRD  &= (~(1<<PD2));
	// enable internal pull up resistor at INT0/PD2 pin
	PORTD |= (1<<PD2);

	// configure first 6 pins in PORTA as output pins
    DDRA |= 0x3F;

    // configure first four pins of PORTC as output pins
	DDRC |= 0x0F;

	// initialize all the 7-segment with zero value
	PORTA |= 0x3F;
	PORTC &= 0xF0;

	// enable global interrupts in MC.
	SREG  |= (1<<7);

	// start timer1 to generate compare interrupt every 1000 MiliSeconds(1 Second)
	timer1_init_CTC_mode(1000);

	// activate external interrupt INT0
	INT0_Init();

    while(1)
    {
    	if(count_second_flag == 1)
    	{
    		//enter here every one second
    		//increment seconds count
    		seconds_count++;

    		if(seconds_count == 60)
    		{
    			seconds_count = 0;
    			minutes_count++;
    		}
    		if(minutes_count == 60)
    		{
    			minutes_count = 0;
    			hours_count++;
    		}
    		if(hours_count == 24)
    		{
    			hours_count = 0;
    		}
    	    // reset the flag again
    	    count_second_flag = 0;
    	}
    	else
    	{
    		// out the number of seconds
    		PORTA = (PORTA & 0xC0) | 0x01;
    		PORTC = (PORTC & 0xF0) | (seconds_count%10);

    		// make small delay to see the changes in the 7-segment
    		// 2Miliseconds delay will not effect the seconds count
    		_delay_ms(2);

    		PORTA = (PORTA & 0xC0) | 0x02;
    		PORTC = (PORTC & 0xF0) | (seconds_count/10);

    		_delay_ms(2);

    		// out the number of minutes
    		PORTA = (PORTA & 0xC0) | 0x04;
    		PORTC = (PORTC & 0xF0) | (minutes_count%10);

    		_delay_ms(2);

    		PORTA = (PORTA & 0xC0) | 0x08;
    	    PORTC = (PORTC & 0xF0) | (minutes_count/10);

    	    _delay_ms(2);

    	    // out the number of hours
    	    PORTA = (PORTA & 0xC0) | 0x10;
    	    PORTC = (PORTC & 0xF0) | (hours_count%10);

    	    _delay_ms(2);

    	    PORTA = (PORTA & 0xC0) | 0x20;
    	    PORTC = (PORTC & 0xF0) | (hours_count/10);

    	    _delay_ms(2);
    	}
    }
}
