TARGET=furnace_fan_switch
MCU=atmega8
F_CPU=1000000L
AVRDUDE_PROGRAMMER=usbtiny

ARDUINO_BOARD=atmega8

ARDUINO_MK_DIR=/usr/local/arduino-bsd-mk

MAKEOBJDIRPREFIX=/usr/obj

ARDUINO_CFLAGS+= -Wall -Werror -Wstrict-prototypes

.include "${ARDUINO_MK_DIR}/bsd.arduino.mk"
