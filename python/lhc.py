# coding: UTF-8
import RPi.GPIO as GPIO
import Image
import ImageDraw
import pprint
import time

BS = 7
B0 = 8
B1 = 10
B2 = 11
B3 = 12
B4 = 13
B5 = 15
B6 = 16
B7 = 18
PS = 3

DISPLAY_WIDTH = 156
DISPLAY_HEIGHT = 16

global pixmap
global draw
global font

pixmap = Image.new('1', (152,16))

GPIO.setmode(GPIO.BOARD)

GPIO.setup(BS, GPIO.OUT)
GPIO.setup(B0, GPIO.OUT)
GPIO.setup(B1, GPIO.OUT)
GPIO.setup(B2, GPIO.OUT)
GPIO.setup(B3, GPIO.OUT)
GPIO.setup(B4, GPIO.OUT)
GPIO.setup(B5, GPIO.OUT)
GPIO.setup(B6, GPIO.OUT)
GPIO.setup(B7, GPIO.OUT)
GPIO.setup(PS, GPIO.OUT)

def chunks(l, n):
    for i in xrange(0, len(l), n):
        yield l[i:i+n]


def transfer():
	pixmap2 = pixmap.rotate(180)

	for x in chunks(list(pixmap2.getdata()),8):
		GPIO.output(B0, x[0])
		GPIO.output(B1, x[1])
		GPIO.output(B2, x[2])
		GPIO.output(B3, x[3])
		GPIO.output(B4, x[4])
		GPIO.output(B5, x[5])
		GPIO.output(B6, x[6])
		GPIO.output(B7, x[7])

		GPIO.output(BS, GPIO.HIGH)
		GPIO.output(BS, GPIO.LOW)


	# test relais
	GPIO.output(B0, 1)
	
	GPIO.output(BS, GPIO.HIGH)
	GPIO.output(BS, GPIO.LOW)

	GPIO.output(PS, GPIO.HIGH)
        GPIO.output(PS, GPIO.LOW)


import ImageFont
font = ImageFont.truetype("/root/miso-bold.ttf", 24)

draw = ImageDraw.Draw(pixmap)

def centertext(text):
	draw.rectangle((0,0,152,16),fill=0)
	x = 66 - (draw.textsize(text)[0] / 2)

	draw.text((x,0), text, font=font, fill=1)
	transfer()
	time.sleep(1)


while 1:
	centertext("LHC4")
	centertext("COLLIDING")
	centertext("NERDS")
	centertext("SINCE")
	centertext("2 0 1 0")
