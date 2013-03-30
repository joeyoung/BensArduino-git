// ttcode2 text from serial to morse code output, double-buffered
//
// created: Feb 28/13  G. D. Young <jyoung@islandnet.com>
//
// Illustrates getting line from Serial in a non-blocking way so
// that ttMorse can function while still monitoring for serial i/p.
//
// Compared with the ttcode example, a pair of line buffers allows
// for typing into one while the other is being sent. It's still
// possible to clobber sending if typing ahead by over a whole line.
//
// The serial monitor needs to be set for 'newline' line ending.


#include "ttMorse.h"

ttMorse ttm( 13, 12, 800, 15, "xx" );

const char termchr = '\n';
char inp[2][80];
char ccnt;
char seq;
boolean term[2];
boolean mstart;

void setup( ) {
  Serial.begin( 9600 );
  ccnt = 0;
  inp[0][0] = '\0';
  inp[1][0] = '\0';
  term[0] = false;
  term[1] = false;
  seq = 0;
}

void loop( ) {
  
  if( Serial.available( ) ) {
    inp[seq][ccnt] = Serial.read( );
    if( inp[seq][ccnt] == termchr ) {
      term[seq] = true;
      if( inp[seq][ccnt-1] != ' ' ) {
        inp[seq][ccnt] = ' ';  // force trailing space
        ccnt++;
      }
      inp[seq][ccnt] = '\0';  // zap the terminator
    } else {
      ccnt++;
      inp[seq][ccnt] = '\0';
    }
  }
  
  if( term[seq] && !mstart ) {  // must wait for sending to end
    Serial.println( inp[seq] );
    ccnt = 0;
    term[seq] = false;
    ttm.msetStr( inp[seq] );
    mstart = true;
    if( ++seq > 1 ) seq = 0;  // switch buffers
    inp[seq][0] = '\0';
  }
  
  if( mstart ) {
    if( ttm.msend( ) == false ) mstart = false;
  }
  
}
