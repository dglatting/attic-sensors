# attic-sensors
An array of MQ sensors and i2c devices for my attic, and written in c++. The sensor array includes six MQ sensors (MQ2, MQ3, MQ4, MQ6, MQ7, and MQ9) connected to A/D converters. Temperature and relative humidity in addition to the MQ sensors are graphed under Munin.

The purpose of this project was to install an array of sensors in my attic and graph them over a year. The project's goals included:
1) Learn what kind of gasses and temperatures occur in my attic year round,
2) Learn i2c bus interaction through C++, 
3) Explore some C++17 features, and
4) To have fun.
I am aware that I could have written the code in Python or used Circuit Python, which may make sense and I'm not disparaging those tools, but they were contrary to my goals. This code isn't as compact or efficient as it could be, and I don't care - that wasn't one of my goals. There is a fair amount of logging and conditional compilation which can be removed by undefining compile-time variables in the Makefile. Eliminating those variable definitions and compiling with the -Os optimization flag will significantly reduce the code's size. Be aware this code is multi-threaded, too.
