/* calculatorv0.9 - using Keypad_I2C with event listeners
 *
 * Created: Oct 24, 2011 - G. D. Young
 *                       - started with HelloKeypad as below
 *
 * Revised: Oct 25/11 - main calculator functions + - * working for ints
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
 *          Feb 29/12 - New Keypad_I2C library, Ben's tone definition
 *          Apr  6/12 - Order of calling kpd kpd2 functions matters??
 *          Apr  9/12 - Order prob fixed - see helloKeypadTwo_I2C.ino
 *          Apr 12/12 - Order prob actually fixed, more flags in listeners
 *          Aug 21/12 - For mini - use ) HOLD to enter tune mode, ) exit
 *          Nov 29/12 - allow chain operations
*/
#include <Keypad_I2C.h>
#include <Keypad.h>
#include <Wire.h>

#define ROWS 4
#define COLS 4
#define ROWS2 4
#define COLS2 3

// With A0, A1 and A2 of PCF8574 to ground I2C address is 0x20
#define ADDR1 0x21  // using custom 8574
#define ADDR2 0x22

char keymap1[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'c','0','.','='}
};
char keymap2[ROWS2][COLS2] = {
  {'A','D','/'},
  {'B','E','%'},
  {'C','F','('},
  {'c','0',')'}
};

#define NUMKEYS 26
char allkeys[ ] = { 'c', '0', '1', '2', '3', '4', '5', '6', '7', '8',
                    '9', 'A', 'B', 'C', 'D', 'E', 'F', '+', '-', '*',
                    '.', '=', '/', '%', '(', ')' };

// Use musical names for frequencies -- Ben Young
#include "notenames.h"
int keyfreq[ ] = {
   N_AA, N_B,   N_C,   N_D,   N_E,   N_F,   N_G,   N_a,  N_b,  N_c,
   N_d,  N_Csh, N_Fsh, N_ash, N_Dsh, N_Gsh, N_csh, N_e,  N_f,  N_g,
   N_A,  N_aa,  N_dsh, N_fsh, N_gsh, N_Ash
};

byte rowPins1[ROWS] = {0, 1, 2, 3}; //connect to the row pin bit# of the keypad
byte colPins1[COLS] = {4, 5, 6, 7}; //connect to the column pin bit #
byte rowPins2[ROWS2] = {0, 1, 2, 3}; // same for 2nd keypad
byte colPins2[COLS2] = {4, 5, 6};

byte lcdNumCols = 16;     // -- number of columns in the LCD
byte lcdNumRows = 2;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd2( 0x20, 16, 2 ); //instance at I2C port 20, 16 char, 2 rows
//LiquidCrystal_I2C lcd( 0x38, 16, 2 ); //instance at I2C port 38, 16 char, 2 rows

Keypad_I2C kpd = Keypad_I2C( makeKeymap(keymap1), rowPins1, colPins1, ROWS, COLS, ADDR1 );
Keypad_I2C kpd2 = Keypad_I2C( makeKeymap(keymap2), rowPins2, colPins2, ROWS2, COLS2, ADDR2 );

boolean kpdbusy, kpd2busy;  // keypad order fix

long arg1, arg2;  // calculator variables
byte op;
char entry[16];
byte eptr;
byte endop;
char result[32];

const byte tonepin = 12;
const byte tunepin = 2;
byte tunemode = HIGH;

long long_sqrt( long x ) {
  unsigned long old, newx;
  
  if( x == 0L ) {
    return( 0L );
  } else {
    newx = x;
    do {
      old = newx;
      newx = (newx + x/newx)>>1;
    } while( newx < old );
    return( old );
  } // if x == 0
  
} // long_sqrt

byte sgn( long x ) {
  if( x < 0 ) return false;
  else return true;
} // sgn

long imod( long x, long y ) {
  long tmp;
  if( y == 0 ) return x;
  tmp = x / y;
  if( sgn( x ) != sgn( y ) ) --tmp;
  x = x - tmp * y;
  if( x == y ) x -= y;
  return x;
} // integer x mod y

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
    oarg1 = imod( oarg1, oarg2 );
    break;
  case '(':                     // integer sqrt
    rem = oarg1;
    oarg1 = long_sqrt( oarg1 );
    rem = rem - oarg1*oarg1;
    break;
  case 0:
    oarg1 = oarg2;
    break;
  default:
    oarg1 = oarg2;
  } // switch on op

//  lcd.setCursor( 0, 1 );
  lcd2.setCursor( 0, 1 );
  lcd2.print( "                " ); //prepare blank line for result
  lcd2.setCursor( 0, 1 );           // 2012-11-29
  if( op == '/' ) {
    sprintf( result, "%ld R%ld", oarg1, rem );
  } else if( op == '(' ) {
    sprintf( result, "%ld D%ld", oarg1, rem );
  } else {
    sprintf( result, "%ld", oarg1 );
  } // if divide or sqrt, include remainder
//  lcd.print( result );
  lcd2.print( result );
//  endop = true;          // 12-11-29
  arg1 = oarg1;

} // do_op

void playtune( char digits[ ] ) {
  byte ix = 0;
  while( digits[ix] ) {
    playtone( digits[ix] );
    delay( 200 );
    noTone( tonepin );
    delay( 50 );
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


const char tunekey = ')';      // 12-8-21

void playtone( byte key ) {
  
  int freq = 110;
  int mult = 8;
  char keyt = NO_KEY;
  
  freq = mult * keyfreq[ lookup( key ) ];

//  tunemode = digitalRead( tunepin );  // check for tuning
  if( tunemode == LOW ) {
    lcd2.clear( );
    lcd2.print( "tune " );
    lcd2.print( (char)key );

    while( tunemode == LOW ) {
      lcd2.setCursor( 0, 1 );
      lcd2.print( freq, 10 );
      tone( tonepin, freq );
      keyt = kpd.getKey( );   // instead of checking switch 2012-8-21
      while( keyt == NO_KEY && tunemode == LOW ) {
        keyt = kpd.getKey( );
        if( keyt == NO_KEY ) keyt = kpd2.getKey( );
        if( keyt == tunekey ) tunemode = HIGH; // exit tunemode on next tunekey
      } // wait for key
      if( tunemode == LOW ) {
         if( keyt == '+' ) {
           freq++;
         } else if( keyt == '-' ) {
           freq--;
         } // if plus or minus - ignore others
      } // if still tunemode
    } // while tune mode
    lcd2.clear( );
    noTone( tonepin );
    keyfreq[ lookup( key ) ] = freq / mult;  // save adjustment 2012-8-21
          // this is a bit crude--rounding error, doesn't survive reset
  } else {
    
    tone( tonepin, freq );
    
  } // if tuning
  
} // playtone

char key, key1, key2;
KeyState kst, kst2;
boolean release1, release2;
byte entry_cursor;           // 2012-11-29
boolean newCalc;
   
void setup(){
//  Wire.begin( );
  kpd.begin( );
  kpd2.begin( );
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
  lcd2.print( "Calculator v0.9" );
  lcd2.setCursor( 0, 1 );
  lcd2.setBacklight( HIGH );
  kpd2.addEventListener( kpd2lock );
  kpd2.setHoldTime( 2000 );         // 2012-8-21
  kpd.addEventListener( kpdlock );
  kpdbusy = false;
  kpd2busy = false;
  kst = RELEASED;
  kst2 = RELEASED;
  release1 = true;
  release2 = true;
  eptr = 0;
  endop = true;
  arg1 = 0;          // 2012-11-29
  entry_cursor = 0;
  newCalc = true;
}


void loop(){
  if( kpdbusy == false ) key2 = kpd2.getKey( );
  if( kpd2busy == false ) key1 = kpd.getKey( );

  int ix = 0;  
  if( key != NO_KEY ){
    if( endop ) {
//      lcd.clear( );
      lcd2.clear( );
      lcd2.setCursor( 0, 1 );
      lcd2.print( result );   // redisplay result
      lcd2.setCursor( 0, 0 );
      eptr = 0;
      op = 0;
      entry_cursor = 0;
      endop = false;
    } // if new operation is starting
//    lcd.print( key );
    lcd2.print( key );
    if( key != '=' ) {
      playtone( key );
    } // don't sound = key because it's shortened by playtune

   // check for special keys
    if( key == 'c' ) {     // if clear key
      lcd2.clear( );
      eptr = 0;
      op = 0;
      arg1 = 0;
      strcpy( result, "0               " );
      lcd2.setCursor( 0, 1 );
      lcd2.print( result );
      lcd2.setCursor( 0, 0 );
      entry_cursor = 0;
      endop = true;
      newCalc = true;

    } else if( key == '+' || key == '-' || key == '*' 
                        || key == '/' || key == '%' 
                        || key == '(' ) {             // if key is operator
      if( ( key == '-' || key == '+' ) && ( op != 0 ) && eptr == 0 ) {
        entry[eptr] = key;
        eptr++;
        entry[eptr] = 0;
      } else { 
        if( newCalc && eptr == 0 ) {
          op = key;
          entry_cursor = 1;
          entry[eptr] = 0;   // make null string
          newCalc = false;
        } // if first entry is an operator (it will op on 0)
        if( newCalc && eptr != 0 ) {
          op = key;              // save new operator
          arg1 = atol( entry );  // put entry into first op position
          entry_cursor = eptr + 1;
          eptr = 0;
          entry[eptr] = 0;
          newCalc = false;
        } // if new
        if( newCalc == false && eptr == 0 ) {
          op = key;
          if( entry_cursor==0) entry_cursor++;
          lcd2.setCursor( entry_cursor, 0 );
        } // if continuing entry
        if( newCalc == false && eptr != 0 ) {
          arg2 = atol( entry );   // 2012-11-29
          entry_cursor = entry_cursor + eptr + 1;  // "  remember entry position
          eptr = 0;
          do_op( op, arg1, arg2 );
          op = key;                // remember this chain op for next arg
          lcd2.setCursor( entry_cursor, 0 );
        } // if not leading op
      } // if leading sign

    } else if( key == '=' ) {
      if( eptr == 0 && op != '(' ) {
//        lcd.setCursor( 0, 1 );
//        lcd.print( result );
        lcd2.setCursor( 0, 1 );
        lcd2.print( result );
        playtune( result );  // replay last result
//        lcd.setCursor( 0, 0 );
        lcd2.setCursor( 0, 0 );
        endop = true;
      } else if( eptr == 0 && op == '(' ) {  //special sqrt handling
        do_op( op, arg1, arg2 );
        playtune( result );
        endop = true;
      } else {
        arg2 = atol( entry );  // using atol() saves 500 bytes cf strtol()
        eptr = 0;
        do_op( op, arg1, arg2 );
        playtune( result );
        endop = true;
      } // if leading = 

    }  else {
   // if not special, save as entry
    entry[eptr] = key;
    eptr++;
    entry[eptr] = 0;   // keep input buffer valid string
    } // end of else if chain examining keys

    key = NO_KEY;

  } // if any key to examine

    if( release1 && release2 ) {
      noTone( tonepin );
    }

} // loop

void kpd2lock( KeypadEvent key2 ) {
  kst2 = kpd2.getState( );
  switch( kst2 ) {
    case PRESSED:
      kpd2busy = true;
      release2 = false;
      key = key2;
    break;
    case HOLD:
     if( key2 == tunekey ) tunemode = LOW;   // 2012-8-21
     lcd2.print( "-->tune. key?" );
     break;
    case RELEASED:
      kpd2busy = false;
      release2 = true;
    break;
  } // switch on right, kpd2 state
} // kpd2lock

void kpdlock( KeypadEvent key1 ) {
  kst = kpd.getState( );
  switch( kst ) {
    case PRESSED:
      kpdbusy = true;
      release1 = false;
      key = key1;
    break;
    case RELEASED:
      kpdbusy = false;
      release1 = true;
    break;
  } // switch on left kpd state
} // kpdlock


