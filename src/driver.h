#ifndef __DRIVER_H
#define __DRIVER_H

#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#define PIN5 4
#define PIN6 17
#define PIN7 18
#define PIN8 21
#define PIN9 22

#define DISPLAY_WIDTH 152
#define DISPLAY_HEIGHT 16

void setup_gpio (void);
void setup_display (void);
timespec diff(timespec start, timespec end);
void _transmissionDelay (uint32_t usec);
void refresh_display (void);
void send_pixel (bool set);
void send_pixel_block_spacer (bool swt, bool s);
#endif
