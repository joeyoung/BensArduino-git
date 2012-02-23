/* Revised Dec 29/11 as a subr for clock05 - G. D. Young

 Melody
 
 Plays a melody 
 
 created 21 Jan 2010
 modified 30 Aug 2011
 by Tom Igoe 
 */

 #include "pitches.h"
 
 #define TONEPIN 12
 #define EXTMULT 8    // provide for external division by EXTMULT
 #define REST 2
 
// pre-calculate note durations in duration array
int wn = 1000;   // whole note duration

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
//    int noteDuration = 1000/noteDurations[thisNote];

int melody0[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3, REST, NOTE_B3, NOTE_C4,0};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations0[ ] = {
  wn/4, wn/8, wn/8, wn/4, wn/4, wn/4, wn/4, wn/4, 0 };

int melody1[] = {
   NOTE_G3, NOTE_G3, NOTE_G3, NOTE_G4, NOTE_B3, NOTE_G4, 0 };
int noteDurations1[ ] = {
       wn/8,   wn/8,   wn/8,     wn/4,    wn/8,    wn/1, 0 };

int melody2[ ] = {
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, 
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4, 0 };
int noteDurations2[ ] = {
   wn/4,    wn/4,     wn/4,    wn/4,    wn/4,    wn/4,   wn/2,
   wn/4,    wn/4,     wn/4,    wn/4,    wn/4,    wn/4,   wn/2,
   wn/4,    wn/4,     wn/4,    wn/4,    wn/4,    wn/4,   wn/2,
   wn/4,    wn/4,     wn/4,    wn/4,    wn/4,    wn/4,   wn/2,
   wn/4,    wn/4,     wn/4,    wn/4,    wn/4,    wn/4,   wn/2,
   wn/4,    wn/4,     wn/4,    wn/4,    wn/4,    wn/4,   wn/2, 0 };
   

int* mptr;
int* dptr;

void playtune( uint8_t tunenr ) {

  switch( tunenr ) {
    case 0:
            mptr = &melody0[0];
            dptr = &noteDurations0[0];
            break;
    case 1:
            mptr = &melody1[0];
            dptr = &noteDurations1[0];
            break;
    case 2:
            mptr = &melody2[0];
            dptr = &noteDurations2[0];
            break;
    default:
            mptr = &melody0[0];
            dptr = &noteDurations0[0];
  }; // switch on tune selector
  // iterate over the notes of the melody:
//  for (int thisNote = 0; thisNote < NRNOTES; thisNote++) {
//  int thisNote = 0;
  while( *mptr ) {
    int freq = EXTMULT * (*mptr);
    if( freq < NOTE_B1 ) freq = 0;  // insert rest
    tone(TONEPIN, freq, *dptr );

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
//    int pauseBetweenNotes = noteDurations[thisNote] * 1.30;
    int pauseBetweenNotes = *dptr * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(TONEPIN);
//    thisNote++;
    mptr++;
    dptr++;
  } // loop over notes
} // playtune

void kbdBeep( int note, int durn ) {
  tone( TONEPIN, EXTMULT*note, durn );
} // kbdBeep
