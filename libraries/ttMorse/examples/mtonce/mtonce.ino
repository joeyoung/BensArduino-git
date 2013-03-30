// mtest - simple ttMorse example

// created: Feb 26/13  G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Mar 19/13 from mtest, flag to control once only sending
//                    second string includes a prosign

// starts with constructor's string, when it's finished, switches
// to a second string which is then sent continuously thereafter.
// Shows handling of leading spaces in string.

#include <ttMorse.h>

char *teststr = "this is a test =";
char *another = "    and another. *ar*";

ttMorse ttm( 13, 12, 800, 10, teststr );

boolean first;
char msgcount;

void setup( ) {
  first = true;
  msgcount = 2;
} // setup

void loop ( ) {
  
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
