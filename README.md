# LibAPRS_Tracker
This is nano minimal version of LibAPRS for tracker devices

This library is based on https://github.com/markqvist/LibAPRS 
Originally I tried to use LibAPRS, when used in conjuction with Adafruid_GPS or TinyGPS it takes the whole global memory leaving no space for APRS Tracker functions (e.g.: serial configuration )
So I decided to sanitize the LibAPRS for those who will use it for mini tracker devices to send APRS packeges.

# Features
- Generate APRS packets
- Generate tones useing resistor ladder
- Send Location
-
-

# Features taht does not exist
- send messages over APRS
- receive messages over APRS
- receive and decode APRS messages


# Default definitions 
D3 PTT Pin
D4, D5, D6, D7 Audio generation PINs
Destination HYMTR
PATH1 WIDE1-1
PATH2 WIDE2-1








