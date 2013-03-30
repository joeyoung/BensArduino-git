/*
|| @file Keypad_MAX.cpp - heavily modified version of Mark's Keypad
|| @author G. D. (Joe) Young - Feb 18/13 jyoung@islandnet.com
|| @description
|| | This modification gets keypad input from a MAX7359, 64-key,
|| | encoder IC, which has hardware scanning/debouncing of the key
|| | array and thus does not need Keypad's scanning capability.
|| | Instead, valid key activity is read from the I2C, FIFO output
|| | register on the chip.
||
|| @file Keypad.cpp
|| @version 3.1
|| @author Mark Stanley, Alexander Brevig
|| @contact mstanley@technologist.com, alexanderbrevig@gmail.com
||
|| @description
|| | This library provides a simple interface for using matrix
|| | keypads. It supports multiple keypresses while maintaining
|| | backwards compatibility with the old single key library.
|| | It also supports user selectable pins and definable keymaps.
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
#include <Keypad_MAX.h>

// configuration, i/o registers
#define FIFO 0x00		//MAX7359 keys fifo
#define CONFIG 0x01		//config
#define DEBNCE 0x02		//debounce
#define INTRPT 0x03		//interrupt
#define PORTS  0x04		//ports
#define KEYRPT 0x05		//key repeat
#define SLEEP  0x06		//sleep

// <<constructor>> Allows custom keymap, pin configuration, and keypad sizes.
Keypad_MAX::Keypad_MAX(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols, int i2caddr) {
	_i2caddr = i2caddr;
	portset = 0x17;       // column/port/debounce setup - needs user function

    rowPins = row;
    columnPins = col;
    sizeKpd.rows = numRows;
    sizeKpd.columns = numCols;

    begin(userKeymap);

    setDebounceTime(10);
    setHoldTime(500);
	keypadEventListener = 0;

	startTime = 0;
}

// Let the user define a keymap - assume the same row/column count as defined in constructor
void Keypad_MAX::begin(char *userKeymap) {
    keymap = userKeymap;
}

// Set up the MAX7359
void Keypad_MAX::begin( ) {
	TwoWire::begin( );
	TwoWire::beginTransmission( _i2caddr );
	TwoWire::write( DEBNCE );
  // maybe other registers to set
	TwoWire::write( portset );   // setup port pins for keypad in use
	TwoWire::endTransmission( );
	TwoWire::beginTransmission( _i2caddr );
	TwoWire::write( FIFO );     // point to keys fifo
	TwoWire::endTransmission( );
} // begin( )

// Returns a single key only. Retained for backwards compatibility.
char Keypad_MAX::getKey() {
	if (getKeys() && key[0].stateChanged && (key[0].kstate==PRESSED))
		return key[0].kchar;

	return NO_KEY;
}

// Populate the key list.
bool Keypad_MAX::getKeys() {
	bool keyActivity = false;

	// Limit how often the keypad is scanned. This makes the loop() run 10 times as fast.
	if ( (millis()-startTime)>debounceTime ) {
//		scanKeys();  // don't need for MAX7359
		TwoWire::requestFrom( _i2caddr, KEYS_MAX );  // instead, just read FIFO until empty
		keyActivity = updateList();
		startTime = millis();
	}

	return keyActivity;
}

#if 0
// Private : Hardware scan
void Keypad_MAX::scanKeys() {
	// Re-intialize the pins every time for sharing with other hardware.
	for (byte r=0; r<sizeKpd.rows; r++) {
		pin_mode(rowPins[r],INPUT_PULLUP);
	}

	// bitMap stores ALL the keys that are being pressed.
	for (byte c=0; c<sizeKpd.columns; c++) {
		pin_mode(columnPins[c],OUTPUT);
		pin_write(columnPins[c], LOW);	// Begin column pulse output.
		for (byte r=0; r<sizeKpd.rows; r++) {
			bitWrite(bitMap[r], c, !pin_read(rowPins[r]));  // keypress is active low so invert to high.
		}
		// Set pin to high impedance input. Effectively ends column pulse.
		pin_write(columnPins[c],HIGH);
		pin_mode(columnPins[c],INPUT);
	}
}
#endif

// Manage the list without rearranging the keys. Returns true if any keys on the list changed state.
bool Keypad_MAX::updateList() {

	bool anyActivity = false;

	// Delete IDLE keys
	for (byte i=0; i<LIST_MAX; i++) {
		if (key[i].kstate==IDLE) {
			key[i].kchar = NO_KEY;
			key[i].kcode = -1;
			key[i].stateChanged = false;
		}
	}

#if 0
	// Add new keys to empty slots in the key list.
	for (byte r=0; r<sizeKpd.rows; r++) {
		for (byte c=0; c<sizeKpd.columns; c++) {
			boolean button = bitRead(bitMap[r],c);
			char keyChar = keymap[r * sizeKpd.columns + c];
			int keyCode = r * sizeKpd.columns + c;
			int idx = findInList (keyCode);
			// Key was found on the list so set its next state.
			if (idx > -1) nextKeyState(idx, button);
			// Key is not on the list so add it.
			if ((idx == -1) && button) {
				for (byte i=0; i<LIST_MAX; i++) {
					if (key[i].kchar==NO_KEY) {		// Find an empty slot or don't add key to list.
						key[i].kchar = keyChar;
						key[i].kcode = keyCode;
						key[i].kstate = IDLE;		// Keys NOT on the list have an initial state of IDLE.
						nextKeyState (i, button);
						break;	// Don't fill all the empty slots with the same key.
					}
				}
			}
		}
	}
#endif
// MAX7359 does not need row/column examination of bitmap. Keycodes already have the
// row and column values, so just need to read from the Wire buffer filled in getKeys
	byte keyCode;
	do {
		keyCode = TwoWire::read( );
		if( keyCode == 0x3f ) {                    // if key fifo is empty
			for( byte r=0; r<sizeKpd.rows; r++ ) {
				for( byte c=0; c<sizeKpd.columns; c++ ) {
					// check list for all codes
					keyCode = rowPins[r] * sizeKpd.columns + columnPins[c];
					int idx = findInList( (int)keyCode );
					if( idx > -1 && key[idx].kstate != PRESSED ) {
						nextKeyState( (byte)idx, false );
						anyActivity = true;
					}
//					if( idx > -1 && (key[idx].kstate == PRESSED 
//								|| key[idx].kstate == HOLD) ) {  // this gets HOLD noticed
//						nextKeyState( (byte)idx, CLOSED );            // but causes release if held
//						anyActivity = true;
//					}
				} // all columns
			} // all rows
			return anyActivity;
		}
		bool button = ( (keyCode&0x40) != 0x40 );      // bit6 = 1 ==>release
		byte krow = rowPins[(keyCode&0x3f)%8];
		byte kcol = columnPins[(keyCode&0x3f)/8];
		char keyChar = keymap[krow * sizeKpd.columns + kcol];
		int idx = findInList( (int)(keyCode&0x3f) );
		// key already on list
		if( idx > -1 ) nextKeyState( (byte)idx, button );
		// key needs to be added
		if( (idx == -1) && button ) {
			for( byte i=0; i<LIST_MAX; i++ ) {
				if( key[i].kchar == NO_KEY ) {  // find empty slot
					key[i].kchar = keyChar;
					key[i].kcode = keyCode&0x3f;
					key[i].kstate = IDLE;       // init entry to IDLE
					nextKeyState( i, button );  // set to button (PRESSED)
					break;
				} // if list slot empty
			} // for all list
		} // if not on list
	} while( (keyCode&0x80) == 0x80 );    // last valid key read from fifo has clear bit7

	// Report if the user changed the state of any key.
	for (byte i=0; i<LIST_MAX; i++) {
		if (key[i].stateChanged) anyActivity = true;
	}

	return anyActivity;
}

// Private
// This function is a state machine but is also used for debouncing the keys.
void Keypad_MAX::nextKeyState(byte idx, boolean button) {
	key[idx].stateChanged = false;

	switch (key[idx].kstate) {
		case IDLE:
			if (button==CLOSED) {
				transitionTo (idx, PRESSED);
				holdTimer = millis(); }		// Get ready for next HOLD state.
			break;
		case PRESSED:
			if ((millis()-holdTimer)>holdTime)	// Waiting for a key HOLD...
				transitionTo (idx, HOLD);
			else if (button==OPEN)				// or for a key to be RELEASED.
				transitionTo (idx, RELEASED);
			break;
		case HOLD:
			if (button==OPEN)
				transitionTo (idx, RELEASED);
			break;
		case RELEASED:
			transitionTo (idx, IDLE);
			break;
	}
}

// New in 2.1
bool Keypad_MAX::isPressed(char keyChar) {
	for (byte i=0; i<LIST_MAX; i++) {
		if ( key[i].kchar == keyChar ) {
			if ( (key[i].kstate == PRESSED) && key[i].stateChanged )
				return true;
		}
	}
	return false;	// Not pressed.
}

// Search for a key (by character) in the list of active keys.
// Returns -1 if not found or the index into the list of active keys.
int Keypad_MAX::findInList (char keyChar) {
	for (byte i=0; i<LIST_MAX; i++) {
		if (key[i].kchar == keyChar) {
			return i;
		}
	}
	return -1;
}

// Search for a key (by code) in the list of active keys.
// Returns -1 if not found or the index into the list of active keys.
int Keypad_MAX::findInList (int keyCode) {
	for (byte i=0; i<LIST_MAX; i++) {
		if (key[i].kcode == keyCode) {
			return i;
		}
	}
	return -1;
}

// New in 2.0
char Keypad_MAX::waitForKey() {
	char waitKey = NO_KEY;
	while( (waitKey = getKey()) == NO_KEY );	// Block everything while waiting for a keypress.
	return waitKey;
}

// Backwards compatibility function.
KeyState Keypad_MAX::getState() {
	return key[0].kstate;
}

// The end user can test for any changes in state before deciding
// if any variables, etc. needs to be updated in their code.
bool Keypad_MAX::keyStateChanged() {
	return key[0].stateChanged;
}

// The number of keys on the key list, key[LIST_MAX], equals the number
// of bytes in the key list divided by the number of bytes in a Key object.
byte Keypad_MAX::numKeys() {
	return sizeof(key)/sizeof(Key);
}

// Minimum debounceTime is 10 mS. Any lower *will* slow down the loop().
void Keypad_MAX::setDebounceTime(uint debounce) {
	debounce<1 ? debounceTime=1 : debounceTime=debounce;
}

void Keypad_MAX::setHoldTime(uint hold) {
    holdTime = hold;
}

void Keypad_MAX::addEventListener(void (*listener)(char)){
	keypadEventListener = listener;
}

void Keypad_MAX::transitionTo(byte n, KeyState nextState) {
	key[n].kstate = nextState;
	key[n].stateChanged = true;

	if (keypadEventListener!=NULL)
		keypadEventListener(key[0].kchar);
}

/*
|| @changelog
|| | 1.0 2013-02-18 - Joe Young Keypad_MAX version initial release derived from Keypad 3.1
|| | 3.1 2013-01-15 - Mark Stanley     : Fixed missing RELEASED & IDLE status when using a single key.
|| #
*/
