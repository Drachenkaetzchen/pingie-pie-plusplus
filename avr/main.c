/*
 * main.c
 *
 *  Created on: Jun 24, 2012
 *      Author: felicitus
 */

#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include "driver.h"
#include "bit.h"


#define uniq(LOW,HEIGHT)        ((HEIGHT << 8)|LOW)                     // 2x 8Bit      --> 16Bit
#define LOW_BYTE(x)             (x & 0xff)                                      // 16Bit        --> 8Bit
#define HIGH_BYTE(x)            ((x >> 8) & 0xff)                       // 16Bit        --> 8Bit

#define sbi(ADDRESS,BIT)        ((ADDRESS) |= (1<<(BIT)))       // set Bit
#define cbi(ADDRESS,BIT)        ((ADDRESS) &= ~(1<<(BIT)))      // clear Bit
#define toggle(ADDRESS,BIT)     ((ADDRESS) ^= (1<<BIT))         // Bit umschalten

#define bis(ADDRESS,BIT)        (ADDRESS & (1<<BIT))            // bit is set?
#define bic(ADDRESS,BIT)        (!(ADDRESS & (1<<BIT)))         // bit is clear?

char* volatile transfer_pixmap;
char* volatile recv_pixmap;
char* volatile pixmap;

uint16_t volatile write_address = 0;

void push_foo(void);
void pixmap_to_strange_pixmap(void);
int get_address_for_x_y (int x, int y);
void clear (void);
void setpixel (int x, int y);
void checkerboard (bool offset);

#define BUFFER_SIZE (DISPLAY_WIDTH*DISPLAY_HEIGHT)/8

int main(void) {
	int i;
	DDRB |= (1 << PB0);
	PORTB &= ~(1 << PB0);
	PORTD = 0;

	DDRB |= (1 << PB3);

	sbi(DDRB, PB3);
	sbi(DDRC, PC4);

	cli();
	setup_transfer_irq();
	setup_display();
	pixmap = (char*)malloc(BUFFER_SIZE);
	transfer_pixmap = (char*)malloc(BUFFER_SIZE);
	recv_pixmap = (char*)malloc(BUFFER_SIZE);

	memset(pixmap, 0, BUFFER_SIZE);
	memset(transfer_pixmap, 0, BUFFER_SIZE);
	memset(recv_pixmap, 0, BUFFER_SIZE);

	write_address = 0;

	int x, y;

	x=0;
	y=5;

	while (1) {
		//clear();
		//memcpy(rxbuffer, pixmap, 20);
		//memcpy(rxbuffer, pixmap+(DISPLAY_WIDTH*10), 20);
		//setpixel(x,y);

		x++;
		if (x==DISPLAY_WIDTH) {
			x=0;
			y++;
		}

		if (y==DISPLAY_HEIGHT) {
			y=0;
		}

		pixmap_to_strange_pixmap();
		push_foo();
	}

}

void setup_transfer_irq () {
	sbi(PCICR, PCIE0);
	sbi(PCICR, PCIE1);
	sbi(PCMSK0, PCINT2);
	sbi(PCMSK1, PCINT12);
}

/**
 * Checks if the PS-Pin is pulled high; if it is, copy the receive pixmap to the
 * pixmap to be displayed (pointer flip). Also resets the write address.
 */
ISR(PCINT1_vect) {
	char *c;

	if (bis(PINC, PC4)) {
		c = recv_pixmap;
		recv_pixmap = pixmap;
		pixmap = c;

		write_address = 0;
	}
}

/**
 * Checks if the BS-Pin is pulled high; if it is, copy all data on PIND to the recv_pixmap at write_address.
 *
 * If the pixmap buffer is already full, receive additional control data.
 */
ISR(PCINT0_vect) {
	if(bis(PINB, PB2))
	{
		recv_pixmap[write_address] = PIND;

		if (write_address < BUFFER_SIZE) {
			write_address++;
		} else {
			if (bis(PIND, PD0)) {
				sbi(PORTB, PB5);
			} else {
				cbi(PORTB, PB5);
			}

		}
	}
}


void checkerboard (bool offset) {
	int x,y;

	if (offset) {
		for (x=1;x<DISPLAY_WIDTH;x+=2) {
			for (y=0;y<DISPLAY_HEIGHT;y+=2) {
				setpixel(x,y);
			}
		}
		for (x=0;x<DISPLAY_WIDTH;x+=2) {
			for (y=1;y<DISPLAY_HEIGHT;y+=2) {
				setpixel(x,y);
			}
		}
	} else {
		for (x=0;x<DISPLAY_WIDTH;x+=2) {
			for (y=0;y<DISPLAY_HEIGHT;y+=2) {
				setpixel(x,y);
			}
		}
		for (x=1;x<DISPLAY_WIDTH;x+=2) {
			for (y=1;y<DISPLAY_HEIGHT;y+=2) {
				setpixel(x,y);
			}
		}
    }
}
void clear (void) {
	memset(pixmap, 0, BUFFER_SIZE);
}

void pixmap_to_strange_pixmap(void) {
	uint8_t x, y;
	int i;

	for (y = 0; y < DISPLAY_HEIGHT; y++) {
		for (x = 0; x < DISPLAY_WIDTH; x++) {
			i = get_address_for_x_y(x,y);


			if (bit_get(pixmap[((y*DISPLAY_WIDTH)+x) >> 3], BIT(((y*DISPLAY_WIDTH)+x)%8))) {
				bit_set(transfer_pixmap[i>>3], BIT(i%8));
			} else {
				bit_clear(transfer_pixmap[i >> 3], BIT(i%8));
			}
		}
	}

}

void setpixel (int x, int y) {
	bit_set(pixmap[((y*DISPLAY_WIDTH)>> 3) + (int)(x>> 3)], BIT((x%8)));
}

int get_address_for_x_y (int x, int y) {
	int baseAddress = x * 8;

	if (y % 2 != 0) {
		baseAddress += 1216;
	}

	if (y < 8) {
		baseAddress += (y/2);
	} else {
		if (y % 2 != 0) {
			baseAddress += 11 - (y/2);
		} else {
			baseAddress += 11 - (y/2);
		}
	}

	return baseAddress;
}

void push_foo(void) {
	int i, j;

	// Ungerade Zeilen schieben
	for (j = 0; j < 32; j++) {
		for (i = 0; i < 38; i++) {
			send_pixel(bit_get(transfer_pixmap[((j*38)+i)>> 3], BIT(((j*38)+i)%8)) == BIT(((j*38)+i)%8));
		}

		if (j == 31) {
			send_pixel_block_spacer(true, false);
		} else {
			send_pixel_block_spacer(false, false);
		}
	}

	// Gerade Zeilen schieben
	for (j = 0; j < 32; j++) {
		for (i = 0; i < 38; i++) {
			send_pixel(bit_get(transfer_pixmap[((j*38)+i+1216)>> 3], BIT(((j*38)+i+1216)%8)) == BIT(((j*38)+i+1216)%8));
		}

		if (j == 31) {
			send_pixel_block_spacer(true, true);
		} else {
			send_pixel_block_spacer(false, true);
		}
	}
}
