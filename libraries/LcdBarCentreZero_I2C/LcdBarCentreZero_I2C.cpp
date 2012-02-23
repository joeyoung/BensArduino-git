/* LcdBarCentreZero_I2C Lcd centre-zero bargraph library
 *
 * Created: Sept 19, 2011 - G. D. Young
 *
 * Revised: Sept 20/11 - add blank bar display area before drawing
 *          Sept 20/11 - create library version
 *          Oct 14/11 - version for I2C connected display
 *
 * Use LiquidCrystal library for drawing special bar-graph characters
 *
 * Similar to LcdBarGraph from Balazs Keleman; differences:
 *    - no scaling inside bar drawing - caller must scale to graph size
 *    - bars not solid, appear uniform across character columns
 *    - can draw several bar graphs on each of any line of display
 *    - because both pos and neg display, bars can grow to left or 
 *      right or both
 */

#include "WProgram.h"
#include "LcdBarCentreZero_I2C.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* Graphic characters to load into display's character generator
 * memory via lcd.createChar()
 */
byte LcdBarCentreZero_I2C::_gchar0[8] = {
  B00000,
  B00000,
  B00000,
  B00000,      // 3 bar full character
  B10101,
  B10101,
  B10101,
  B10101
};

byte LcdBarCentreZero_I2C::_gchar1[8] = {
  B00100,
  B01010,
  B01010,
  B00100,      // centre marker, alternate style
  B00000,
  B00100,
  B00100,
  B00100
};

byte LcdBarCentreZero_I2C::_gchar2[8] = {
  B00100,
  B01010,
  B01010,
  B00100,      // centre marker plus right 1 bar
  B00001,
  B00101,
  B00101,
  B00101
};

byte LcdBarCentreZero_I2C::_gchar3[8] = {
  B00100,
  B01010,
  B01010,
  B00100,      // centre marker plus left 1 bar
  B10000,
  B10100,
  B10100,
  B10100
};

byte LcdBarCentreZero_I2C::_gchar4[8] = {
  B00000,
  B00000,
  B00000,
  B00000,      // right 1 bar (second bar of graph)
  B10000,
  B10000,
  B10000,
  B10000
};

byte LcdBarCentreZero_I2C::_gchar5[8] = {
  B00000,
  B00000,
  B00000,
  B00000,      // left 1 bar (second bar of graph)
  B00001,
  B00001,
  B00001,
  B00001
};

byte LcdBarCentreZero_I2C::_gchar6[8] = {
  B00000,
  B00000,
  B00000,
  B00000,      // right 2 bar (third and 4th)
  B10100,
  B10100,
  B10100,
  B10100
};

byte LcdBarCentreZero_I2C::_gchar7[8] = {
  B00000,
  B00000,
  B00000,
  B00000,      // left 2 bar (third and 4th)
  B00101,
  B00101,
  B00101,
  B00101
};

//constructor
LcdBarCentreZero_I2C::LcdBarCentreZero_I2C( LiquidCrystal_I2C* lic2 ) {

   _lcdi2c = lic2;

} //constructor

void LcdBarCentreZero_I2C::loadCG( void ) {

//  _lcdi2c->init( );
  _lcdi2c->createChar( 0, _gchar0 );  //fill special character memory
  _lcdi2c->createChar( 1, _gchar1 );
  _lcdi2c->createChar( 2, _gchar2 );
  _lcdi2c->createChar( 3, _gchar3 );
  _lcdi2c->createChar( 4, _gchar4 );
  _lcdi2c->createChar( 5, _gchar5 );
  _lcdi2c->createChar( 6, _gchar6 );
  _lcdi2c->createChar( 7, _gchar7 );
  
} //loadCG


void LcdBarCentreZero_I2C::drawBar( int value, byte maxbars, byte centre, byte line ) {

  byte _neg;
  byte _colsToBlank;

  // clear entire display area of any previous bars
  _colsToBlank = ( ( maxbars+1 )/3 ) * 2 + 1;
  _lcdi2c->setCursor( centre - _colsToBlank/2, line );
  _lcdi2c->leftToRight( );
  for( byte i=0; i<_colsToBlank; i++ ) _lcdi2c->write( ' ' );
  
  _lcdi2c->setCursor( centre, line );
  if( value < 0 ) {
    _lcdi2c->rightToLeft( );
    _neg = 1;
    value = -value;
  } else {
    _lcdi2c->leftToRight( );
    _neg = 0;
  }
  if( value > (int)maxbars ) value = (int)maxbars;
  if( value == 0 ) {
    _lcdi2c->write( 1 );      // only put centre marker
   }
  if( value > 0 ) {
    if( _neg ) {
      _lcdi2c->write( 3 );
    } else {             //centre plus one bar
      _lcdi2c->write( 2 );
    }
    value -= 1;
  } // if value > 0
  
  byte _num3bar = value/3;  // number of 3 bar characters
  for( byte i=0; i < _num3bar; i++ ) {
    _lcdi2c->write( 0 );
  }
  if( value - _num3bar * 3 == 1 ) {
    if( _neg ) {
      _lcdi2c->write( 5 );
    } else {
      _lcdi2c->write( 4 );
    }
  } // if leftover number = 1
  if( value - _num3bar * 3 == 2 ) {
    if( _neg ) {
      _lcdi2c->write( 7 );
    } else {
      _lcdi2c->write( 6 );
    }
  } // if leftover number = 2
  
} // drawBar



