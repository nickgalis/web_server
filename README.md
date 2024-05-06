# cs410_final


Team Members: Nick Galis, Moryan Tchoumi, Anamu Uenishi.

The code attached is our final webserver project. We have implemented all basic test cases, as well as caching. We did not add support for multithreading. To compile everything, in the terminal run "make" and then "./webserv \<port\>". To add support for caching, add "-c" as a command option. After running, in any browser, type http://localhost:\<port\> to submit requests. To run the my-histogram program, do the following: http://localhost:<\port\>/my-histogram?directory=\<name-of-directory\> to run the program on your directory of choice. 

The cache replacement implemented is a FIFO replacement.

Our physical project involved a drone in which we soldered an ESP8266 chip in order to provide WiFi communication to support the betaflight configurator from a distance. The hardware was completed but unfortunately we did not have enough time to properly understand the Betaflight source code to be able to modify it to interpret MSP commands, so we used open source software instead.
