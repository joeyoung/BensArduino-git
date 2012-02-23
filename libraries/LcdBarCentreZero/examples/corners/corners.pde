/* right and left only bars - test LcdBarCentreZero library
 *
 * Created: Sept 20, 2011 - G. D. Young
 *
 * Draws 4 bar graphs in corners of 16 col 2 line display
 */


#include <LiquidCrystal.h>
#include <LcdBarCentreZero.h>

#define MAXBARS 20        //length of bar to draw

LiquidCrystal lcd( 12, 11, 5, 4, 3, 2 ); // create LCD instance

LcdBarCentreZero zcb( &lcd );     // create bar graph instance


byte lcdNumCols = 16;     // -- number of columns in the LCD
int barCount = 0;         // -- value to plot for this example
byte up = 1;

long interval = 150;      // interval at which to inc bar (milliseconds)
long previousMillis;
long randNr;


void setup( ){
  lcd.begin( 2, lcdNumCols );
  lcd.clear( );
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
      randNr = random( -MAXBARS, 1 );    // use random number to show blanking
      zcb.drawBar( abs( barCount ), MAXBARS, 0, 0 );
      zcb.drawBar( (int)randNr, MAXBARS, 15, 0 );
      zcb.drawBar( -abs( barCount ), MAXBARS, 15, 1 );    //short bar to show clipping to max
      zcb.drawBar( (int)-randNr, 8, 0, 1 );
      
    } // if time to count
}

