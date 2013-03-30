#include "Keypad_M.h"

// configuration, i/o registers
#define FIFO 0x00		//MAX7359 keys fifo
#define CONFIG 0x01		//config
#define DEBNCE 0x02		//debounce
#define INTRPT 0x03		//interrupt
#define PORTS  0x04		//ports
#define KEYRPT 0x05		//key repeat
#define SLEEP  0x06		//sleep

// constructor
Keypad_M::Keypad_M( char *userKeymap, byte *row, byte *col, byte numRows, byte numCols, int i2caddr ) {
	_i2caddr = i2caddr;
	_rowN = row;			// array of row numbers used for keypad
	_colN = col;			// array of col numbers used for keypad
    _rows = numRows;
    _columns = numCols;
	_keymap = userKeymap;
//    _begin( );

	portset = 0x17;
//	setDebounceTime(10);
//	setHoldTime(500);
//	keypadEventListener = 0;

} // constructor

void Keypad_M::begin( ) {
	TwoWire::begin();
	_begin( );
}

char Keypad_M::getKeys( ) {
	char numKeys = 0;
	TwoWire::requestFrom( _i2caddr, 8 );
	keycodes[numKeys] = TwoWire::read( );
	if( keycodes[0] == 0x3f ) return 0;      // if key fifo is empty
	while( keycodes[numKeys] != 0x3f ) {
//	while( numKeys < 2 ) {
		numKeys++;
		keycodes[numKeys] = TwoWire::read( );
	} // while keycodes to read
	return numKeys;
} // getKeys

char Keypad_M::readKeys( char keyn ) {
	return keycodes[keyn];
} // readKeys( )

// Initialize MAX7359
void Keypad_M::_begin( void ) {
	TwoWire::beginTransmission( _i2caddr );
	TwoWire::write( DEBNCE );
  // maybe other registers to set
	TwoWire::write( portset );   // setup port pins for keypad in use
	TwoWire::endTransmission( );
	TwoWire::beginTransmission( _i2caddr );
	TwoWire::write( FIFO );     // point to keys fifo
	TwoWire::endTransmission( );
} // _begin( )


#if 0  // may want this for GPO pin write
void Keypad_MC16::pin_write(byte pinNum, boolean level) {
	word mask = 1<<pinNum;
	if( level == HIGH ) {
		pinState |= mask;
	} else {
		pinState &= ~mask;
	}
	port_write( pinState );
} // MC16xWrite( )

void Keypad_MC16::port_write( word i2cportval ) {
// MCP23016 requires a register address on each write
	TwoWire::beginTransmission((int)i2caddr);
	TwoWire::write( GP0 );
	TwoWire::write( lowByte( i2cportval ) );
	TwoWire::write( highByte( i2cportval ) );
	TwoWire::endTransmission();
	pinState = i2cportval;
} // port_write( )
#endif

/*
|| @changelog
|| |
|| | 1.0 2013-02-14 - Joe Young : start
|| #
*/
