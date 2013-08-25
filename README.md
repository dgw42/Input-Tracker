Input-Tracker
=============

Linux Input Tracker project for CS283

This program tracks mouse and keyboard input for a specified period of time and outputs relevant statistics. Typical usage would be as a background process executed by the shell.

Options:
-h : provide program usage and exit
-t : specify the length of time in seconds to run the program, runs for 10 seconds by default
-n : specify mouse output name formatting, will be appended by 0 indexed screen number, defaults to 'out'
-i : specify image format in lowercase without period, currently supports png and jpg, defaults to png
-l : specify the file to write keyboard log messages, if the file does not exist it will be created, if it does exist it will be overwritten. The default file is 'log.txt'
-s : specify the file to write keyboard statistics, if the file does not exist it will be created, if it does exist it will be overwritten. The default file is 'stat.txt'

Special Cases:
There are no signal handlers for the mouse data, therefore no mouse data image will be generated when an interrupt is received, however keyboard data will be output.

Known Bugs:
None
