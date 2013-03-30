// ttMorse.h header for text to morse code generator - library version


// started: Feb 25/13 - G. D. Young

// revised: Mar  1/13 - sendingsp boolean for complete spacing handling.
//          Mar 11/13 - over-rideable timer functions
//          Mar 19/13 - tidyups, revisions(see .cpp)
//          

#ifndef TTMORSE_H
#define TTMORSE_H

#include "Arduino.h"

#define NONE 0xff		// to specify no tone in constructors

class ttMorse {

public:

	// constructor
	ttMorse( char ledpin, byte tonepin, unsigned int tfreq, byte cspwpm, char *str );

	// access functions
	bool msend( );				// start sending constructed string
	bool msend( char *str );	// start sending new string
	bool mbusy( );  			// true if sending underway
	void msetStr( char *str );  // specify string to send
	void mspeed( byte speed );	// change sending speed

	virtual void toneOn( ); 	// to allow for pwm tone
	virtual void toneOff( );

	virtual inline bool onTimer( );	// to allow timers from pwm gen
	virtual inline bool offTimer( );
	virtual unsigned long initTimers( );
	unsigned long elementStart, elementEnd;  // for timers

protected:
	bool toneon;

private:
	byte mlpin, mtpin, cspwpm;
	unsigned int mtfreq;
	char *mstr;

	unsigned long dotDur;
	bool sending;
	bool eltOn;			//arduino-1.0.3 doesn't allow initializing
	bool endCode;
	bool lastelt;
	bool sendingsp;
	bool opinvert;
	bool prosign;

	char cidx, mtidx, lcount;
	char tabsiz;

	byte codeChar;
	byte bitCnt;
	byte nextBit;         // mask for element position

	bool getBit( );
	void setGetBit( );
	void msendSet( );
	bool mmsend( );

}; // class ttMorse

#endif

