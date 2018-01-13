class matrix  {
public:
  matrix(byte latch, byte enable);
  
  /*  NOTE:
  the display is stored as follows:
  If the ints were written MSB first out in binary, with each int on its own line, it would show the display.
  i.e.,    0101000000000000
           0000000000000000
           1000100000000000
           0111000000000000   etc...   is a smiley face in the upper left of the display, if these 4 ints are indices 0-3 in the array
  Keep this order in mind when writing to display variables  */
  unsigned int red[8];
  unsigned int green[8];
  void scan();
  void enable();
  void disable();

private:
  byte _latch;
  byte _enable;
  byte _currentRow;
  void shiftSwap(byte value);
};

matrix::matrix(byte latch, byte enable)  {
  _latch = latch;
  _enable = enable;
  pinMode(_latch, OUTPUT);
  pinMode(_enable, OUTPUT);
  digitalWrite(_latch, HIGH);
  digitalWrite(_enable, HIGH);
}

void matrix::scan()  {
  digitalWrite(_latch, LOW);
 
  shiftSwap(0x80 >> _currentRow);
  //SPI.transfer(1 << _currentRow);
  shiftSwap(byte(green[_currentRow]));
  shiftSwap(byte(red[_currentRow]));
  
  
  shiftSwap(0x80 >> _currentRow);
  //SPI.transfer(1 << _currentRow);
  shiftSwap(byte(green[_currentRow]>>8));
  shiftSwap(byte(red[_currentRow]>>8));
  
  digitalWrite(_latch, HIGH);
  _currentRow = (++_currentRow) % 8;
}

void matrix::enable()  {
  digitalWrite(_enable, LOW);
}

void matrix::disable()  {
  digitalWrite(_enable, HIGH);
}

void matrix::shiftSwap(byte value)  {
  byte newByte = ((value << 4) & 0xf0) | ((value >> 4) & 0x0f);
  SPI.transfer(newByte);
}
