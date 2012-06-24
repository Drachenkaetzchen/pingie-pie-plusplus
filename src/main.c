/*
 * main.c
 *
 *  Created on: Jun 24, 2012
 *      Author: felicitus
 */

#include "bcm_gpio.h"
#include "driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include <sys/mman.h>

bool ftd = false;

char *transfer_pixmap;
char *pixmap;

void push_foo(void);
void pixmap_to_strange_pixmap(void);
int get_address_for_x_y (int x, int y);

#define SHMOBJ_PATH         "/foo1410"

int main(void) {
	int i;
	int shmid;
	setup_display();
	shm_unlink(SHMOBJ_PATH);
	transfer_pixmap = (char*)malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT);
	pixmap = (char*)malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT);
	int shared_seg_size = DISPLAY_WIDTH * DISPLAY_HEIGHT;

	key_t key = ftok("/tmp/foo", 'A');
	shmid = shmget (key,shared_seg_size,IPC_CREAT|0600);
	if (shmid==-1) {
	    perror("shmget");
	    exit(1);
	  }

	pixmap = shmat(shmid, 0, 0);

	memset(transfer_pixmap, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT);
	memset(pixmap, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT);

	int x, y;

	while (1) {
		pixmap_to_strange_pixmap();
		push_foo();
	}

}

void pixmap_to_strange_pixmap(void) {
	int x, y,i;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < DISPLAY_WIDTH; x++) {
			transfer_pixmap[get_address_for_x_y(x,y)] = pixmap[(y*DISPLAY_WIDTH)+x];
		}
	}

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
			send_pixel((char*)transfer_pixmap[(j*38)+i]);
			ftd = !ftd;
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
			send_pixel((char*)transfer_pixmap[(j*38)+i+1216]);
		}

		if (j == 31) {
			send_pixel_block_spacer(true, true);
		} else {
			send_pixel_block_spacer(false, true);
		}
	}
}
