// ttMorse.cpp text to morse code generator - library version

// started: Feb 25/13 - G. D. Young

// revised: Feb 28/13 - remap lower case for smaller code table, fixes
//          Mar  1/13 - complete handling of (extra) spaces - start, end,
//                      middle, spaces-only line
//          Mar  3/13 - correct mspeed constant. Simplify msend access functions
//          Mar 11/13 - over-rideable timer functions
//          Mar 19/13 - tidyup, correction to if( elton ... , ledpin polarity
//                      inversion option, prosign handling
//          
// the calculation of mtidx from each input ascii moves the ascii table down 0x20,
// eliminating the (non-printing) first two rows; in addition, the moved result
// is tested for the lower-case alphabet and characters in those two rows are 
// moved down two more. The codetable can then be 0x00 to 0x3f instead of 0x00 to
// 0x5f. The larger-codetable lookup (and the larger table in codeTable.h) is left
// in commented lines, in case it's later desired for some reason unfathomable now.

#include <string.h>
#include "ttMorse.h"
#include "codeTable.h"


ttMorse::ttMorse( char ledpin, byte tonepin, unsigned int tfreq, byte cspwpm, char *str ) {
	mlpin = ledpin;
	if( tonepin == NONE ) {
		toneon = false;
	} else {
		toneon = true;
		mtpin = tonepin;
	} // if no tone
	if( ledpin < 0 ) {
		opinvert = true;
		mlpin = -ledpin;
		digitalWrite( mlpin, HIGH );
	} else {
		opinvert = false;
		mlpin = ledpin;
		digitalWrite( mlpin, LOW );
	} // if invert output logic level
	pinMode( mlpin, OUTPUT );						// these should be in a begin( )
	if( mtpin != NONE )pinMode( mtpin, OUTPUT );
	mtfreq = tfreq;
	dotDur = 1200UL/cspwpm;  // translate wpm to dot time in msec
	mstr = str;
	prosign = false;
} // constructor


// access functions

bool ttMorse::msend( ) {
	return mmsend( );
}

bool ttMorse::msend( char *str ) {
	mstr = str;
	return mmsend( );
}

bool ttMorse::mbusy( ) {
	return sending;
} // mbusy( )

void ttMorse::msetStr( char *str ) {
	mstr = str;
} // msetStr( )

void ttMorse::mspeed( byte cspwpm ) {
	dotDur = 1200UL/cspwpm;
} // mspeed( )

// wrappers for tone functions and timers that can be replaced
// with ttMorsePWM versions
void ttMorse::toneOn( ) {
	tone( mtpin, mtfreq );
} // toneOn( )

void ttMorse::toneOff( ) {
	noTone( mtpin );
} // toneOff( )

bool ttMorse::onTimer( ) {
	return elementStart < millis( );
} // onTimer( )

bool ttMorse::offTimer( ) {
	return elementEnd < millis( );
} // offTimer( )

unsigned long ttMorse::initTimers( ) {
	return millis( );
} // initTimers


// (re)set pointers, counters, first charcter
void ttMorse::msendSet( ) {
	eltOn = false;
	sending = true;
	cidx = 0;
	tabsiz = strlen( mstr );
//	elementStart = millis( ) + 2;
	if( mstr[cidx] == '*' ) {
		prosign = true;
		cidx++;
	} // if start of prosign
	elementStart = initTimers( ) + 2;
//	codeChar = morseTable[(mstr[cidx]&0x7f)-0x20];
	mtidx = ((mstr[cidx]-0x20)>0x3f) ? mstr[cidx]-0x40 : mstr[cidx]-0x20;
	codeChar = morseTable[mtidx];
	while( codeChar == 0 && cidx < tabsiz ) {		// handle several leading spaces
		elementStart += 7*dotDur;
		cidx++;
//		codeChar = morseTable[(mstr[cidx]&0x7f)-0x20];
		mtidx = ((mstr[cidx]-0x20)>0x3f) ? mstr[cidx]-0x40 : mstr[cidx]-0x20;
		codeChar = morseTable[mtidx];
	}
	if( cidx >= tabsiz ) { 		// only spaces found
		lastelt = true;
		eltOn = true;
		elementEnd = elementStart - dotDur;		// pretend elt on, end after space(s)
		prosign = false;		// cancel possible isolated prosign
	} else {					// char found, setup decoder
		setGetBit( );
		endCode = false;
	}
} // msendSet( )


// enter here at loop scan rate, but action controlled by element timers, many flags
bool ttMorse::mmsend( ) {

	if( !sending ) ttMorse::msendSet( );	// first time in, do setup

//	if( !eltOn && sending && (elementStart < millis( )) ) {
	if( !eltOn && sending && onTimer( ) ) {
		elementStart += 2*dotDur;   	// at least one dot duration and space
		if( getBit( ) ) {
			elementStart += 2*dotDur; 	// if element is dash, add two more
		} // if dash
		elementEnd = elementStart - dotDur;
		if( opinvert ) {
			digitalWrite( mlpin, LOW );
		} else {
			digitalWrite( mlpin, HIGH );
		} // send the element as level
//		if( toneon ) tone( mtpin, mtfreq );		// and as tone, if on
		if( toneon ) toneOn( );		// and as tone, if on
		eltOn = true;
		if( endCode ) {
			if( !prosign ) elementStart += 2*dotDur;   // letter space
			cidx++;
			if( mstr[cidx] == '*' ) {
				prosign = !prosign;
				cidx++;
			}
			if( cidx >= tabsiz ) {
				lastelt = true;		//str exhausted, prepare to stop when elt over
				prosign = false; 	// cancel possible isolated prosign entry
			} else {
//				codeChar = morseTable[(mstr[cidx]&0x7f)-0x20];
				mtidx = ((mstr[cidx]-0x20)>0x3f) ? mstr[cidx]-0x40 : mstr[cidx]-0x20;
				codeChar = morseTable[mtidx];
				if( codeChar == 0 ) {          // word space(s)
					elementStart += 4*dotDur;	// finish space started with last letter
					sendingsp = true;
					cidx++;
					if( mstr[cidx] == '*' ) {
						prosign = !prosign;
						cidx++;
					}
					if( cidx >= tabsiz ) {
						lastelt = true;
						prosign = false;
					} else {
//						codeChar = morseTable[(mstr[cidx]&0x7f)-0x20];
						mtidx = ((mstr[cidx]-0x20)>0x3f) ? mstr[cidx]-0x40 : mstr[cidx]-0x20;
						codeChar = morseTable[mtidx];
						while( codeChar == 0 ) {
							elementStart += 7*dotDur;
//							sendingsp = true;
							cidx++;
							if( cidx >= tabsiz ) {
								lastelt = true;
								break;		//watch for trailing spaces
							} 
//							codeChar = morseTable[(mstr[cidx]&0x7f)-0x20];
							mtidx = ((mstr[cidx]-0x20)>0x3f) ? mstr[cidx]-0x40 : mstr[cidx]-0x20;
							codeChar = morseTable[mtidx];
						} // while more word spaces
					} // if end table and spacing
				} // if word space
				setGetBit( );
				endCode = false;
			} // if end of string
		} // if end of code character
	} // if elementStart

	if( eltOn && offTimer( ) ) {
		if( opinvert ) {
			digitalWrite( mlpin, HIGH );
		} else {
			digitalWrite( mlpin, LOW );
		} // end the element as level
//		noTone( mtpin );
//		toneOff( mtpin );
		toneOff( );
		if( sendingsp ) {
			elementEnd = elementStart - dotDur;		// 'element' continues for space(s)
			sendingsp = false;
		} else {
			eltOn = false;
			if( lastelt ) {
				sending = false;
				lastelt = false;
			} // if last elt finished
		}
	} // if end of element

	return sending;

} // msend( )


// setup getBit - locate fence bit in codeChar
void ttMorse::setGetBit( ) {

  bitCnt = 7;
  nextBit = 0b10000000;
  
  while( (bitCnt > 0) && ((nextBit & codeChar) == 0) ) {
    nextBit = nextBit >> 1;  // find fence
    bitCnt--;
  } // while leading 0s before fence

} // setGetBit( )


// examine codeCharacter for next element to send
bool ttMorse::getBit( ) {
    nextBit = nextBit>>1;
    bitCnt--;
  if( bitCnt == 0 ) {
    endCode = true; 
  } // if last bit in codeChar
  return( (nextBit & codeChar) == nextBit );
} // getBit( )


