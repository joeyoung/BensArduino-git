// mtest2 - illustrate two morse objects
//
// created: Feb26/13 G. D. (Joe) Young <jyoung@islandnet.com>
//
// Show that two separate morse objects can run simultaneously.
// Also use of NONE parameter for tonepin (there can only be one
// tone output running at one time). Some example stopping methods.

#include <ttMorse.h>

char *teststr = "this is a test =";
char *another = "    and another.";

char *test2 = " moe moe moe";

ttMorse ttm( 13, 12, 800, 20, teststr );
ttMorse ttm2( 11, NONE, 800, 10, teststr );

unsigned long timeout;
boolean m2done;
char m2count;

void setup( ) {
  timeout = millis( ) + 30000;
  m2done = false;
  m2count = 2;
} // setup

void loop ( ) {

    ttm.msend( );

    if( ttm2.msend( test2 ) == false ) {
      m2count--;
      if( m2count <= 0 ) m2done = true;
    }
    
    if( ttm.mbusy( ) ) {
      // nothing
    } else {
      ttm.mspeed( 25 );
      ttm.msend( another );
    }
    
    if( timeout < millis( )  || m2done ) {
      noTone( 12 );
      do {
        //nothing
      } while( true );
    } // if timeout

} // loop
