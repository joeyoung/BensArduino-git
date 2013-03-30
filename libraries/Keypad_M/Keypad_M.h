#ifndef KEYPAD_MAX_H
#define KEYPAD_MAX_H

#include "Arduino.h"
#include "Wire.h"

#define makeKeymap(x) ((char*)x)
#define MAXCODES 0x20


class Keypad_M : public TwoWire {

public:
 // constructor for Keypad_M
	Keypad_M(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols, int i2caddr);



	// Wire function
	void begin( );

//	void pin_mode(byte pinNum, byte mode);
//	void pin_write(byte pinNum, boolean level);
//	int  pin_read(byte pinNum);
	// read initial value for pinState
//	word pinState_set( );
	// write a whole word to i2c port
//	void port_write( word i2cportval );
	// access functions for IODIR state copy
//	word iodir_read( );
//	void iodir_write( word iodir );

	// MAX7359 access
	char getKeys( );
	char readKeys( char keyn );

private:
    // I2C device address
	int _i2caddr;
	byte _rows;
	byte _columns;
	byte* _rowN;
	byte* _colN;
	char *_keymap;
	byte portset;

	//array of keycodes read
	char keycodes[ MAXCODES ];

	// I2C pin_write state persistant storage
//	word pinState;
//	byte pin_iosetup( );
	// MC16 setup
//	word iodir_state;    // copy of IODIR register
	void _begin( void );
};


#endif // KEYPAD_MAX_H

/*
|| @changelog
|| |
|| | 1.0 2013-02-14 - Joe Young : Start
|| #
*/

