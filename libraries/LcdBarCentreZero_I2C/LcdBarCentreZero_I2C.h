/* LcdBarCentreZero_I2C.h Lcd centre-zero bargraph library
 *
 * Created: Sept 19, 2011 - G. D. Young
 *
 * Revised: Sept 20/11 - add blank bar display area before drawing
 *          Sept 20/11 - create library version
 *          Oct 14/11 - I2C connected display version
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

#ifndef LcdBarCentreZero_I2C_h
#define LcdBarCentreZero_I2C_h

#include "WProgram.h"
#include <LcdBarCentreZero_I2C.h>
#include <LiquidCrystal_I2C.h>

class LcdBarCentreZero_I2C
{
public:
	//class constructor
	LcdBarCentreZero_I2C( LiquidCrystal_I2C* lic2 );

	//draw a zero-centre bargraph with centre at column centre on line line
	// and longest bar length = maxbars
	void drawBar( int value, byte maxbars, byte centre, byte line );

	//load character generator with bargraph symbols
	void loadCG( void );

private:
	LiquidCrystal_I2C* _lcdi2c;

	static byte _gchar0[8];
	static byte _gchar1[8];
	static byte _gchar2[8];
	static byte _gchar3[8];
	static byte _gchar4[8];
	static byte _gchar5[8];
	static byte _gchar6[8];
	static byte _gchar7[8];

}; // class LcdBarCentreZero_I2C

#endif
