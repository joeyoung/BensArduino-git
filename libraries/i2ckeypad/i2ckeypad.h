#ifndef i2ckeypad_h
#define i2ckeypad_h

#include <inttypes.h>

class i2ckeypad {
public:
//  i2ckeypad(int);
  i2ckeypad( char*, int, int, int);
  char get_key( int );
  void init( int );
  
private:
  char* keymap;
  void pcf8574_write(int, int, int);
  int pcf8574_byte_read(int);
  int pcf8574_i2c_addr;
  int num_rows;
  int num_cols;
  static int row_select[2];
  static int current_data[2];
  static int temp_key[2];
};

#endif

