#ifndef controlLED_h
#define controlLED_h

#include "WProgram.h"
#include "WConstants.h"

class controlLED
{
public:
	//class constructor
        controlLED( int pin, int state );

	void cntrolLED( int pin, int state );

private:
	int _pin;
	int _state;

}; // class controlLED

#endif
