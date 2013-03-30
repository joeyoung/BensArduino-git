#ifndef saveState_h
#define saveState_h

//#include "WProgam.h"   // older than 1.0
#include "Arduino.h"


class saveState
{
public:
	//class constructor
	saveState( bool initstate );

	bool getState( );
	void setState( bool setstate );

private:
	bool _savedState;

}; // class saveState

#endif
