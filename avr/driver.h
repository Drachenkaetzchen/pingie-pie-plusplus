#ifndef __DRIVER_H
#define __DRIVER_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#define LINE5 PC3
#define LINE6 PC2
#define LINE7 PC1
#define LINE9 PC0

#define DISPLAY_WIDTH 152
#define DISPLAY_HEIGHT 16

void setup_gpio (void);
void setup_display (void);
void refresh_display (void);
void send_pixel (bool set);
void send_pixel_block_spacer (bool swt, bool s);
void init_refresh_timer (void);
#endif
