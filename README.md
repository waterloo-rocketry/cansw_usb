# cansw_usb

## Overview:

The USB debug system for Waterloo Rocketry's CAN bus project. The system is 
designed to facilitate sending and receiving CAN messages via a USB serial port.
The C code is written for a PIC16f1455, and as such currently runs on the
Waterloo Rocketry pic development board AKA "Keto" and the USB Debug board.
It is based on Waterloo Rocketry's RocketCAN library:
https://github.com/waterloo-rocketry/canlib

## Sending CAN Messages:

CAN messages can be sent to the CAN bus using the command:

```echo -n "mSSS,DD,DD,DD;" > [Serial Port]```

-the "m" at the beginning of the string is a literal character m.
``SSS`` is the 11 bit message ID, and ``DD`` is one byte of data. Up to 8 bytes
of data can be sent.

## Parsley:

Used to make CAN messages more readable.

Run in bash using command:

```cat [Serial Port] | python -u parsley.py```

where [Serial PORT] is the port USB board is on.

The ``-u`` argument tells python to run unbuffered, without it the output will 
not apear until the program stops running.

Example Port:

/dev/ttyACM0 on Linux or Mac

/dev/ttySX on Windows, where X = COM# - 1

(for example COM6 is /dev/ttyS5)

### Example output and explination of format:

### Pasing files into parsley:


