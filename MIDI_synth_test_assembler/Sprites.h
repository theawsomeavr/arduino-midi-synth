/*  Digits and other characters are monochrome, 4x4 pixels.  Fits in one unsigned int.

@ = on      . = off

0:      1:      2:      3:      4:      5:      6:      7:      8:      9:

@@@@    .@@.    @@@.    @@@@    @.@@    @@@@    @@@@    @@@@    @@@@    @@@@
@.@@    @@@.    ..@@    .@@@    @.@@    @@@.    @...    ..@@    @..@    @@@@
@.@@    .@@.    .@@.    ...@    @@@@    ..@@    @@@@    .@@.    @@@@    ...@
@@@@    @@@@    @@@@    @@@@    ..@@    @@@.    @@@@    @@..    @@@@    @@@@

Order of bits in the int:  LSB = 0, MSB = F:
FEDC
BA98
7654
3210

Watch for right order when reading sprites!
*/

unsigned int digits[10] = {
  0xfbbf,
  0x6e6f,
  0xe36f,
  0xf71f,
  0xbbf3,
  0xfe3e,
  0xf8ff,
  0xf36c,
  0xf9ff,
  0xff1f
};
  
  
  
