/* @file HelloKeypad.pde

   Revised to use I2C i/o Feb 26/12 - G. D. Young

|| @version 1.0
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Demonstrates the simplest use of the matrix Keypad library.
|| #
*/
#include <Keypad_I2C.h>
#include <Wire.h>
#define I2CADDR 0x21

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'C','0','.','='}
};
// Digitran keypad, bit numbers of PCF8574 i/o port
byte rowPins[ROWS] = {0, 1, 2, 3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 6, 7}; //connect to the column pinouts of the keypad

Keypad_I2C kpd( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR );

void setup(){
  Wire.begin( );
  kpd.begin( makeKeymap(keys) );
  Serial.begin(9600);
  Serial.println( "start" );
}
void loop(){
  
  char key = kpd.getKey();
  
  if (key){
    Serial.println(key);
  }
}
