#ifndef toggleLED_h
#define toggleLED_h

#include "WProgram.h"
#include <controlLED.h>

class toggleLED
{
public:
	//class constructor
	toggleLED( int pin, int state );

	int tggleLED( int pin, int state );

private:
	int _pin;
	int _state;
	controlLED* _ctrl;

}; // class toggleLED

#endif
