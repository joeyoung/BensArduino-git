// testing ttMorse_PWM library

// works fine with risetime control ISR, but full ttMorse examples
// do not--the ISR with risetime control takes too long, so morse
// timing gets clobbered (by what?).

// Mar 17/13 using background waveform update (at ~8kHz) testPWMtiming2 - still
// ng with morse generator.

// Mar 20/13 clean out most test hacks, leave rest for example

#include <ttMorse.h>
#include <sinTab.h>
#include <ttMorse_PWM.h>

char str[ ] = " foo";
byte rt = 5;           // set for "clickless" keying

ttMorse_PWM ttm( 13, 11, 850, 20, str, rt );

boolean dash = false;
boolean ton = true;
unsigned long timer;
const unsigned long dur = 50;
const unsigned long delta = 0;  // testing a "weight" feature wi iamb

unsigned long onesec;
unsigned long lps;    //needs to be long-ety loop below ~116000 lps


void setup( ) {
  pinMode( 13, OUTPUT );
  pinMode( 2, OUTPUT );
  ttm.setTimInt( );
  ttm.toneOn(  );
  delay( 2000 );
  ttm.toneOff(  );
  timer = millis( ) + dur;
  timer = ttm.initTimers( ) + dur;
  ttm.elementStart = ttm.initTimers( ) + 2*dur;
  ttm.elementEnd = ttm.elementStart - dur-rt;
//  ttm.elementStart = ttm.elementStart + 2*dur;
  Serial.begin( 9600 );
  onesec = millis( ) + 1000UL;
}

void loop( ) {
 
  lps++;
  if( onesec < millis( ) ) {
    onesec += 1000UL;
    Serial.print( lps );
    Serial.print( "  " );
    Serial.println( ttm.initTimers( ) );
    lps = 0;
  } // onesec passed


  digitalWrite( 2, iamb( ) );    // this works
//  ttm.msend( );                  // this doesn't.. but now does!!!
  
}

bool iamb( ) {
  if( !ton && ttm.onTimer( ) ) {
    ttm.elementStart = ttm.elementStart + 2*dur;
    if( dash ) ttm.elementStart += 2*dur;
//    ttm.elementEnd = ttm.elementEnd + 2*dur;
    ttm.elementEnd = ttm.elementStart - dur + delta;
    ttm.toneOn( );
    digitalWrite( 13, HIGH );
    ton = true;
  } else {
    if( ton && ttm.offTimer( ) ) {
    ttm.toneOff( );
    dash = !dash;
    digitalWrite( 13, LOW );
    ton = false;
    }
  }
  return ton;
}

