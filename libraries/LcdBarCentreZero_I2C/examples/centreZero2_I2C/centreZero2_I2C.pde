/* centreZero_I2C - test LcdBarCentreZero library
 *
 * Created: Sept 20, 2011 - G. D. Young
 *
 * Revised: Oct 14/11 test with I2C connected display library
 *          Oct 19/11 working with lib
 *
 * Draws one max size bar graph on line 1 of 16 col 2 line CD
 *
 * Illustrates overwriting character generator memory (to modify
 * the centre zero marker from the library's 0 to a v)
 *
 * Note display updating is a little slower than with the parallel-
 * connected version in LiquidCrystal
 */


//#include <LiquidCrystal.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <LcdBarCentreZero_I2C.h>

#define MAXBARS 22        //length of display, either side

//LiquidCrystal lcd( 12, 11, 5, 4, 3, 2 ); // create LCD instance
LiquidCrystal_I2C lcd( 0x38, 16, 2 ); //instance at I2C port 38, 16 char, 2 rows

LcdBarCentreZero_I2C zcb( &lcd );     // create bar graph instance


byte lcdNumCols = 16;     // -- number of columns in the LCD
int barCount = 0;         // -- value to plot for this example
byte up = 1;

long interval = 150;      // interval at which to inc bar (milliseconds)
long previousMillis;
long randNr;

byte carat1[8] = {
  B00000,
  B00000,
  B01010,
  B00100,      // centre marker, alternate style
  B00000,
  B00100,
  B00100,
  B00100
};
byte carat2[8] = {
  B00000,
  B00000,
  B01010,
  B00100,      // centre marker plus right 1 bar
  B00001,
  B00101,
  B00101,
  B00101
};

byte carat3[8] = {
  B00000,
  B00000,
  B01010,
  B00100,      // centre marker plus left 1 bar
  B10000,
  B10100,
  B10100,
  B10100
};

void setup( ){
//  lcd.begin( 2, lcdNumCols );
  lcd.init();
  zcb.loadCG( );
  lcd.createChar( 1, carat1 );    //use alternate centre marker
  lcd.createChar( 2, carat2 );    //use alternate centre marker
  lcd.createChar( 3, carat3 );    //use alternate centre marker
  lcd.clear( );
  lcd.print( " LCD Bar Graph" );
  previousMillis = millis( );    // init interval timer for drawing
}

void loop( )
{
    if( millis( ) - previousMillis > interval ) {
      // save the last time you counted
      previousMillis = millis( );   

      // count up the number of bars to draw
      if( up ) {
        barCount += 1;
        if( barCount > MAXBARS ) {
          up = 0;
          barCount = MAXBARS;
        }
      } else {              // OR, count them down
        barCount -= 1;
        if( barCount < -MAXBARS ) {
          up = 1;
          barCount = -MAXBARS;
        }
      }

      zcb.drawBar( barCount , MAXBARS, 7, 1 );
      
    } // if time to count
}

