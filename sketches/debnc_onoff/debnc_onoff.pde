/*
  Blink with on/off button
  
  Created: Aug 11/11 G. D. Young from combining:
                         Blink without delay 2005
                         Debounce Nov/06 by David Mellis 

  Revised: Aug 12/11 - add control state led on pin 12 for debugging
           Aug 13/11 - add guard around bouncing time
  
 Turns on an LED on for one second, then off for one second, repeatedly.
 
 Each time the input pin goes from LOW to HIGH (e.g. because of a push-button
 press), the output pin flashing is allowed or prevented.  There's
 a minimum delay between toggles to debounce the circuit (i.e. to ignore
 noise).  
 

 The circuit:
 * LED connected from digital pin 13 to ground.
 * pushbutton attached from pin 2 to +5V
 * 10K resistor attached from pin 2 to ground

 * Note: On most Arduino boards, there is already an LED on the board
 connected to pin 13, so you don't need any extra components for this example.
 
 
 http://arduino.cc/en/Tutorial/Blink
 http://www.arduino.cc/en/Tutorial/Debounce
 
 based on an orginal by H. Barragan for the Wiring i/o board
 
 */

// constants won't change. They're used here to 
// set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  13;      // LED connected to digital pin 13
const int statePin = 12;     // pin with ledState

// Variables will change:
int ledState;                // the current state of the output pin
int controlState;            // on/off control at button transitions
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int oldState;                // the way the button was before debounce
int startDebounce;

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;   // the last time the output pin was toggled
long debounceDelay = 100;    // the debounce time; increase if the output flickers
long previousMillis = 0;     // will store last time LED was updated
long interval = 1000;        // interval at which to blink (milliseconds)


// The setup() method runs once, when the sketch starts

void setup()   {                
  // initialize the digital pin as an output:
  pinMode(ledPin, OUTPUT);     
  pinMode(buttonPin, INPUT);    //button pin as input
  pinMode( statePin, OUTPUT);
  startDebounce = LOW;          // flag guarding bouncing time
}

// the loop() method runs over and over again,
// as long as the Arduino has power

void loop()                     
{

  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    if( startDebounce == LOW ) {     // first time in
      oldState = lastButtonState;    // remember the way it was
      startDebounce = HIGH;
    }
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonState = reading;
    startDebounce = LOW;            // bounce over, reset guard
  }
  
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;

  // change controlState at de-bounced rising edges of buttonState
  
  if( buttonState == HIGH && oldState == LOW ) {
    if( controlState == LOW ) {
      controlState = HIGH;
      digitalWrite( statePin, HIGH );
      ledState = LOW;
    } else {
      controlState = LOW;
      digitalWrite( statePin, LOW );
      digitalWrite( ledPin, LOW );
    }
    oldState = buttonState;
  }
 
   // surround the led blinking with the test of controlState
  if( controlState == HIGH ) {

    // check to see if it's time to blink the LED; that is, is the difference
    // between the current time and last time we blinked the LED bigger than
    // the interval at which we want to blink the LED.
    if (millis() - previousMillis > interval) {
      // save the last time you blinked the LED 
      previousMillis = millis();   

      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW)
        ledState = HIGH;
      else
        ledState = LOW;
      
      // set the LED with the ledState of the variable:
      digitalWrite(ledPin, ledState);
    } // if flash time up

  } // if controlState
 
} // loop

