//codeTable.h - morse characters translation from ascii text
//
// created: Feb 23, 2013 G. D. (Joe) Young <jyoung@islandnet.com>
//
// revised: Feb 28/13 - remove lower case by remapping lookup index
//                      (see .cpp for description). Add some punctn.
//
// Each byte entry encodes one character. The variable length is
// handled by using a 'fence' bit just before the first element
// of the code, followed by the code bits with 1 ==> dash, 0 ==> dot.
// Non-translateable charaters encode as SP (0b00000000)

//#include <ctype.h>


const unsigned char morseTable[ ] = {
  0b00000000, 0b01101011, 0b01010010, 0b00000000, 0b10001001, //SP!"#$
  0b00000000, 0b00101000, 0b01011110, 0b00110110, 0b01101101, //%&'()
  0b00000000, 0b00101010, 0b01110011, 0b01100001, 0b01010101, //*+,-.
  0b00110010, ///
  0b00111111, 0b00101111, 0b00100111, 0b00100011, 0b00100001, //01234
  0b00100000, 0b00110000, 0b00111000, 0b00111100, 0b00111110, //56789
  0b01111000, 0b01101010, 0b01101101, 0b00110001, 0b01101101, //:;<=>
  0b01001100, 0b01011010, 0b00000101, 0b00011000, 0b00011010, //?@ABC
  0b00001100, 0b00000010, 0b00010010, 0b00001110, 0b00010000, //DEFGH
  0b00000100, 0b00010111, 0b00001101, 0b00010100, 0b00000111, //IJKLM
  0b00000110, 0b00001111, 0b00010110, 0b00011101, 0b00001010, //NOPQR
  0b00001000, 0b00000011, 0b00001001, 0b00010001, 0b00001011, //STUVW
  0b00011001, 0b00011011, 0b00011100, 0b00110110, 0b00110010, //XYZ[ bks
  0b01101101, 0b00000000, 0b01001101  //]^_ 
//  0b01011110, 0b00000101, 0b00011000, 0b00011010, //'abc
//  0b00001100, 0b00000010, 0b00010010, 0b00001110, 0b00010000, //defgh
//  0b00000100, 0b00010111, 0b00001101, 0b00010100, 0b00000111, //ijklm
//  0b00000110, 0b00001111, 0b00010110, 0b00011101, 0b00001010, //nopqr
//  0b00001000, 0b00000011, 0b00001001, 0b00010001, 0b00001011, //stuvw
//  0b00011001, 0b00011011, 0b00011100, 0b01101101, 0b00110010, //xyz{|
//  0b01101101, 0b00000000, 0b00000000  //}~DEL
}; // morseTable
