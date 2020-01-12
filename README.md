# LibAPRS_Tracker
This is nano minimal version of LibAPRS for tracker devices

This library is based on https://github.com/markqvist/LibAPRS 
Originally I tried to use LibAPRS, when used in conjuction with Adafruid_GPS or TinyGPS it takes the whole global memory leaving no space for APRS Tracker functions (e.g.: serial configuration )

So I decided to sanitize the LibAPRS for those who will use it for mini tracker devices to send APRS packeges.

LibAPRS_Tracker is a library for the Arduino IDE, that makes it easy to design and implemet your own APRS Tracker devices for sending out your location, telemetry, etc. The hardware is Micromodem compiant using resistor ladder on pins D4,5,6,7. The PTT pin is D3. All the AFSK modulation, AX.25 packing, and such is running transparently in the “background” in interrupts.

# Features
- Generate APRS AX.25 packets
- Generate tones using resistor ladder
- Send Location
- Send telemetry
- Easy configuration
- Setting destination, path, symbol, height, power, etc.

# Features that does not exist
- send messages over APRS
- receive messages over APRS
- receive and decode APRS messages


# Default definitions 
D3 PTT Pin
D4, D5, D6, D7 Audio generation PINs
Destination : HYMTR
PATH1 : WIDE1-1
PATH2 : WIDE2-1
Preamble : 350
TX Tail : 100
Symbol Table : /
Symbol : > (car)
Power : 10   (not used if > 10)
Height : 10  (not used if > 10)
Gain : 10    (not used if > 10)
Directivity : 10 (not used if > 10)









