# About

Tracker is a program I created to help with tracking your HAB via radio. It was built around the Raspberry Pi 3, NTX2 transmitter and the Adafruit Ultimate GPS.

Once tracker is started it will read GPS data from the local gpsd daemon and send it out in RTTY for you to read.

Because I'm horrible with C, almost all of this code was lifted from the following sources 
and put together in a horrible mess:

https://ava.upuaut.net/?p=627

http://www.airspayce.com/mikem/bcm2835/index.html

http://catb.org/gpsd/


# Pre-reqs

* gpsd running on the Pi on the default port



# Use

To run on the Pi, start gpsd first:

`sudo gpsd /dev/serial0 -F /var/run/gpsd.sock`

Then start up tracker:

`sudo /usr/local/bin/tracker`

`sudo` is needed to help get `tracker` into a more "real-time" mode.



# Reading the data

I use software to receive the signals so my setup consists of gqrx and fldigi. Whatever you use you
will want to set your RTTY config to

* 50 baud
* 7 bits
* no parity
* 2 stop bits

The carrier shift will depend on how you've setup the NTX2, mine works well around the 650 range.


# Building

To build from source you will need the autotools suite. You will also need the bcm2835 
library as well as the gpsd library (see links above).

Checkout the source and run:

`autoreconf --install`

`./configure`

`make`

`sudo make install`
