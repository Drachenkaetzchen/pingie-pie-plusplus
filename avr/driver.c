#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "driver.h"
#include "bit.h"


void setup_gpio (void) {
	// Setup the AVR GPIO access methods
	DDRC |= (1 << LINE5) | (1 << LINE6) | (1 << LINE7) | (1 << LINE9);

	// Initialize all GPIO lines to low
	PORTC &= ~(1 << LINE5);
	PORTC &= ~(1 << LINE6);
	PORTC &= ~(1 << LINE7);
	PORTC &= ~(1 << LINE9);

}

/**
 * Setup the display communication, GPIO, timers etc.
 */
void setup_display (void) {
	setup_gpio();
	init_refresh_timer();
}

/**
 * Delays the transmission by the specified microseconds
 */
void _transmissionDelay (uint32_t usec) {

}



/**
 * Initializes a timer which runs clock_rate / 1024
 */
void init_refresh_timer (void) {
	TCCR0A = 0b00000010;		// |COM0A1|COM0A0|COM0B1|COM0B0|0|0|WGM01|WGM00|
						// COMA and COMB are set to normal,  OC0A and OC0B are disconnected respectively
						// WGM is set to CTC - Clear Timer on Compare (of OCR0A) Mode#2 = 010

		TCCR0B = 0b00000000;		// |FOC0A|FOC0B|0|0|WGM02|CS02|CS01|CS00|
						// FOC set to default - WGM02 set to 0 - Clock set to off

		TIMSK0 = 0b00000110;		// |0|0|0|0|0|OCIE0B|OCIE0A|TOIE0|
						// Timer Output Compare Match Interrupt A and B are enabled

		TIFR0  = 0b00000000;		// |0|0|0|0|0|OCFOB|OCF0A|TOV0|
						// Flags are set to 0 - Default settings

						// IE If running at 1 mhz (1000000 ticks per second), and selection set to 1024.
						// Duration of one timer tick - 1024/1000000 sec = 0.001024
		OCR0A = 244;			// Set to trip every 0.249856 sec (244 * 0.001024) Which is about 0.25sec, 1/4 of a second.
		OCR0B = 122;
		sei();
		TCCR0B = 0b00000100;
}

/**
 * Refresh timer
 */
ISR(TIMER0_COMPA_vect)
{
	PORTC |= (1 << LINE5);
}

ISR(TIMER0_COMPB_vect)
{
	PORTC &= ~(1 << LINE5);
}

/**_
 * Sends one individual pixel cycle.
 */
void send_pixel (bool set) {
	PORTC |= (1 << LINE7);
	if (set) {
		PORTC |= (1 << LINE9);
	} else {
		PORTC &= ~(1 << LINE9);
	}

	_delay_us(1);
	PORTC &= ~(1 << LINE7);
	_delay_us(1);
}

/**
 * Sends the spacer between the pixel blocks.
 *
 * @param endFullBlock true if it's the last block prior bank switching, false otherwise
 * @param odd true If odd rows needs to be filled, false otherwise.
 */
void send_pixel_block_spacer (bool endFullBlock, bool odd) {
	send_pixel(false);
	send_pixel(false);

	if (odd) {
		send_pixel(true);
	} else {
		send_pixel(false);
	}

	send_pixel(false);
	send_pixel(false);
	send_pixel(true);
	send_pixel(false);
	send_pixel(false);
	send_pixel(false);
	send_pixel(false);

	if (endFullBlock) {
		PORTC |= (1 << LINE6);
		_delay_us(2);
		PORTC &= ~(1 << LINE6);
		_delay_us(2);
	}
}
