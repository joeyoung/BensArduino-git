// clock calendar display
//
// created: Dec 15, 2011 G. D. Young
//
// revised: Dec 22/11 - use PCF8563 chip
//          Dec 23/11 - add year, month setting, display
//          Dec 27,28/11 - add alarm, freq out control
//          Dec 29/11 - add sound o/p
//          Aug 23/12 - New Keypad libraries, Keypad entry to setting,
//                      arduino 1.0, version 0.6
//
// Stand-alone version to develop display formatting. Will eventually
// use a real-time clock/calendar IC to retain values over power down.
//  - chip added Dec 22, counters in the IC are used to SET the 
//    'stand-alone' counters, IC is only accessed from setup()
//  - the sound routines are placed in separate 'tabs' which are
//    brought in with #include "clocksubr.h". This is the mechanism
//    arduino uses to implement modules in separate files.

#include <ctype.h>

#include <Keypad.h>
#include <Keypad_I2C.h>
#include <Wire.h>

#define LEFTKPD_ADR 0x21
#define ROWS 4
#define COLS 4

char keymap0[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'c','0','.','='}
};

byte rowPins0[ROWS] = {0, 1, 2, 3}; //connect to the row pin bit# of the keypad
byte colPins0[COLS] = {4, 5, 6, 7}; //connect to the column pin bit #

Keypad_I2C kpd( makeKeymap(keymap0), rowPins0, colPins0, ROWS, COLS, LEFTKPD_ADR );


#define LCDCOLS 16
#define LCDROWS 2
#define LCD_ADR 0x20

#define RTC_ADR 0x51      //7-bit adr - datasheet A2 write, A3 read
#define RTC_ST_RD_ERR 1   //clock access error codes - status read
#define RTC_TM_RD_ERR 2   // ..time read


#include "clocksubr.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd2( LCD_ADR, LCDCOLS, LCDROWS );


const char days[ 7 ][ 4 ] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
const char months[ 12 ][ 4 ] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

// clock variables
int seconds;
int minutes;
int hours;
int dow;
int dom;
int month;
int year;
int century;
char lvsignal;

long msecounter;
char datestr[32];

char str[32];
byte state[2];
byte rawtime[16];

// setting variables
//const byte setpin = 2;
//const byte alarmsetpin = 3;
const byte alarmoutpin = 11;
byte curpos = 0;
byte setmode = HIGH;
byte alarmsetmode = HIGH;
byte setting = false;
byte alarmsetting = false;
byte setkey;
char inputstr[32];    // a few extra chars only protection of overrun

byte flags;
byte errcode;
byte first;           // flag to control top line printing

byte getRtcStatus( byte *st ) {
  Wire.beginTransmission( RTC_ADR ); // start write to slave
  Wire.write( (uint8_t) 0x00 );    // set adr pointer to status 1
  Wire.endTransmission();
  errcode = Wire.requestFrom( RTC_ADR, 2 ); // request control_status_1 and 2
  if( errcode == 2 ) {
    st[0] = Wire.read( );
    st[1] = Wire.read( );
    return 0;
  } else {
    return RTC_ST_RD_ERR;
  } // if RTC responded with 2 bytes
} // getRtcStatus

byte clrRtcStatus( uint8_t fl ) {
  state[1] = state[1] & (~fl);    // reflect new state immediately
  Wire.beginTransmission( RTC_ADR ); // start write to slave
  Wire.write( (uint8_t) 0x01 );    // set adr pointer to status 2
  Wire.write( (uint8_t) state[1] );
  Wire.endTransmission();
  return 0;
} // clrRtcStatus

byte getRtcTime( byte *rt ) {
  Wire.beginTransmission( RTC_ADR ); // start write to slave
  Wire.write( (uint8_t) 0x02 );    // set adr pointer to VL_seconds
  Wire.endTransmission();
  errcode = Wire.requestFrom( RTC_ADR, 11 ); // request control_status_1 and 2
  if( errcode == 11 ) {
    for( byte ix=0; ix<11; ix++ ) {
      *(rt+ix) = Wire.read( );
    } // for received bytes
    return 0;
  } else {
    return RTC_TM_RD_ERR;
  } // if RTC responded with 7 bytes
} // getRtcTime

void setRtcClkOut( byte ctrl ) {
  Wire.beginTransmission( RTC_ADR );
  Wire.write( (uint8_t) 0x0d );  // adr pointer to contro reg
  Wire.write( (uint8_t) ctrl );
  Wire.endTransmission( );
} // setRtcClkOut

void setup(){
//  Wire.begin( );
  kpd.begin( );
//  Serial.begin( 9600 );         //debug
  lcd2.init();
  lcd2.begin( LCDCOLS, LCDROWS );
  lcd2.clear( );
  lcd2.setCursor( 0, 0 );
  lcd2.print( "Clk 0.6 " );
  lcd2.setCursor( 0, 1 );
  lcd2.setBacklight( HIGH );
//  kpd.init( 0 );
  errcode = getRtcStatus( state );
  if( errcode ) {
    lcd2.print( "clk st err " );
    lcd2.print( (int)errcode );
  } // if read error
  errcode = getRtcTime( rawtime );
  if( errcode ) {
    lcd2.print( "clk rd err " );
    lcd2.print( (int)errcode );
  } // if read error
  if( errcode == 0 ) {
    lvsignal = ' ';
    if( (rawtime[0]&0x80) == 0x80 ) lvsignal = '?';
    seconds = rawtime[0] & 0x0f;
    seconds += ( (rawtime[0] & 0x70)>>4 )*10;
    minutes = rawtime[1] & 0x0f;
    minutes += ( (rawtime[1] & 0x70)>>4 )*10;
    hours = rawtime[2] & 0x0f;
    hours += ( (rawtime[2] & 0x30)>>4 )*10;
    dom = rawtime[3] & 0x0f;
    dom +=( (rawtime[3]&0x30)>>4 )*10;
    dow = rawtime[4]&0x07;
    month = ( (rawtime[5]&0x10)>>4 )*10;
    month += rawtime[5]&0x0f;
    year = ( (rawtime[6]&0xf0)>>4 )*10;
    year += rawtime[6]&0x0f;
    century = 19;                 // 4-digit year kludge.
    if( year < 80 ) century = 20; 
  } else {
    seconds = 1;     // no clock present default distinctive pattern
    minutes = 2;
    hours = 3;
    dow = 4;
    dom = 5;
    month = 6;
    year = 7;
    century = 20;
  } // if good time available
//  setRtcClkOut( (byte)0x83 );  // set clk out to 1 Hz
  setRtcClkOut( (byte)0 );     // set clk out OFF
  msecounter = millis( );
//  pinMode( setpin, INPUT );
//  pinMode( alarmsetpin, INPUT );
//  digitalWrite( setpin, HIGH );
//  digitalWrite( alarmsetpin, HIGH );  // turn on pullups
  pinMode( alarmoutpin, OUTPUT );
  first = true;

  kpd.addEventListener( startSetMode );
  kpd.setHoldTime( 600 );

// debug
//  sprintf( str, "%.2x %.2x %.2x ", state[1], rawtime[7], rawtime[8] );
//  Serial.print( str );

}

void loop( ) {

  if( millis( ) >= msecounter+1000 ) {
    msecounter += 1000;
    seconds += 1;
    if( seconds > 59 ) {
      seconds = 0;
      minutes += 1;
      digitalWrite( alarmoutpin, LOW );  //uncomment to clear in 1min
      getRtcStatus( state );    // check for alarm
      if( ( state[1] & 0x08 ) == 0x08 ) {
        digitalWrite( alarmoutpin, HIGH );
        playtune( minutes&3 );
        clrRtcStatus( 0x08 );
      } // if alarm flag
    } // if minute
    if( minutes > 59 ) {
      minutes = 0;
      hours += 1;
    } // if hour
    if( hours > 23 ) {
      hours = 0;
      dow += 1;
    } // if day
    if( dow > 6 ) {
      dow = 0;
    } // if week

    if( first ) {
      sprintf( datestr, "Clk 0.6 %.3s %.2d%.2d",
                   months[month-1], century, year );
      lcd2.noBlink( );
      lcd2.setCursor( 0, 0 );
      lcd2.print( datestr );
      first = false;
    } // put year, month if first pass
    sprintf( datestr, "%.2d:%.2d:%.2d%c %s %.2d", 
             hours, minutes, seconds, lvsignal, days[dow], dom );
    lcd2.noBlink( );
    lcd2.setCursor( 0, 1 );
    lcd2.print( datestr );

  } // if second has elapsed
  
//  setmode = digitalRead( setpin );
// use keypad HOLD state to enter setmode
  setkey = kpd.getKey( );
  if( setmode == LOW ) {
    sprintf( datestr, "%.2d.%.2d.%.2d %.1d %.2d %.2d", 
                   hours, minutes, seconds, dow, dom, month );
    kbdBeep( NOTE_D4, 100 );
    delay( 150 );
    kbdBeep( NOTE_A4, 100 );
    curpos = 0;
  } // if setmode
  while( setmode == LOW ) {
    lcd2.setCursor( 0, 1 );
    lcd2.print( datestr );
    if( curpos < 17 ) {
      lcd2.setCursor( curpos, 1 );
    } else {
      lcd2.setCursor( curpos-3, 0 );
    } // if setting bottom line
    lcd2.blink( );
    setkey = kpd.getKey( );
    while( setkey == NO_KEY && setmode == LOW ) {
//      setmode = digitalRead( setpin );
      setkey = kpd.getKey( );
      if( setkey == '+' ) setmode = HIGH;  // exit when set key again
    } // wait for key
    setting = true;
    if( setmode == LOW ) {
      if( isdigit( setkey ) || setkey == '.' ) {
        kbdBeep( NOTE_A5, 100 );
        inputstr[curpos] = setkey;
        datestr[curpos] = setkey;
        curpos += 1;
      } else {
        kbdBeep( NOTE_A3, 150 );
      } // if entry valid digit or period
    } // only update if still in setmode
  } // while setmode

// alarmsetmode = digitalRead( alarmsetpin );
// use keypad HOLD state to enter setmode
  setkey = kpd.getKey( );
  if( alarmsetmode == LOW ) {
    lcd2.setCursor( 0, 0 );
    lcd2.print( "ALM Set" );
    kbdBeep( NOTE_D4, 100 );
    delay( 150 );
    kbdBeep( NOTE_A4, 100 );
    curpos = 0;
    sprintf( datestr, "mm.hh dm d      " );
  } // if alarmsetmode
  while( alarmsetmode == LOW ) {
    digitalWrite( alarmoutpin, HIGH );
    lcd2.setCursor( 0, 1 );
    lcd2.print( datestr );
    lcd2.setCursor( curpos, 1 );
    lcd2.blink( );
    setkey = kpd.getKey( );
    while( setkey == NO_KEY && alarmsetmode == LOW ) {
//      alarmsetmode = digitalRead( alarmsetpin );
      setkey = kpd.getKey( );
      if( setkey == '-' ) alarmsetmode = HIGH; // exit alrmsetmode on C key
    } // wait for key
    digitalWrite( alarmoutpin, LOW );
    alarmsetting = true;
    if( alarmsetmode == LOW ) {
      if( isdigit( setkey ) || setkey == '.' ) {
        kbdBeep( NOTE_A5, 100 );
        inputstr[curpos] = setkey;
        datestr[curpos] = setkey;
        curpos += 1;
      } else {
        kbdBeep( NOTE_A3, 150 );
      } // if entry valid
    } // if still setting
  } // while alarmsetmode
  
  if( setting && ( curpos == 19 || curpos == 8 ) ) { 
    //only set if exact number entries: 8 is time only
    getRtcTime( rawtime );     // get current info in case only time
    inputstr[2] = '\0';
    inputstr[5] = '\0';
    inputstr[8] = '\0';        // separate into individual strings
    inputstr[10] = '\0';
    inputstr[13] = '\0';
    inputstr[16] = '\0';
    inputstr[19] = '\0';
    hours = atoi( &inputstr[0] );
    minutes = atoi( &inputstr[3] );
    seconds = atoi( &inputstr[6] );
    if( curpos > 8 ) {
      dow = atoi( &inputstr[9] );
      dom = atoi( &inputstr[11] );
      month = atoi( &inputstr[14] );
      year = atoi( &inputstr[17] );
      century = 19;
      if( year < 80 ) century = 20;
    } // if setting whole shebang
  // also pack the inputstr chars into the clock bcd format rawtime
    rawtime[2] = (inputstr[0]&0x03)<<4; // hours
    rawtime[2] |= inputstr[1]&0x0f;
    rawtime[1] = (inputstr[3]&0x0f)<<4;  // minutes
    rawtime[1] |= inputstr[4]&0x0f;
    rawtime[0] = (inputstr[6]&0x07)<<4;  // seconds
    rawtime[0] |= inputstr[7]&0x0f;
    if( curpos > 8 ) {
      rawtime[4] = inputstr[9]&0x07;       // day of week
      rawtime[3] = (inputstr[11]&0x03)<<4; // day of month
      rawtime[3] |= inputstr[12]&0x0f;
      rawtime[5] = (inputstr[14]&0x01)<<4; // month
      rawtime[5] |= inputstr[15]&0x0f;
      rawtime[6] = (inputstr[17]&0x0f)<<4; //years
      rawtime[6] |= inputstr[18]&0x0f;
    } // if setting whole shebang
    Wire.beginTransmission( RTC_ADR ); // setup to write to clock
    Wire.write( 0x00 );            // adress pointer <- 0
    Wire.write( 0x20 );            // ctrl 1, STOP bit
    Wire.write( 0x00 );            // ctrl 2, disable ints, alarms
    for( byte ix=0; ix<7; ix++ ) Wire.write( rawtime[ix] );
    Wire.endTransmission( );           // STOP, set all clock regs
    Wire.beginTransmission( RTC_ADR );
    Wire.write( 0x00 );
    Wire.write( 0x00 );
    Wire.endTransmission( );           // restart the counters
//    setRtcClkOut( (byte)0x83 );  // set clk out to 1 Hz
    setting = false;
    first = true;                 // arm for calendar display
    curpos = 0;
    msecounter = millis( );
    kbdBeep( NOTE_A5, 100 );      // happy exit
  } // if setting entry correct
    else if(setting) {
      kbdBeep( NOTE_A3, 150 );       // not good entry
      setting = false;
  } // else if setting entry was invalid
 
  
  if( alarmsetting ) {
    inputstr[2] = '\0';
    inputstr[5] = '\0';
    inputstr[8] = '\0';        // separate into individual strings
    inputstr[10] = '\0';
    rawtime[7] = 0x80;          // clear all alarm enables
    rawtime[8] = 0x80;
    rawtime[9] = 0x80;
    rawtime[10] = 0x80;
    flags = 0x08;
    if( curpos < 2 || curpos > 10 ) {
      // no setting
      lcd2.setCursor( 0, 0 );
      lcd2.print( "NOT Set" );
      kbdBeep( NOTE_A3, 150 );       // not good entry
      alarmsetting = false;
   } else {
      kbdBeep( NOTE_A5, 100 );      // happy with entry
      if( curpos >= 2 ) {
        rawtime[7] = (inputstr[0]&0x07)<<4;   // minute alarm
        rawtime[7] |= inputstr[1]&0x0f;
      } // if minutes entry
      if( curpos >= 5 ) {
        rawtime[8] = (inputstr[3]&0x03)<<4;  // hour alarm
        rawtime[8] |= inputstr[4]&0x0f;
      } // if hour entry
      if( curpos >= 8 ) {
        rawtime[9] = (inputstr[6]&0x03)<<4;  // day of month alarm
        rawtime[9] |= inputstr[7]&0x0f;
        if( rawtime[9] == 0 ) rawtime[9] = 0x80; // 0 means no DOM
      } // if day of month entry
      if( curpos == 10 ) {
        rawtime[10] = inputstr[9]&0x07;       // day of week
      } // if day of week entry
    } // if reasonable number entries

    Wire.beginTransmission( RTC_ADR ); // setup to write to clock
    Wire.write( 0x09 );            // adress pointer <- 9 alarm regs
    for( byte ix=7; ix<11; ix++ ) Wire.write( rawtime[ix] );
    Wire.endTransmission( );      // set all alarm regs
    alarmsetting = false;
    curpos = 0;
//    first = true;                  // re-arm time display
  clrRtcStatus( flags );
//  sprintf( str, "%x %x %x ", state[1], rawtime[7], rawtime[8] );
//  Serial.print( str );

  } // if alarmsetting

  
} // loop

void startSetMode( KeypadEvent setkey ) {
  
  switch( kpd.getState( ) ){
  case PRESSED:
 //     if( setkey == '=' && calmode ) exitkey = true;
      break;
  case HOLD:
      if( setkey == '+' ) {
        setmode = LOW;
      } // if setting key held
      if( setkey == '-' ) {
        alarmsetmode = LOW;
      } // if alarm setting key held
//      if( setkey == '*' ) {
//        scalesetmode = LOW;
//      } // if set scale key held
//      if( setkey == '=' ) {
//        calmode = true;
//      } // if cal key held
      break;  
  case RELEASED:
      break;
  } // switch on keypad state
  
} // startSetMode( )
