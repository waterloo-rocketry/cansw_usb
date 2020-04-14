# cansw_usb
The USB debug system for Waterloo Rocketry's CAN bus project

Parsley:
Used to make CAN messages more readable.
Run in bash using command:
cat [COM PORT] | python -u parsley.py
where [COM PORT] is the port USB board is on.
Example Port:
/dev/ttyACM0 on Linux or Mac
/dev/ttySX on Windows, where X = COM# - 1
(for example COM6 is /dev/ttyS5)