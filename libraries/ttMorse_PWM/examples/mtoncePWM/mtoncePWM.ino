// mtest - simple ttMorse example

// created: Feb 26/13  G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Mar 19/13 - flags to control one time, o/p pwm on 11
//                      prosign in second string.

// starts with constructor's string, when it's finished, switches
// to a second string which is then sent continuously thereafter.
// Shows handling of leading spaces in string.

#include <ttMorse.h>
#include <sinTab.h>
#include <ttMorse_PWM.h>


char *teststr = "this is a test =";
char *another = "    and another. *ar*";

ttMorse_PWM ttm( 13, 11, 900, 10, teststr, 5 );

boolean first;
char msgcount;

unsigned long onesec;
unsigned long lps;    //needs to be long-ety loop below ~116000 lps

void setup( ) {
  Serial.begin( 9600 );
  onesec = millis( ) + 1000UL;
  lps = 0;
  ttm.setTimInt( );
  first = true;
  msgcount = 2;
} // setup

void loop ( ) {
  
  lps++;
  if( onesec < millis( ) ) {
    onesec += 1000UL;
    Serial.println( lps );
    lps = 0;
  } // onesec passed

  if( first ) {
    ttm.msend( );
    if( ttm.mbusy( ) ) {
      // nothing
    } else {
      msgcount--;
      ttm.msetStr( another );
    }
  } // if first
  
  if( msgcount <= 0 ) first = false;

} // loop
