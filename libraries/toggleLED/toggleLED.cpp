#include "WProgram.h"
#include <toggleLED.h>
#include <controlLED.h>

// constructor
  toggleLED::toggleLED( int pin, int state ) {
    int _pin = pin;
    int _state = state;
    controlLED _ctrl( int _pin, int _state );
  } // constructor


int toggleLED::tggleLED( int _pin, int _state ) {
  
      // if the LED is off turn it on and vice-versa:
      if( _state == LOW ) {
        _state = HIGH;
      } else {
        _state = LOW;
      }
      // set the LED with the ledState of the variable:
//      digitalWrite( _pin, _state );  // using built-in library
      _ctrl->cntrolLED( _pin, _state );  // using add-on library
      
      return _state;
    
} // toggleLED


