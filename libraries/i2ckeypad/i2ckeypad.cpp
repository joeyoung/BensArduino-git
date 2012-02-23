/*
 *  i2ckeypad.cpp v0.1 - keypad/I2C expander interface for Arduino
 *
 *  Copyright (c) 2009 Angel Sancho <angelitodeb@gmail.com>
 *  All rights reserved.
 *
 *  Original source from keypad v0.3 of Mark Stanley <mstanley@technologist.com>
 *  (http://www.arduino.cc/playground/Main/KeypadTutorial)
 *
 *
 *  Revised: Dec 2, 2011 - G. D. Young  Accommodate more than one keypad.
 *           - Header file corrected to include all private variables' 
 *             declarations.
 *           - Statics used by get_key( ) made into arrays so that each
 *             instance can have their own status information that must
 *             persist between calls for get_key to work.
 *           - Default #rows, #columns moved to constructor.
 *           - Provide for second keymap.
 *
 *           This revision is incomplete since the status arrays are only
 *           two deep, need a function argument with the object call, and
 *           probably shouldn't be using statics in this way.
 *
 *  LICENSE
 *  -------
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  EXPLANATION
 *  -----------
 *  This library is designed for use with PCF8574, but can possibly be
 *  adapted to other I2C port expanders
 *
 *  Wiring diagrams for PCF8574 and 4x3 keypad can be found under
 *  examples directory. Library runs correctly without cols pull-up
 *  resistors but it's better to use it
 *
 *  You can change pin connections between PCF8574 and keypad under
 *  PIN MAPPING section below
 *
 *  IMPORTANT! You have to call Wire.begin() before init() in your code
 *
 *  ... and sorry for my poor english!
 */

#include "i2ckeypad.h"
#include <Wire.h>

extern "C" {
  #include "WConstants.h"
}


/*
 *  PIN MAPPING
 *
 *  Here you can change your wire mapping between your keypad and PCF8574
 *  Default mapping is for sparkfun 4x3 keypad
 */

//#define COL0  2  // P2 of PCF8574, col0 is usually pin 3 of 4x3 keypads
//#define COL1  0  // P0 of PCF8574, col1 is usually pin 1 of 4x3 keypads
//#define COL2  4  // P4 of PCF8574, col2 is usually pin 5 of 4x3 keypads
//#define COL3  7  // sorry, don't have a 4x4 keypad to try it
//#define ROW0  1  // P1 of PCF8574, row0 is usually pin 2 of 4x3 keypads
//#define ROW1  6  // P6 of PCF8574, row1 is usually pin 7 of 4x3 keypads
//#define ROW2  5  // P5 of PCF8574, row2 is usually pin 6 of 4x3 keypads
//#define ROW3  3  // P3 of PCF8574, row3 is usually pin 4 of 4x3 keypads

// digitran 'plug-in' pin mapping
#define COL0 4
#define COL1 5
#define COL2 6
#define COL3 7
#define ROW0 0
#define ROW1 1
#define ROW2 2
#define ROW3 3


/*
 *  KEYPAD KEY MAPPING
 *
 *  Default key mapping for 4x4 keypads, you can change it here if you have or
 *  like different keys
 */

//const char keymap[4][5] =
//{
//  "123+",
//  "456-",
//  "789*",
//  "C0.="
//};


/*
 *  VAR AND CONSTANTS DEFINITION. Don't change nothing here
 *
 */

// Default row and col pin counts
//int num_rows = 4;
//int num_cols = 3;

// PCF8574 i2c address
//int pcf8574_i2c_addr;

// Current search row
//static int row_select;

// Current data set in PCF8574
//static int current_data;

// Hex byte statement for each port of PCF8574
const int hex_data[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

// Hex data for each row of keypad in PCF8574
const int pcf8574_row_data[4] = 
{
  hex_data[ROW1] | hex_data[ROW2] | hex_data[ROW3] |
  hex_data[COL0] | hex_data[COL1] | hex_data[COL2] | hex_data[COL3],
  hex_data[ROW0] | hex_data[ROW2] | hex_data[ROW3] |
  hex_data[COL0] | hex_data[COL1] | hex_data[COL2] | hex_data[COL3],
  hex_data[ROW0] | hex_data[ROW1] | hex_data[ROW3] |
  hex_data[COL0] | hex_data[COL1] | hex_data[COL2] | hex_data[COL3],
  hex_data[ROW0] | hex_data[ROW1] | hex_data[ROW2] |
  hex_data[COL0] | hex_data[COL1] | hex_data[COL2] | hex_data[COL3],
};

// Hex data for each col of keypad in PCF8574
int col[4] = {hex_data[COL0], hex_data[COL1], hex_data[COL2], hex_data[COL3]};


/*keymap
 *  CONSTRUCTORS
 */

//i2ckeypad::i2ckeypad(int addr)
//{
//  pcf8574_i2c_addr = addr;
//}

i2ckeypad::i2ckeypad( char* map, int addr, int r = 4, int c = 3 )
{
  keymap = map;

  pcf8574_i2c_addr = addr;
  num_rows = r;
  num_cols = c;
}

// Current search row
int i2ckeypad::row_select[2];

// Current data set in PCF8574
int i2ckeypad::current_data[2];

/*
 *  PUBLIC METHODS
 */

void i2ckeypad::init( int pad )
{
  // All PCF8574 ports high
  pcf8574_write(pcf8574_i2c_addr, 0xff, pad);

  // Start with the first row
  row_select[pad] = 0;
}
  
int i2ckeypad::temp_key[2];

char i2ckeypad::get_key( int pad )
{
//  static int temp_key;

  int tmp_data;
  int r;

  int key = '\0';

  // Search row low
  pcf8574_write(pcf8574_i2c_addr, pcf8574_row_data[row_select[pad]], pad);

  for(r=0;r<num_cols;r++) {
    // Read pcf8574 port data
    tmp_data = pcf8574_byte_read(pcf8574_i2c_addr);

    // XOR to compare obtained data and current data and know
    // if some column are low
    tmp_data ^= current_data[pad];

    // Key pressed!
    if(col[r] == tmp_data) {
      temp_key[pad] = keymap[row_select[pad]*(num_cols+1)+r];   //access map as 1D array
      return '\0';
    }
  }

  // Key was pressed and then released
  if((key == '\0') && (temp_key[pad] != '\0'))    
  {
    key = temp_key[pad];
    temp_key[pad] = '\0';
    return key;
  }

  // All PCF8574 ports high again
  pcf8574_write(pcf8574_i2c_addr, 0xff, pad);

  // Next row
  row_select[pad]++;
  if(row_select[pad] == num_rows) {
    row_select[pad] = 0;
  }

  return key;
}

/*
 *  PRIVATE METHODS
 */

void i2ckeypad::pcf8574_write(int addr, int data, int pad )
{
  current_data[pad] = data;

  Wire.beginTransmission(addr);
  Wire.send(data);
  Wire.endTransmission();
}

int i2ckeypad::pcf8574_byte_read(int addr)
{
  Wire.requestFrom(addr, 1);

  return Wire.receive();
}

