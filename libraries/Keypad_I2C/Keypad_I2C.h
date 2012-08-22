#ifndef KEYPAD_I2C_H
#define KEYPAD_I2C_H

#include "Keypad.h"
//#include "../Wire/Wire.h"
#include "Wire.h"

class Keypad_I2C : public Keypad, public TwoWire {
public:
	Keypad_I2C(char* userKeymap, byte* row, byte* col, byte numRows, byte numCols, byte address) :
		Keypad(userKeymap, row, col, numRows, numCols) { i2caddr = address; }

	// Keypad function
	void begin(char *userKeymap);
	// Wire function
	void begin(void);
	// Wire function
	void begin(byte address);
	// Wire function
	void begin(int address);

	void pin_mode(byte pinNum, byte mode) {}
	void pin_write(byte pinNum, boolean level);
	int  pin_read(byte pinNum);
	// read initial value for pinState
	byte pinState_set( );
	// write a whole byte to i2c port
	void port_write( byte i2cportval );

private:
    // I2C device address
    byte i2caddr;
	// I2C pin_write state persistant storage
	byte pinState;
};



#endif // KEYPAD_I2C_H
