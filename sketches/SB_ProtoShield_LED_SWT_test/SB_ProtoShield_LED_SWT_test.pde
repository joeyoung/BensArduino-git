#include <Bounce.h>

/*
  SB-Protoshield basic function test
  
  Based on work by DojoDave & Tom Igoe
  http://www.arduino.cc/en/Tutorial/Button
   created 2009
   modified October 8 2009
   by Dave Hrynkiw, Solarbotics Ltd / HVW Technologies
   http://www.solarbotics.com

**Note: This sketch uses the "Bounce" library which has to be downloaded and put in your
  library. Full details here:
  http://www.arduino.cc/playground/Code/Bounce

The Circuit:
  * Connect Digital pins 0-3 to the LED header 1-4 (which illuminate on HIGH signal)
  * Connect Digital pins 8-11 to Switch header 1-4
    (which pass a LOW signal when pressed)
*/

// constants won't change. They're used here to 
// set pin numbers:
const int Led1 = 4;      // the number of the LED pin
const int Led2 = 5;      // the number of the LED pin
const int Led3 = 6;      // the number of the LED pin
const int Led4 = 7;      // the number of the LED pin

const byte Button1 = 8;       // the number of the pushbutton pin
const byte Button2 = 9;       // the number of the pushbutton pin
const byte Button3 = 10;      // the number of the pushbutton pin
const byte Button4 = 11;      // the number of the pushbutton pin



// Variables that will change:
int Button1State = 0;         // variable for reading the pushbutton status
int Button2State = 0;         // variable for reading the pushbutton status
int Button3State = 0;         // variable for reading the pushbutton status
int Button4State = 0;         // variable for reading the pushbutton status

int Button1Store = 1;         // variable for storing the pushbutton status
int Button2Store = 1;         // variable for storing the pushbutton status
int Button3Store = 1;         // variable for storing the pushbutton status
int Button4Store = 1;         // variable for storing the pushbutton status

// Instantiate a Bounce object with a 5 millisecond debounce time
Bounce Bounce1 = Bounce( Button1,5 ); 
Bounce Bounce2 = Bounce( Button2,5 ); 
Bounce Bounce3 = Bounce( Button3,5 ); 
Bounce Bounce4 = Bounce( Button4,5 ); 

void setup() {
  // initialize the LED pins as outputs:
  pinMode(Led1, OUTPUT);      
  pinMode(Led2, OUTPUT);      
  pinMode(Led3, OUTPUT);      
  pinMode(Led4, OUTPUT);      
  
  // initialize the pushbutton pin as inputs:
  pinMode(Button1, INPUT);
  digitalWrite(Button1, HIGH); //turn on Pullup Resistor
  pinMode(Button2, INPUT);     
  digitalWrite(Button2, HIGH); //turn on Pullup Resistor
  pinMode(Button3, INPUT);     
  digitalWrite(Button3, HIGH); //turn on Pullup Resistor
  pinMode(Button4, INPUT);     
  digitalWrite(Button4, HIGH); //turn on Pullup Resistor
  
  // turn on the LEDs to start with, so you know it's ready to play!
  digitalWrite(Led1, Button1Store);
  digitalWrite(Led2, Button1Store);
  digitalWrite(Led3, Button1Store);
  digitalWrite(Led4, Button1Store);
}

void loop(){
   // Update the debouncer
  Bounce1.update ( );
  Bounce2.update ( );
  Bounce3.update ( );
  Bounce4.update ( );

  
  // check each of the pushbuttons for a pressed state.
  // if it is, the buttonXState is HIGH:
  // read the state of the pushbutton value:
  int Button1State = Bounce1.read();     //read the button via the debouncer
  if (Button1State != HIGH) {            //if the button was being activated, to something!
    Button1Store = !Button1Store;        // invert stored signal value (if 1, make it 0; if 0, make it 1)
    digitalWrite(Led1, Button1Store);    // turn LED to new LED value    
  } 
  int Button2State = Bounce2.read();     //read the button via the debouncer
  if (Button2State != HIGH) {            //if the button was being activated, to something!     
    Button2Store = !Button2Store;        // invert stored signal value (if 1, make it 0; if 0, make it 1)
    digitalWrite(Led2, Button2Store);    // turn LED to new LED value    
  } 
  int Button3State = Bounce3.read();     //read the button via the debouncer
  if (Button3State != HIGH) {            //if the button was being activated, to something!     
    Button3Store = !Button3Store;        // invert stored signal value (if 1, make it 0; if 0, make it 1)
    digitalWrite(Led3, Button3Store);    // turn LED to new LED value    
  } 
  int Button4State = Bounce4.read();     //read the button via the debouncer
  if (Button4State != HIGH) {            //if the button was being activated, to something!     
    Button4Store = !Button4Store;        // invert stored signal value (if 1, make it 0; if 0, make it 1)
    digitalWrite(Led4, Button4Store);    // turn LED to new LED value    
  } 
  delay(100);                            //Pause the loop 100mS, otherwise a continuous push makes it seem
                                         //like it's still bouncing (when it's really toggling really fast)
 
}


