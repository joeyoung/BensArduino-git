/* centreZero - test LcdBarCentreZero library
 *
 * Created: Sept 20, 2011 - G. D. Young
 *
 * Draws one max size bar graph on line 1 of 16 col 2 line CD
 */


#include <LiquidCrystal.h>
#include <LcdBarCentreZero.h>

#define MAXBARS 22        //length of display, either side

LiquidCrystal lcd( 12, 11, 5, 4, 3, 2 ); // create LCD instance

LcdBarCentreZero zcb( &lcd );     // create bar graph instance


byte lcdNumCols = 16;     // -- number of columns in the LCD
int barCount = 0;         // -- value to plot for this example
byte up = 1;

long interval = 50;      // interval at which to inc bar (milliseconds)
long previousMillis;
long randNr;


void setup( ){
  lcd.begin( 2, lcdNumCols );
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

