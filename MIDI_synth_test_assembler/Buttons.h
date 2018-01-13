void beginButtons(byte ssPin)  {
  pinMode(ssPin, OUTPUT);
  digitalWrite(ssPin, LOW);
}

byte readButtons()  {
  PORTD = PORTD | B00100000;
  byte reading = SPI.transfer(0x00);
  PORTD = PORTD & B11011111;
  return reading;
}

// Button bits in reading byte.  LSB = 0, MSB = 7
// e.g. to read the state of one button, reading & (1<<BUTTONBIT)

#define BUTTON_UP 0       // four d-pad buttons
#define BUTTON_DOWN 2
#define BUTTON_LEFT 3
#define BUTTON_RIGHT 1
#define BUTTON_A 4  //the upper of the two non-dpad buttons.  black
#define BUTTON_B 6  // the lower. red

// two bits are unused, only 6 buttons.
