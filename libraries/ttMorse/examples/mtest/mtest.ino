// mtest - simple ttMorse example

// created: Feb 26/13  G. D. (Joe) Young <jyoung@islandnet.com>

// starts with constructor's string, when it's finished, switches
// to a second string which is then sent continuously thereafter.
// Shows handling of leading spaces in string.

#include <ttMorse.h>

char *teststr = "this is a test =";
char *another = "    and another.";

ttMorse ttm( 13, 12, 800, 10, teststr );

boolean first;

void setup( ) {
} // setup

void loop ( ) {

    ttm.msend( );
    if( ttm.mbusy( ) ) {
      // nothing
    } else {
      ttm.msend( another );
    }

} // loop
