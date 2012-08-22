#include "Keypad_I2C.h"

/////// Extended Keypad library functions. ////////////////////////////

// Let the user define a keymap - assume the same row/column count as defined in constructor
void Keypad_I2C::begin(char *userKeymap) {
    Keypad::begin(userKeymap);
	TwoWire::begin();
	pinState = pinState_set( );
}


/////// Extended Keypad_I2C library functions. ////////////////////////

// Initialize I2C
void Keypad_I2C::begin(void) {
	TwoWire::begin();
//	pinState = 0xff;
	pinState = pinState_set( );
}

// Initialize I2C
void Keypad_I2C::begin(byte address) {
	i2caddr = address;
	TwoWire::begin(address);
//	pinState = 0xff;
	pinState = pinState_set( );
}

// Initialize I2C
void Keypad_I2C::begin(int address) {
	i2caddr = address;
	TwoWire::begin(address);
//	pinState = 0xff;
	pinState = pinState_set( );
}


void Keypad_I2C::pin_write(byte pinNum, boolean level) {
//	static byte pinState;  // allocated in constructor GDY120704
	byte mask = 1<<pinNum;
	if( level == HIGH ) {
		pinState |= mask;
	} else {
		pinState &= ~mask;
	}
//	TwoWire::beginTransmission((int)i2caddr);
//	TwoWire::write( pinState );
//	TwoWire::endTransmission();
	port_write( pinState );
} // I2CxWrite( )


int Keypad_I2C::pin_read(byte pinNum) {
	byte mask = 0x1<<pinNum;
	TwoWire::requestFrom((int)i2caddr, 1);
	byte pinVal = TwoWire::read( );
	pinVal &= mask;
	if( pinVal == mask ) {
		return 1;
	} else {
		return 0;
	}
}

void Keypad_I2C::port_write( byte i2cportval ) {
	TwoWire::beginTransmission((int)i2caddr);
	TwoWire::write( i2cportval );
	TwoWire::endTransmission();
	pinState = i2cportval;
} // port_write( )

byte Keypad_I2C::pinState_set( ) {
	TwoWire::requestFrom( (int)i2caddr, 1 );
	pinState = TwoWire::read( );
	return pinState;
} // set_pinState( )

