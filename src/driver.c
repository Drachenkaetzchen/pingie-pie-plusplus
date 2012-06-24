#include "driver.h"
#include "bcm_gpio.h"

/**
 * Stores the last time when the display was refreshed.
 */
timespec lastRefreshTime;
bool refreshState = false;

void setup_gpio (void) {
	// Setup the RPi GPIO access methods
	setup_rpi_io();	

	// Setup the 5 output ports
	INP_GPIO(PIN5); // must use INP_GPIO before we can use OUT_GPIO
	OUT_GPIO(PIN5);

	INP_GPIO(PIN6); // must use INP_GPIO before we can use OUT_GPIO
	OUT_GPIO(PIN6);

	INP_GPIO(PIN7); // must use INP_GPIO before we can use OUT_GPIO
	OUT_GPIO(PIN7);

	INP_GPIO(PIN9); // must use INP_GPIO before we can use OUT_GPIO
	OUT_GPIO(PIN9);

	// Initialize all GPIO lines to low
	GPIO_CLR = 1<<PIN5;
	GPIO_CLR = 1<<PIN6;
	GPIO_CLR = 1<<PIN7;
	GPIO_CLR = 1<<PIN9;
}

/**
 * Setup the display communication, GPIO, timers etc.
 */
void setup_display (void) {
	setup_gpio();
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &lastRefreshTime);

}
/**
 * Calculates the difference between two timespec's.
 */
timespec diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

/**
 * Delays the transmission by the specified microseconds
 */
void _transmissionDelay (uint32_t usec) {
	timespec time1, time2, tdiff;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);

	while (1) {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
		tdiff = diff(time1, time2);

		refresh_display();
		if (tdiff.tv_nsec > (usec * 10)) {
			break;
		}
	}


}

/**
 * Checks if the refresh display cycle needs to be sent.
 *
 * This happens every 4 ms.
 */
void refresh_display (void) {
	timespec time2, tdiff;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
	tdiff = diff(lastRefreshTime, time2);

	if (tdiff.tv_nsec > (4000000)) {
		lastRefreshTime = time2;

		if (refreshState) {
			GPIO_CLR = 1<<PIN5;
		} else {
			GPIO_SET = 1<<PIN5;
		}

		refreshState = !refreshState;
	}
}

/**
 * Sends one individual pixel cycle.
 */
void send_pixel (bool set) {
	GPIO_SET = 1<<PIN7;
	if (set) {
		GPIO_SET = 1<<PIN9;
	} else {
		GPIO_CLR = 1<<PIN9;
	}
	
	_transmissionDelay(10);

	GPIO_CLR = 1<<PIN7;

	_transmissionDelay(10);
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
		GPIO_SET = 1<<PIN6;
		_transmissionDelay(10);
		GPIO_CLR = 1<<PIN6;
		_transmissionDelay(10);
	}
}
