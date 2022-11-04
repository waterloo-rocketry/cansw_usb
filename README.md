# cansw_usb

## Overview:

The USB debug system for Waterloo Rocketry's CAN bus project.

## Sending CAN Messages:

CAN messages can be sent to the CAN bus using the command:

```echo -n "mSSS,DD,DD,DD;" > [Serial Port]```

-the "m" at the beginning of the string is a literal character m
-SSS is the message command type (in hex)
-DD are the data fields defined in canlib (in hex)


## Parsley:

Used to make CAN messages more readable.

Run in bash using command:

```cat [Serial Port] | python -u parsley.py```

where [COM PORT] is the port USB board is on.

The ``-u`` argument tells python to run unbuffered, without it the output will not apear until the program stops running.

Example Port:

/dev/ttyACM0 on Linux or Mac

/dev/ttySX on Windows, where X = COM# - 1

(for example COM6 is /dev/ttyS5)

### Example output and explination of format:

### Pasing files into parsley:


