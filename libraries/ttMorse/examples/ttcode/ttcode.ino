// ttcode text from serial to morse code output example
//
// created: Feb 28/13  G. D. Young <jyoung@islandnet.com>
//
// Illustrates getting line from Serial in a non-blocking way so
// that ttMorse can function while still monitoring for serial i/p.
// However, this example does not prevent clobbering the data
// being sent by typing in another line over top the first data.
// The serial monitor needs to be set for 'newline' line ending.


#include "ttMorse.h"

ttMorse ttm( 13, 12, 800, 15, "xx" );

const char termchr = '\n';
char inp[80];
char ccnt;
boolean term, mstart;

void setup( ) {
  Serial.begin( 9600 );
  ccnt = 0;
  inp[0] = '\0';
  term = false;
}

void loop( ) {
  
  if( Serial.available( ) ) {
    inp[ccnt] = Serial.read( );
    if( inp[ccnt] == termchr ) {
      term = true;
      inp[ccnt] = '\0';  // zap the terminator
    } else {
      ccnt++;
      inp[ccnt] = '\0';
    }
  }
  
  if( term ) {
    Serial.println( inp );
    ccnt = 0;
    term = false;
    ttm.msetStr( inp );
    mstart = true;
  }
  
  if( mstart ) {
    if( ttm.msend( ) == false ) mstart = false;
  }
  
}
