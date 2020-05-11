# cansw_usb

The USB debug system for Waterloo Rocketry's CAN bus project


Parsley:

Used to make CAN messages more readable.

Run in bash using command:

```cat [Serial Port] | python -u parsley.py```

where [COM PORT] is the port USB board is on.
The ``-u`` argument tells python to run unbuffered, without it the output will not apear until the program stops running.

Example Port:

/dev/ttyACM0 on Linux or Mac

/dev/ttySX on Windows, where X = COM# - 1

(for example COM6 is /dev/ttyS5)

