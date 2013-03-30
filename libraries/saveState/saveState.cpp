// #include "WProgram.h"
#include "Arduino.h"
#include <saveState.h>

// constructor
  saveState::saveState( bool initstate ) {
    _savedState = initstate;
  } // constructor


bool saveState::getState( void ) {
	return _savedState;
} // getState


void saveState::setState( bool setstate ) {
	_savedState = setstate;
} // setState
