#include "WProgram.h"
#include <controlLED.h>

// constructor
  controlLED::controlLED( int pin, int state ) {
    int _pin = pin;
    int _state = state;
} // constructor


  void controlLED::cntrolLED( int _pin, int _state ) {
  
    digitalWrite( _pin, _state );

} // controlLED


