/* calculatorvxx
 *
 * Created: Oct 24, 2011 - G. D. Young
 *                       - started with HelloKeypad as below
 *
 * Revised: 
 *          Oct 25/11 - main calculator functions + - * working for ints
 *          Oct 27/11 - I2C keyboard calculator02
 *          Oct 28/11 - strtol conversion, long arg1, arg2,
 *                      multiply tone freq for external divider,
 *                      playtune( )
 *          Oct 29/11 - leading neg sign, atol() size reduction,
 *                      replay on leading =
 *          Nov 2/11  - lcd2 for home-made I2C interface to lcd,
 *                      Tune mode in playtone( )
 *          Nov 13/11 - 2nd keyboard
 *          Nov 14/11 - investigating kpd constructor problem: no progress
 *          Dec  2/11 - keypad library (kludge) working - see src .cpp file
 *          Dec  6/11 - cleanups, ver 0.4, tone array, key array, divide
 *          Jan 22/12 - attempt to change key layout (ben)
 *
|| @file HelloKeypad.pde
|| @version 1.0
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Demonstrates the simplest use of the matrix Keypad library.
|| #
*/
#include <i2ckeypad.h>

#define ROWS 4
#define COLS 4
#define ROWS2 4
#define COLS2 3
#ifndef NO_KEY
#define NO_KEY '\0'    //I2C keypad lib doesn't define NO_KEY
#endif

// With A0, A1 and A2 of PCF8574 to ground I2C address is 0x20
// Use PCF8574A, base adr is 0x38
//#define PCF8574_ADDR 0x39  // using port expander shield 2nd port
#define PCF8574_ADDR 0x21  // using custom 8574
#define PCF8574_ADDR2 0x22

char keymap0[4][5] =
{
  "123+",
  "456-",
  "789*",
  "c0.="
};
char keymap1[4][4] =
{
  "AD/",
  "BE%",
  "CF(",
  "c0)"
};

#define NUMKEYS 26
char allkeys[ ] = { 'c', '0', '1', '2', '3', '4', '5', '6', '7', '8',
                    '9', 'A', 'B', 'C', 'D', 'E', 'F', '+', '-', '*',
                    '.', '=', '/', '%', '(', ')' };

// int keyfreq[ ] = { 110, 220, 233, 247, 262, 277, 294, 311, 330, 349,
//                     370, 392, 415, 440, 466, 494, 523, 554, 587, 622,
//                     659, 698, 740, 784, 831, 880
// }; // key freq array

// Use musical names for frequencies -- Ben Young
#include "notenames.h"
int keyfreq[ ] = {
   N_AA, N_B,   N_C,   N_D,   N_E,   N_F,   N_G,   N_a,  N_b,  N_c,   
   N_d,  N_Csh, N_Fsh, N_ash, N_Dsh, N_Gsh, N_csh, N_e,  N_f,  N_g,
   N_A,  N_aa,  N_dsh, N_fsh, N_gsh, N_Ash  
};



// i2ckeypad library .cpp is modified for our keymap, pins.
// It would be cleaner if i2ckeypad's constructor worked like keypad
// where the pins and map are passed in as arguments to constructor
// Dec 2/11 - this parameterization of constructor is now done.


i2ckeypad kpd2( &keymap1[0][0], PCF8574_ADDR2, ROWS2, COLS2 );
i2ckeypad kpd( &keymap0[0][0], PCF8574_ADDR, ROWS, COLS );

// PROBLEM: only the second kpdx constructor results in a working
//          keypad. A following constructor appears to be the only
//          one that works--ie, swapping the above 2 lines with NO
//          other changes swaps the keyboard that responds. No errors
//          generated on using the one that doesn't work.
//          Note that lcd constructors work properly, giving 2
//          instances that both work.
// FIXED: Dec 2/11 - see i2ckeypad.cpp

// following is setup for using the keypad library
//const byte ROWS = 4; //four rows
//const byte COLS = 4; //four columns
// Oct 24/11 map for digitran telephone keypad - GDY
// Oct 25/11 digitran calculator 16-key
//char keys[ROWS][COLS] = {
//  {'=','.','0','C'},
//  {'*','9','8','7'},
//  {'-','6','5','4'},
//  {'+','3','2','1'}
//};
//byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad

byte lcdNumCols = 16;     // -- number of columns in the LCD
byte lcdNumRows = 2;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd2( 0x20, 16, 2 ); //instance at I2C port 20, 16 char, 2 rows
//LiquidCrystal_I2C lcd( 0x38, 16, 2 ); //instance at I2C port 38, 16 char, 2 rows

//Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


long arg1, arg2;  // calculator variables
byte op;
char entry[16];
byte eptr;
byte endop;
char result[32];

const byte tonepin = 12;
const byte tunepin = 2;
byte tunemode = LOW;

void do_op( byte op, long oarg1, long oarg2 ) {

  long rem;
  
  switch( op ) {
  case '+':
    oarg1 += oarg2;
    break;
  case '-':
    oarg1 -= oarg2;
    break;
  case '*':
    oarg1 *= oarg2;
    break;
  case '/':
    rem = oarg1 % oarg2;
    oarg1 /= oarg2;
    break;
  case '%':
    oarg1 %= oarg2;
    break;
  case 0:
    oarg1 = oarg2;
    break;
  default:
    oarg1 = oarg2;
  } // switch on op

//  lcd.setCursor( 0, 1 );
  lcd2.setCursor( 0, 1 );
  if( op == '/' ) {
    sprintf( result, "%ld R%ld", oarg1, rem );
  } else {
    sprintf( result, "%ld", oarg1 );
  } // if divide, include remainder
//  lcd.print( result );
  lcd2.print( result );
  endop = true;

} // do_op

void playtune( char digits[ ] ) {
  byte ix = 0;
  while( digits[ix] ) {
    delay( 250 );
    playtone( digits[ix] );
    ix++;
  } // while digits to sound
} // playtune

int lookup( char key ) {
  byte ikey = 0;
  while( ikey < NUMKEYS ) {
    if( key == allkeys[ikey] ) break;
    ikey++;
  } // while keys to check
  return ikey;
} // lookup

void playtone( byte key ) {
  
  int freq = 110;
  int mult = 8;
  char keyt = NO_KEY;
  
  freq = mult * keyfreq[ lookup( key ) ];

  tunemode = digitalRead( tunepin );  // check for tuning
  if( tunemode == LOW ) {
    lcd2.clear( );
    lcd2.print( "tune " );
    lcd2.print( key );

    while( tunemode == LOW ) {
      lcd2.setCursor( 0, 1 );
      lcd2.print( freq, 10 );
      keyt = kpd.get_key( 0 );
      if( keyt == NO_KEY ) keyt = kpd2.get_key( 1 );
      tone( tonepin, freq );
      while( keyt == NO_KEY && tunemode == LOW ) {
        keyt = kpd.get_key( 0 );
        if( keyt == NO_KEY ) keyt = kpd2.get_key( 1 );
        tunemode = digitalRead( tunepin );
      } // wait for key 
      if( tunemode == LOW ) {
         if( keyt == '+' ) {
           freq++;
         } else {
           freq--;
         } // if plus
      } // if still tunemode
    } // while tune mode
    lcd2.clear( );
    noTone( tonepin );

  } else {
    
    tone( tonepin, freq, 200 );
    
  } // if tuning
  
} // playtone


   
void setup(){
//  lcd.init();
//  lcd.begin( lcdNumCols, lcdNumRows );
//  lcd.clear( );
//  lcd.setCursor( 0, 0 );
//  lcd.print( "Calculator v0.4r" );
//  lcd.setCursor( 0, 1 );
//  lcd.setBacklight( HIGH );
  pinMode( tonepin, OUTPUT );
  pinMode( tunepin, INPUT );
  digitalWrite( tunepin, HIGH ); //turn on pullup resistors
  lcd2.init();
  lcd2.begin( lcdNumCols, lcdNumRows );
  lcd2.clear( );
  lcd2.setCursor( 0, 0 );
  lcd2.print( "Calculator v0.4x" );
  lcd2.setCursor( 0, 1 );
  lcd2.setBacklight( HIGH );
  kpd2.init( 1 );
  kpd.init( 0 );
  eptr = 0;
  endop = true;
}

void loop(){
  char key = kpd.get_key( 0 );
  if( key == NO_KEY ) key = kpd2.get_key( 1 );
  
  int ix = 0;  
  if( key != NO_KEY ){
    if( endop ) {
//      lcd.clear( );
      lcd2.clear( );
      eptr = 0;
      op = 0;
      endop = false;
    } // if new calculation is starting
//    lcd.print( key );
    lcd2.print( key );
    playtone( key );
   // check for special keys
    if( key == 'c' ) {
//      lcd.clear( );
//      lcd.setCursor( 0, 0 );
      lcd2.clear( );
      lcd2.setCursor( 0, 0 );
      eptr = 0;
      op = 0;
    } // if clear key
    else if( key == '+' || key == '-' || key == '*' 
                        || key == '/' || key == '%' ) {
      if( key == '-' && eptr == 0 ) {
        entry[eptr] = key;     // it's a leading negative sign
        eptr++;
      } else {
        op = key;              // it's an operator
        entry[eptr] = 0;
        arg1 = atol( entry );
        eptr = 0;
      } // if leading minus
    } // if key is operator
    else if( key == '=' ) {
      if( eptr == 0 ) {
//        lcd.setCursor( 0, 1 );
//        lcd.print( result );
        lcd2.setCursor( 0, 1 );
        lcd2.print( result );
        playtune( result );  // replay last result
//        lcd.setCursor( 0, 0 );
        lcd2.setCursor( 0, 0 );
        endop = true;
      } else {
        entry[eptr] = 0;
        arg2 = atol( entry );  // using atol() saves 500 bytes cf strtol()
        eptr = 0;
        do_op( op, arg1, arg2 );
        playtune( result );
      } // if leading = 
    } // if key is equal sign

    else {
   // if not special, save as entry
    entry[eptr] = key;
    eptr++;
    } // end of else if chain examining keys
  } // if any key to examine
} // loop

