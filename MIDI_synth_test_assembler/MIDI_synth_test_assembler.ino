/*
Pin mappings
 
 0   \__  serial, used for MIDI
 1   /
 2
 3
 4
 5  button board slave select - HIGH to transfer data
 6  matrix enable pin - LOW for matrix is on
 7  matrix slave select pin - LOW to transfer data
 8
 9    \_ audio output pins - only 9 is wired up for now
 10   /
 11  SPI  \__ MOSI and MISO, can't remember which is which
 12  SPI  /
 13  SPI clock
 
 A0  \
 A1   |
 A2   \__ the 6 knobs, from right to left
 A3   /
 A4   |
 A5  /
 
 */
#include "Sprites.h"
#include "wavetables.h"


#include <SPI.h>
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

#include "Drawing.h"
#include "LEDMatrix.h"
#include "Buttons.h"

int NoteOn=0x90;
int NoteOff=0x80;


//  A MIDI simple monophonic synth, with wavetable synthesis.

const int notes[] = {
  2093, 2217, 2349,    // other octaves by dividing by power of 2
  2489, 2637, 2794, 2960,   //C7 - B7
  3136, 3322, 3520, 3729, 3951
};



struct oscillatorStruct  {
  volatile unsigned int ticks;
  volatile unsigned int phase;
  volatile byte waveform;  // 0=square,  1=sawtooth,  2=triangle
  volatile byte dutyCycle;  //square w/ dutycycle comes from comparing sawtooth to this
  volatile byte volume;

};

struct oscillatorStruct oscillators[8];


//class declaring/initializing
//MIDI_Class MIDI;
matrix screen(7, 6);


//MIDI MIDI variables
byte currentNotes[4][8];
//byte currentVelocities[4][8];
byte lastWritten[4];
boolean newNotes[4][8];
byte maxPolyphony = 4;


// volatile variables used in ISR
volatile int outputA = 0;
//volatile int outputB = 0;



void setup()  {

  MIDI.begin(MIDI_CHANNEL_OMNI);  //initialize MIDI MIDI 

  //=====================================================================
  //===========================   SPI THINGS  ===========================
  //=====================================================================

  SPI.begin();
  SPI.setBitOrder(LSBFIRST);

  screen.enable();
  beginButtons(5);

  //=====================================================================
  //=================    OUTPUT TIMER SETTINGS     ======================
  //=====================================================================

  pinMode(9, OUTPUT);  //set OC1A pin to output
  pinMode(10, OUTPUT);  //set OC1B pin to output

  //Setting up timer1 for fast pwm mode
  TCCR1A &= ~(1 << WGM10);  
  TCCR1A |= 1 << WGM11;
  TCCR1B |= 1 << WGM12;
  TCCR1B |= 1 << WGM13;

  //Prescalar 1
  TCCR1B |= 1<< CS10;
  TCCR1B &= ~(1 << CS11);
  TCCR1B &= ~(1 << CS12);

  TCCR1A &= ~(1 << COM1A0);
  TCCR1A |= 1 << COM1A1;
  TCCR1A &= ~(1 << COM1B0);
  TCCR1A |= 1 << COM1B1;

  ICR1 = 511;
  OCR1A = 0;
  OCR1B = 0;

  TIMSK1 |= 1 << TOIE1;

  sei();


  //=====================================================================
  //========================    OTHER THINGS     ========================
  //=====================================================================

  setAllVolumes(0);
  setAllDutyCycles(127);
  pinMode(A0, OUTPUT);  //set OC1A pin to output
  pinMode(A1, OUTPUT);
   pinMode(A2, OUTPUT);  //set OC1A pin to output
  pinMode(A3, OUTPUT);
   pinMode(A4, OUTPUT);  //set OC1A pin to output
  pinMode(A5, OUTPUT);
  digitalWrite(A0,1);
  digitalWrite(A2,1);
  digitalWrite(A3,1);
  digitalWrite(A4,1);
  digitalWrite(A5,1);
  digitalWrite(A1,1);






}



void loop()  {

  //WILL BE REMOVED
  int reading = analogRead(5);
  if(reading < 350)  {
    setAllWaveforms(0);
  }
  else if(reading > 700)  {
    setAllWaveforms(2);
  }
  else {
    setAllWaveforms(1);
  }

  reading = analogRead(4);
  reading >>= 2;
  setAllDutyCycles(byte(reading));


  //===========================================================================
  //======================   MIDI MIDI HANDLING   ============================
  //===========================================================================

  if(MIDI.read() == true)  {  //do we get signal
    byte channel = MIDI.getChannel() - 1;  //MIDI function returns 1-16.  we want 0-15, to use as array indices
    byte type = MIDI.getType();
    byte data1 = MIDI.getData1();
    byte data2 = MIDI.getData2();
    if(channel <= 3)  {              // is it within the number of channels?
      if(type == NoteOn && data2 != 0)  {  //is it a noteOn event, and not with 0 velocity?
        for(int i = 0; i <= maxPolyphony; i++)  {
          if(i == maxPolyphony)  {
            currentNotes[channel][lastWritten[channel]] = data1;
            //currentVelocities[channel][lastWritten[channel]] = data2;
            newNotes[channel][lastWritten[channel]] = true;
            break;
          }
          else if(currentNotes[channel][i] == 0)  {
            currentNotes[channel][i] = data1;
            //currentVelocities[channel][i] = data2;
            newNotes[channel][i] = true;
            lastWritten[channel] = i;
            break;
          }
        }
      }
      else if(type == NoteOff || data2 == 0)  {  //is it a NoteOff event,
        for(int i = 0; i < maxPolyphony; i++)  {
          if(currentNotes[channel][i] == data1)  {
            currentNotes[channel][i] = 0;
            newNotes[channel][i] = true;
            break;
          }
        }
      }
    }
  }

  //  End of midi MIDI handling


  //WILL BE REMOVED/MODIFIED
  for(int i = 0; i < maxPolyphony; i++)  {
    if(newNotes[0][i]==true)  {
      newNotes[0][i] = false;
      if(currentNotes[0][i] == 0)  {
        oscillators[i].volume = 0;
      }
      else  {
        int freq = midi2Freq(currentNotes[0][i]);
        setFreq(i, freq);
      }
    }
  }


  screen.scan();



  //=============================================================
  //=======   TESTING SCREEN+CHAR DISPLAY   ====================



  clearGrid(screen.red);
  clearGrid(screen.green);

  if(oscillators[0].volume > 0)  { 
    drawChar(screen.green, digits[1], 0, 0);
  }
  if(oscillators[1].volume > 0)  { 
    drawChar(screen.red, digits[3], 4, 0);
  }
  if(oscillators[2].volume > 0)  { 
    drawChar(screen.green, digits[3], 8, 0);
  }
  if(oscillators[3].volume > 0)  { 
    drawChar(screen.red, digits[7], 12, 0);
  }

}



//===========================================================================
//=======================  ISR+ASSEMBLER STUFF  =============================
//===========================================================================


ISR(TIMER1_OVF_vect)  {                  //ASSEMBLER WOOT
  unsigned int ticks;
  byte phaseLow;
  byte volume;
  byte waveform;
  byte duty;

  OCR1A = outputA;

  byte loopVar;

  __asm__ volatile (
  "ldi %A[outputA],0"                            "\n\t"
    "ldi %B[outputA],0"                           "\n\t"
    "ldi %[loopVar],4"                             "\n\t"
    "LoopBegin:"                                 "\n\t"
    //Loads the two bytes of the ticks variable, and then the two bytes of the phase variable
  //The high byte of the phase variable goes right to the low byte of the wavetable index
  "ld %A[ticks],%a[oscBaseAddress]+"           "\n\t" 
    "ld %B[ticks],%a[oscBaseAddress]+"            "\n\t" 
    "ld %[phaseLow],%a[oscBaseAddress]"              "\n\t" 
    "ldd %A[waveformAddress],%a[oscBaseAddress]+1"  "\n\t" 
    //Update the phase variable in SRAM.  Add ticks to it, then store the two resulting bytes
  "add %[phaseLow],%A[ticks]"                      "\n\t" 
    "adc %A[waveformAddress],%B[ticks]"           "\n\t" 
    "st %a[oscBaseAddress]+,%[phaseLow]"           "\n\t" 
    "st %a[oscBaseAddress]+,%A[waveformAddress]"  "\n\t" 
    //Load the waveform select byte, then the duty cycle byte
  //The duty cycle byte is unused unless the waveform select == 0.
  //But the pointer address still needs to be incremented past it anyway
  "ld %[waveform],%a[oscBaseAddress]+"           "\n\t" 
    "ld %[dutyVol],%a[oscBaseAddress]+"            "\n\t" 
    //Is the waveform select 0?  i.e., square wave?
  "tst %[waveform]"                              "\n\t" 
    "brne NotSquare"                             "\n\t" 
    //Compare the waveform sample to the duty cycle-first waveform is a sawtooth, standard PWM procedure
  //Output is 256 if >= dutycycle, 0 if < dutycycle
  "ld %[phaseLow],%a[waveformAddress]"           "\n\t" 
    "cp %[phaseLow],%[dutyVol]"                      "\n\t" 
    "brlo SquareOff"                             "\n\t" 
    "ser %[phaseLow]"                              "\n\t" 
    "rjmp VolumeMult"                            "\n\t" 
    "SquareOff:"                                 "\n\t" 
    "clr %[phaseLow]"                              "\n\t" 
    "rjmp VolumeMult"                            "\n\t" 
    //All other waveforms
  //Must be decremented - sawtooth waveform index is 0, but waveform select is 1
  //Adds resulting waveform select value to the high byte of the wavetable address
  //Loads the sample, then subtracts the waveform select value, to reset the address
  "NotSquare:"                                 "\n\t" 
    "dec %[waveform]"                              "\n\t" 
    "add %B[waveformAddress],%[waveform]"          "\n\t" 
    "ld %[phaseLow],%a[waveformAddress]"           "\n\t" 
    "sub %B[waveformAddress],%[waveform]"          "\n\t" 
    //256 possible volume levels.  So to get the right % volume, multiply by the volume byte
  // then divide by 256.  Easy, the mult operation does this for us.  Just take the high byte
  // of the multiplication result.  Then add the result to the output variable.
  "VolumeMult:"                                "\n\t" 
    "ld %[dutyVol],%a[oscBaseAddress]+"            "\n\t" 
    "mul %[phaseLow],%[dutyVol]"                     "\n\t" 
    "clr r0"                                     "\n\t" 
    "add %A[outputA],r1"                         "\n\t" 
    "adc %B[outputA],r0"                         "\n\t"
    //Increment the loop variable.  If it hasn't wrapped around to 0, go back to beginning of loop.
  "dec %[loopVar]"                             "\n\t"
    "brne LoopBegin"                             "\n\t"

:  //outputs
  [ticks] "=&d" (ticks),
  [phaseLow] "=&d" (phaseLow),
  [waveform] "=&d" (waveform),
  [dutyVol] "=&d" (duty),
  [outputA] "=&d" (outputA),
  [loopVar] "=&d" (loopVar)
:  //MIDIs
  [oscBaseAddress] "y" (&oscillators[0].ticks),
  [waveformAddress] "z" (&waveforms[0][0])
:  //clobbered
  "r1"
    );



}



//===========================================================================
//======================  OTHER GLOBAL FUNCTIONS  ===========================
//===========================================================================


int midi2Freq(byte note)  {
  byte noteIndex = (note) % 12;  //figure which value in note array corresponds to
  //  recieved MIDI note
  byte noteOctave = (note) / 12; //figure out what octave the note is
  int Hz = notes[noteIndex] / (0x200 >> noteOctave);
  return Hz;
}

/*void setFreq(int voice, int freq, byte velocity)  {
 //  Ticks = Hz * 65536 / 31250
 int ticks = freq*65535UL/31250;
 oscillators[voice].ticks = ticks;
 oscillators[voice].volume = velocity;
 }*/

void setFreq(int voice, int freq)  {
  //  Ticks = Hz * 65536 / 31250
  int ticks = freq*65535UL/31250;
  oscillators[voice].ticks = ticks;
  oscillators[voice].volume = 128;
}


void setAllWaveforms(byte waveform)  {
  for(int i = 0; i<= 7; i++)  {
    oscillators[i].waveform = waveform;
  }
}

void setAllVolumes(byte volume)  {
  for(int i = 0; i<= 7; i++)  {
    oscillators[i].volume = volume;
  }
}

void setAllDutyCycles(byte dutyCycle)  {
  for(int i = 0; i<= 7; i++)  {
    oscillators[i].dutyCycle = dutyCycle;
  }
}
