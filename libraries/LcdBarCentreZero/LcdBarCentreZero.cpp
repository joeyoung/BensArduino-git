/* LcdBarCentreZero Lcd centre-zero bargraph library
 *
 * Created: Sept 19, 2011 - G. D. Young
 *
 * Revised: Sept 20/11 - add blank bar display area before drawing
 *          Sept 20/11 - create library version
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
#include "LcdBarCentreZero.h"
 
#include <LiquidCrystal.h>

/* Graphic characters to load into display's character generator
 * memory via lcd.createChar()
 */
byte LcdBarCentreZero::_gchar0[8] = {
  B00000,
  B00000,
  B00000,
  B00000,      // 3 bar full character
  B10101,
  B10101,
  B10101,
  B10101
};

byte LcdBarCentreZero::_gchar1[8] = {
  B00100,
  B01010,
  B01010,
  B00100,      // centre marker, alternate style
  B00000,
  B00100,
  B00100,
  B00100
};

byte LcdBarCentreZero::_gchar2[8] = {
  B00100,
  B01010,
  B01010,
  B00100,      // centre marker plus right 1 bar
  B00001,
  B00101,
  B00101,
  B00101
};

byte LcdBarCentreZero::_gchar3[8] = {
  B00100,
  B01010,
  B01010,
  B00100,      // centre marker plus left 1 bar
  B10000,
  B10100,
  B10100,
  B10100
};

byte LcdBarCentreZero::_gchar4[8] = {
  B00000,
  B00000,
  B00000,
  B00000,      // right 1 bar (second bar of graph)
  B10000,
  B10000,
  B10000,
  B10000
};

byte LcdBarCentreZero::_gchar5[8] = {
  B00000,
  B00000,
  B00000,
  B00000,      // left 1 bar (second bar of graph)
  B00001,
  B00001,
  B00001,
  B00001
};

byte LcdBarCentreZero::_gchar6[8] = {
  B00000,
  B00000,
  B00000,
  B00000,      // right 2 bar (third and 4th)
  B10100,
  B10100,
  B10100,
  B10100
};

byte LcdBarCentreZero::_gchar7[8] = {
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
LcdBarCentreZero::LcdBarCentreZero( LiquidCrystal* lcd ) {

  _lcd = lcd;
  _lcd->createChar( 0, this->_gchar0 );  //fill special character memory
  _lcd->createChar( 1, this->_gchar1 );
  _lcd->createChar( 2, this->_gchar2 );
  _lcd->createChar( 3, this->_gchar3 );
  _lcd->createChar( 4, this->_gchar4 );
  _lcd->createChar( 5, this->_gchar5 );
  _lcd->createChar( 6, this->_gchar6 );
  _lcd->createChar( 7, this->_gchar7 );
  

} //constructor


void LcdBarCentreZero::drawBar( int value, byte maxbars, byte centre, byte line ) {

  byte _neg;
  byte _colsToBlank;

  // clear entire display area of any previous bars
  _colsToBlank = ( ( maxbars+1 )/3 ) * 2 + 1;
  _lcd->setCursor( centre - _colsToBlank/2, line );
  _lcd->leftToRight( );
  for( byte i=0; i<_colsToBlank; i++ ) _lcd->write( ' ' );
  
  _lcd->setCursor( centre, line );
  if( value < 0 ) {
    _lcd->rightToLeft( );
    _neg = 1;
    value = -value;
  } else {
    _lcd->leftToRight( );
    _neg = 0;
  }
  if( value > (int)maxbars ) value = (int)maxbars;
  if( value == 0 ) {
    _lcd->write( 1 );      // only put centre marker
   }
  if( value > 0 ) {
    if( _neg ) {
      _lcd->write( 3 );
    } else {             //centre plus one bar
      _lcd->write( 2 );
    }
    value -= 1;
  } // if value > 0
  
  byte _num3bar = value/3;  // number of 3 bar characters
  for( byte i=0; i < _num3bar; i++ ) {
    _lcd->write( 0 );
  }
  if( value - _num3bar * 3 == 1 ) {
    if( _neg ) {
      _lcd->write( 5 );
    } else {
      _lcd->write( 4 );
    }
  } // if leftover number = 1
  if( value - _num3bar * 3 == 2 ) {
    if( _neg ) {
      _lcd->write( 7 );
    } else {
      _lcd->write( 6 );
    }
  } // if leftover number = 2
  
} // drawBar



