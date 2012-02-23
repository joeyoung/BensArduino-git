/* twoBar_I2C - Two bargraphs on one line
 *
 * Created: Oct 23, 2011 - G. D. Young
 *
 * Revised: Oct 24/11 - left growing bars
 *
 * Draws two bar graphs - top and bottom on any line of character LCD
 *
 *
 */

// should have a display definition in symbols
// eg:
//    #define LCD_COLS 16
//    #define LCD_LINES 2


//#include <LiquidCrystal.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define MAXBARS 15        //length of display
#define TOP 0
#define BOTTOM 1
#define RIGHT 0
#define LEFT 1

//LiquidCrystal lcd( 12, 11, 5, 4, 3, 2 ); // create LCD instance
LiquidCrystal_I2C lcd( 0x38, 16, 2 ); //instance at I2C port 38, 16 char, 2 rows


byte lcdNumCols = 16;     // -- number of columns in the LCD
int barCount = 0;         // -- value to plot for this example
byte up = 1;
byte barOrigin = 5;       // character position for start of bars
const byte barLine = 1;   // line dual bar is on
//const byte origChar = '0';      // character to mark graph origin
const byte origChar = 0xff;      // character to mark graph origin
const byte topBar = TOP;
const byte bottomBar = BOTTOM;
const byte barDirec = RIGHT;

long interval = 150;      // interval at which to inc bar (milliseconds)
long previousMillis;
long randNr;

// structure of each character in barImage:
// 76543210 bit #
// 0000ttbb
//      ^ ^
//      | +--bottom bar bits: 00 --> no bar, 10 left, 11 both
//      +----top bar bits: 00 --> no bar, 10 left, 11 both

static byte barImage[ (MAXBARS+1)/2 ]; // in-memory image of dual graph

// map of barImage entries to graphic characters:
const byte twobarMap[16] = {
            ' ',       // 0000 (neither has bar segment), use blank char
            'x',       // 0001 not possible
              0,       // 0010 bottom left only
              1,       // 0011 both bottom
            'x',       // 0100 not possible (illegal top)
            'x',       // 0101 not possible (illegal top)
            'x',       // 0110 not possible (illegal top)
            'x',       // 0111 not possible (illegal top)
              2,       // 1000 left top only
            'x',       // 1001 not possible (illegal bottom)
              3,       // 1010 left in both
              4,       // 1011 left top, both bottom
              5,       // 1100 both top only
            'x',       // 1101 not possible (illegal bottom)
              6,       // 1110 both top, left bottom
              7        // 1111 both top and bottom
};  // twobarMap
            

byte twobar0[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11000,       // 0010 bottom left only
  B11000,
  B11000
};
byte twobar1[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11011,       // 0011 both bottom
  B11011,
  B11011
};

byte twobar2[8] = {
  B11000,       // 1000 left top only
  B11000,
  B11000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte twobar3[8] = {
  B11000,
  B11000,
  B11000,
  B00000,       // 1010 left in both
  B00000,
  B11000,
  B11000,
  B11000
};

byte twobar4[8] = {
  B11000,
  B11000,
  B11000,
  B00000,       // 1011 left top, both bottom
  B00000,
  B11011,
  B11011,
  B11011
};

byte twobar5[8] = {
  B11011,       // 1100 both top only
  B11011,
  B11011,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte twobar6[8] = {
  B11011,
  B11011,
  B11011,
  B00000,       // 1110 both top, left bottom
  B00000,
  B11000,
  B11000,
  B11000
};

byte twobar7[8] = {
  B11011,
  B11011,
  B11011,
  B00000,       // 1110 both top and bottom
  B00000,
  B11011,
  B11011,
  B11011
};

// drawBar function to draw either of the two bars
//  - uses static array barImage[ size of graph ] to keep image of
//    the display so each bar can be drawn independently in the image
//
void drawBar( byte bar, byte barLength, byte maxLength, byte origin, byte line ) {
  
  byte colsToBlank;
  byte fullChars, single;
  
  // calculate number of full bar characters, check for extra at end
  fullChars = barLength/2;
  single = barLength - 2*fullChars;
  
  // move along the barImage array, setting bits for the bar being drawn
  if( bar == TOP ) {  // top bar of the pair
 
    for( byte i=0; i<fullChars; i++ ) {
      barImage[i] = barImage[i] | 0xc;  // b00001100;
    } // full characters
    if( single ) {
      barImage[fullChars] = barImage[fullChars] & 0xf3; // b11110011;
      barImage[fullChars] = barImage[fullChars] | 0x8;  // b00001000;
      fullChars += 1;
    } // insert single bar seg if needed
    for( byte i=fullChars; i<(maxLength+1)/2; i++ ) barImage[i] = barImage[i] & 0xf3;
 
  } else {
 
    for( byte i=0; i<fullChars; i++ ) {
      barImage[i] = barImage[i] | 0x3;  // b00000011;
    } // full characters
    if( single ) {
      barImage[fullChars] = barImage[fullChars] & 0xfc;  // b11111100;
      barImage[fullChars] = barImage[fullChars] | 0x2;   // b00000010;
      fullChars +=1;
    } // insert single bar seg if needed
    for( byte i=fullChars; i<(maxLength+1)/2; i++ ) barImage[i] = barImage[i] & 0xfc;
 
   } // if bar to draw is top

  // clear entire display area of any previous bars
  colsToBlank = ( maxLength+1 )/2;
  lcd.setCursor( origin, line );
  lcd.leftToRight( );
  lcd.write( origChar );
 // for( byte i=0; i<colsToBlank; i++ ) lcd.write( ' ' );
  
  lcd.setCursor( origin+1, line );
  for( byte i=0; i<colsToBlank; i++ ) {
    lcd.write( twobarMap[ barImage[i] ] );
  } // write the dual bar image
  
  
} // drawBar

void setup( ){
//  lcd.begin( 2, lcdNumCols );
  lcd.init();
  lcd.createChar( 0, twobar0 );  // load up graphic character memory
  lcd.createChar( 1, twobar1 );
  lcd.createChar( 2, twobar2 );
  lcd.createChar( 3, twobar3 );
  lcd.createChar( 4, twobar4 );
  lcd.createChar( 5, twobar5 );
  lcd.createChar( 6, twobar6 );
  lcd.createChar( 7, twobar7 );
  lcd.clear( );
  lcd.print( " Dual Bar Graph" );
  lcd.setCursor( barOrigin-4, barLine );
  lcd.print( "L/R" );            // example label for bars
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
        if( barCount < 0 ) {
          up = 1;
          barCount = 0;
        }
      }

      randNr = random( barCount, MAXBARS+1 );
      drawBar( topBar, barCount, MAXBARS, barOrigin, barLine );
      drawBar( bottomBar, randNr , MAXBARS, barOrigin, barLine );
      
    } // if time to count
}

