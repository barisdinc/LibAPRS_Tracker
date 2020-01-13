/*
 * This is a very simple usage for LibAPRS_Tracker
 *
 * Author : Baris DINC (TA7W/OH2UDS)
 *
 */

#include <LibAPRS_Tracker.h>
 
void setup() {
  APRS_init();
  char myCALL[] = "TA7W";
  char Lat[] = "2134.00N";
  char Lon[] = "01234.00E";

  APRS_setCallsign(myCALL, 9);
  APRS_setLat(Lat);
  APRS_setLon(Lon);
}
 
void loop() {
    
  char comment []= "TAMSAT hymTR APRS Tracker Test";
  APRS_sendLoc(comment, strlen(comment));
  while(bitRead(PORTB,5)); //Wait for transmission to be completed
  delay(10000);
}
 
 
