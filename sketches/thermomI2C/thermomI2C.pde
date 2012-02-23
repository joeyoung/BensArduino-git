/* Thermometer
 *
 * created: Sept 24, 2011  G. D. Young
 *
 * revised: Oct 8/11 - longer average, variance calc'n
 *          Jan 6/12 - I2C display
 *
 * Uses analog output semiconductor temp sensor LM60 on analog
 * input pin 0
 *
 * Raw a/d reading is scaled to temperature using integer arithmetic
 * implementing the datasheet calibration equation:
 *
 *   T = ( Vin - 424 ) / 6.25
 *
 *   when Vin in mV, or the equation:
 *
 *   T = ( Vincnt - 212 ) / 3.125 [cnt/deg]
 *
 *   when the input voltage is expressed in A/D counts with
 *   Vref = 2.048 (giving 2 mV/count)
 *
 *   further details of calculation: 
 *     - 3.125 = 3 1/8 = 25/8, so divide by 3.125 can be replaced
 *       with multiply by 8/25
 *     - then ten samples are summed to get an average (reading*10)
 *     - finally numerator and denominator scaled by 100 to permit
 *       integer divide and (8/25)*100 = 32
 *
 */

//#include <LiquidCrystal.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>



//LiquidCrystal lcd( 12, 11, 5, 4, 3, 2 ); // create LCD instance
LiquidCrystal_I2C lcd2( 0x20, 16, 2 ); // inst at port 20, 16 col 2 rows

byte lcdNumCols = 16;   // -- number of columns in the LCD
long interval = 100;    // interval at which to inc bar (milliseconds)
long previousMillis;

int analogPin = 0;
int val = 0;
long valavg = 0;
long temp10 = 0;
int tempw, tempf;
int sample = 0;

int sample100 = 0;
long valavg100 = 0;    // 100 sample average, variance calc vars
long variance = 0;
long sumxx = 0;

int test_val;

void setup(){
  // -- initializing the LCD
//  lcd.begin(2, lcdNumCols);
//  lcd.clear();
  Wire.begin( );
  lcd2.init( );
  lcd2.begin( 16, 2 );
  lcd2.clear( );
  lcd2.setBacklight( HIGH );
  
  analogReference( EXTERNAL );
  
  previousMillis = millis();    // init interval timer for drawing
}

void loop()
{
  if (millis() - previousMillis > interval) {
      // save this matching time for the next comparison
      previousMillis = millis();   

    // get a/d sample
    val = analogRead( analogPin );
    valavg = valavg + (long)val;
//    test_val = 240;    // variance s/b 0
//    if( sample % 2 ) {    // points either side of mean
//      test_val = 245;     // 241, 239 variance 1.01
//    } else {
//      test_val = 235;
//    }
//    sumxx = sumxx + (long)( test_val )*(long)( test_val );
//    valavg100 = valavg100 + (long)test_val;
    sumxx = sumxx + (long)( val )*(long)( val );
    valavg100 = valavg100 + (long)val;
    sample += 1;
    
    // if average count up, calculate temperature
    if( sample > 9 ) {
      sample = 0;
      sample100 += 10;
      temp10 = (( valavg - 2120 ) * 32 )/100;
      tempw = temp10/10;
      tempf = temp10 - 10*tempw;
    
      // display result
//      lcd.setCursor( 0, 1 );
//      lcd.print( "Temp = " );
//      lcd.print( tempw, 10 );
//      lcd.print( '.' );
//      lcd.print( abs(tempf), 10 );
//      lcd.print( "   " );
//      lcd.setCursor( 13, 1 );
//      lcd.write( 0xDF );
//      lcd.print( 'C' );
      lcd2.setCursor( 0, 1 );
      lcd2.print( "Temp = " );
      lcd2.print( tempw, 10 );
      lcd2.print( '.' );
      lcd2.print( abs(tempf), 10 );
      lcd2.print( "   " );
      lcd2.setCursor( 13, 1 );
      lcd2.write( 0xDF );
      lcd2.print( 'C' );
      valavg = 0;
      
      if( sample100 > 90 ) {    //check for long interval up
        sample100 = 0;
//        lcd.setCursor( 0, 0 );
//        lcd.print( valavg100, 10 );
        lcd2.setCursor( 0, 0 );
        lcd2.print( valavg100, 10 );
        variance = ( 100*sumxx - valavg100 * valavg100 ) / 99;
        // s/b div by 9900 - use 99 to keep two decimal points
//        lcd.setCursor( 6, 0 );
//        lcd.print( "         " );
//        lcd.setCursor( 6, 0 );
//        lcd.print( variance, 10 );
        lcd2.setCursor( 6, 0 );
        lcd2.print( "         " );
        lcd2.setCursor( 6, 0 );
        lcd2.print( variance, 10 );
        sumxx = 0;
        valavg100 = 0;
      } // if long interval up
      
    } // if sample count

  } // if time to sample a/d

} // loop
