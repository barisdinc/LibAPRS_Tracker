/*
 * This is a non-gps moving example for demonstrating simple usage for LibAPRS_Tracker
 *
 * Author : Baris DINC (TA7W/OH2UDS)
 *
 */

#include <LibAPRS_Tracker.h>

char myCALL[] = "TA7W";
char Lat[] = "2134.00N";
char Lon[] = "01234.00E";
int altitude = 0;  
char comment []= "/A=000000 TAMSAT hymTR APRS Tracker Test";

 
void setup() {
  APRS_init();

  APRS_setCallsign(myCALL, 9);
  APRS_setLat(Lat);
  APRS_setLon(Lon);
}
 
void loop() {
  APRS_sendLoc(comment, strlen(comment));
  while(bitRead(PORTB,5)); //Wait for transmission to be completed
  delay(10000);

  for (int hareket = 0; hareket < 9; hareket++)
  {
    Lat[5] = 48 + hareket;
    Lon[6] = 48 + hareket;
    comment[6] = 48 + hareket;
  }

  
}
