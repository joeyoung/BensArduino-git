// ttMorse_PWM.h - dds, pwm tone for morse code library

// created: March 8, 2013  G. D. (Joe) Young <jyoung@islandnet.com>

// revised: March 19/13 - o/p pin control via tonepin

// The tone generator uses timer 2 in fast pwm mode, the pwm duty cycle is
// updated at each overflow interrupt with an 8-bit sine wave sample 
// obtained from a lookup table in sinTab.h. The sine sample to use is
// determined by using the top 8-bits of a 32-bit phase accumulator to
// address the 256-byte table. The phase accumulator has a phase increment
// determined by the desired tone frequency added at each timer 2 overflow
// interrupt--ie., the pwm generator also clocks direct-digital-synthesis
// (dds) tone generation. Formulas:
//
// pwm wave freq = 16 MHz / 256 = 62500 Hz
// phase increment = ( 2**32 * Fout ) / 62500, approx = 68719 * Fout
//
// Additionally, the tone on and tone off transitions are controlled by a
// linear amplitude ramp that is also synthesized at the pwm rate and
// multiplies the sine samples for the duration of the rise time and fall
// time (identical times in this implementation). The amplitude ramp 
// accumulator is 16-bits, the top 8-bits used to multiply the sine
// samples. During the rise and fall times, the top 8-bits of the
// amplitude, sine product are sent to the pwm.

#ifndef TTMORSEPWM_H
#define TTMORSEPWM_H

#include <ttMorse.h>
//#include "sinTab.h"

// statics for ISR allocated here for global use
	static volatile unsigned long accumulator, increment, pwmtick;
	static volatile unsigned int amplacc, amplinc, sample;
	static volatile unsigned char highbyte, pwmkhz, pwmoutput;
	static bool armtoff, tonestart;

class ttMorse_PWM : public ttMorse {

public:

	// constructor - let ttMorse do most constructing
	ttMorse_PWM( char ledpin, byte tonepin, unsigned int tfreq, byte cspwpm,
														 char *str, byte risetime ) :
	ttMorse( ledpin, tonepin, tfreq, cspwpm, str ){
		rtmsec = risetime;
		if( tonepin == 11 ) {
			pwmpin = 11;
		} else if( tonepin == 3 ) {		// only 11 and 3 avail for timer2 pwm
			pwmpin = 3;
		} else {
			ttMorse::toneon = false;
		}
		setRise( rtmsec );
		setFreq( tfreq );
//		setTimInt( );  		//doesn't work in constructor
	}
	
	// access functions
	void toneOn( );
	void toneOff( );

	inline bool onTimer( ); // use pwm generator for timing
	inline bool offTimer( );
	unsigned long initTimers( );

	void setFreq( unsigned int freq );
	void setRise( byte rtmsec );

	// allow interrupt handler access
	static void timer_int( );
	void setTimInt( );

private:

	byte rtmsec;
	unsigned int mtfreq;
	byte pwmpin;

}; // class ttMorse_PWM


#endif


