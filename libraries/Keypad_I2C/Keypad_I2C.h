/*
   Keypad_I2C.h - modification of Keypad library to use I2C I/O
   started: Feb 26/12 - G. D. Young
||
|| @file Keypad.h
|| @version 2.0
|| @author Mark Stanley, Alexander Brevig
|| @contact mstanley@technologist.com, alexanderbrevig@gmail.com
||
|| @description
|| | This library provides a simple interface for using matrix
|| | keypads. It supports the use of multiple keypads with the
|| | same or different sets of keys.  It also supports user
|| | selectable pins and definable keymaps.
|| #
||
|| @license
|| | This library is free software; you can redistribute it and/or
|| | modify it under the terms of the GNU Lesser General Public
|| | License as published by the Free Software Foundation; version
|| | 2.1 of the License.
|| |
|| | This library is distributed in the hope that it will be useful,
|| | but WITHOUT ANY WARRANTY; without even the implied warranty of
|| | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| | Lesser General Public License for more details.
|| |
|| | You should have received a copy of the GNU Lesser General Public
|| | License along with this library; if not, write to the Free Software
|| | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/

#ifndef KEYPAD_I2C_H
#define KEYPAD_I2C_H

// Arduino versioning.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"	// for digitalRead, digitalWrite, etc
#else
#include "WProgram.h"
#endif

#include <Wire.h>       // replacing digitalRead and digitalWrite
// must be in sketch??


#define OFF LOW
#define ON HIGH

#define CLOSED LOW
#define OPEN HIGH

#define makeKeymap(x) ((char*)x)

typedef char KeypadEvent;

typedef enum {IDLE, PRESSED, HOLD, RELEASED} KeyState;  // KeyState was KeypadState

// Made changes according to this post http://arduino.cc/forum/index.php?topic=58337.0
// by Nick Gammon. Thanks for the input Nick. :)  It actually saved 78 bytes for me.
typedef struct {
    byte rows;
    byte columns;
} KeypadSize;

const char NO_KEY = '\0';
#define KEY_RELEASED NO_KEY

class Keypad_I2C {
public:
	Keypad_I2C( char *userKeymap, byte *row, byte *col, byte numRows, byte numCols, byte i2caddr );

	void begin(char *userKeymap);
	char getKey();
	KeyState getState();
	void setDebounceTime(unsigned int);
	void setHoldTime(unsigned int);
	void addEventListener(void (*listener)(char));
	// New methods
	char waitForKey();
	boolean keyStateChanged();

private:
	void transitionTo(KeyState);
	void initializePins(  );

	char *keymap;
    byte *rowPins;
    byte *columnPins;
	KeypadSize size;
	KeyState state;    // I2C i/o functions to mimic digitalWrite, digitalRead

	char currentKey;
	unsigned int debounceTime;
	unsigned int holdTime;
	void (*keypadEventListener)(char);

	// New methods - 2011-12-23
	boolean scanKeys();
	KeyState getKeyState();
    // I2C i/o functions to mimic digitalWrite, digitalRead
    void I2CxWrite( byte, byte );
	byte I2CxRead( byte );

	// New members - 2011-12-23
	boolean buttons;
	boolean stateChanged;
    // I2C device address
    byte i2caddr;

};

#endif

/*
|| @changelog
|| | 2012-02-27 - Joe Young - using I2C i/o--I2CxWrite(), I2CxRead() instead of digitalWrite,Read
|| | 2.0 2011-12-29 - Mark Stanley     : Added waitForKey().
|| | 2.0 2011-12-23 - Mark Stanley     : Added the public function keyStateChanged().
|| | 2.0 2011-12-23 - Mark Stanley     : Added the private function scanKeys().
|| | 2.0 2011-12-23 - Mark Stanley     : Moved the Finite State Machine into the function getKeyState().
|| | 2.0 2011-12-23 - Mark Stanley     : Removed the member variable lastUdate. Not needed after rewrite.
|| | 1.8 2011-11-21 - Mark Stanley     : Added test to determine which header file to compile,
|| |                                          WProgram.h or Arduino.h.
|| | 1.8 2009-07-08 - Alexander Brevig : No longer uses arrays
|| | 1.7 2009-06-18 - Alexander Brevig : This library is a Finite State Machine every time a state changes
|| |                                          the keypadEventListener will trigger, if set
|| | 1.7 2009-06-18 - Alexander Brevig : Added setDebounceTime setHoldTime specifies the amount of
|| |                                          microseconds before a HOLD state triggers
|| | 1.7 2009-06-18 - Alexander Brevig : Added transitionTo
|| | 1.6 2009-06-15 - Alexander Brevig : Added getState() and state variable
|| | 1.5 2009-05-19 - Alexander Brevig : Added setHoldTime()
|| | 1.4 2009-05-15 - Alexander Brevig : Added addEventListener
|| | 1.3 2009-05-12 - Alexander Brevig : Added lastUdate, in order to do simple debouncing
|| | 1.2 2009-05-09 - Alexander Brevig : Changed getKey()
|| | 1.1 2009-04-28 - Alexander Brevig : Modified API, and made variables private
|| | 1.0 2007-XX-XX - Mark Stanley : Initial Release
|| #
*/
