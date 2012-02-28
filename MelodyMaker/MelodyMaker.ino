#include <math.h>

//  constants related to the Arduino Nano pin use
const int clkIn = 2;           // the digital (clock) input
const int digPin[2] = {3, 4};  // the digital output pins
const int pinOffset = 5;       // the first DAC pin (from 5-12)
const int trigTime = 25;       // 25 ms trigger timing

//  variables for interrupt handling of the clock input
volatile int clkState = LOW;

//  variables used to control the current DIO output states
int digState[2] = {LOW, LOW};        // start with both set low
unsigned long digMilli[2] = {0, 0};  // a place to store millis()

String notes[2] = {"ccggaagffeeddc ", "ccggaagffeeddc "}; // a space represents a rest
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 450;

//TODO: Add the following:
//  bank (12)
//  melody
//  octave
//  reset (& length)

//  ==================== start of setup() ======================

//  This setup routine should be used in any ArdCore sketch that
//  you choose to write; it sets up the pin usage, and sets up
//  initial state. Failure to properly set up the pin usage may
//  lead to damaging the Arduino hardware, or at the very least
//  cause your program to be unstable.

void setup() 
{

  // if you need to send data back to your computer, you need
  // to open the serial device. Otherwise, comment this line out.
  Serial.begin(9600);
  
  // set up the digital (clock) input
  pinMode(clkIn, INPUT);
  
  // set up the digital outputs
  for (int i=0; i<2; i++) {
    pinMode(digPin[i], OUTPUT);
    digitalWrite(digPin[i], LOW);
  }
  
  // set up the 8-bit DAC output pins
  for (int i=0; i<8; i++) {
    pinMode(pinOffset+i, OUTPUT);
    digitalWrite(pinOffset+i, LOW);
  }
  
  // set up an interrupt handler for the clock in. If you
  // aren't going to use clock input, you should probably
  // comment out this call.
  // Note: Interrupt 0 is for pin 2 (clkIn)
  attachInterrupt(0, isr, RISING);
   
}

void playNote(char note, int duration) {
  char names[] = { 'a', 'A', 'b', 'c', 'C', 'd', 'D', 'e', 'f', 'F', 'g', 'G' };
  int tones[] = { 51, 55, 60, 64, 68, 72, 77, 81, 85, 89, 94, 98, 102};
  int d = duration;
  // play the tone corresponding to the note name

  if(note != ' '){
    /* not a rest */
    for (int i = 0; i < 12; i++) {
      if (names[i] == note) {
          //TODO: add octave
          dacOutput(tones[i]);
      }
    }
  }
  
  //hold the note 
  delay(duration);
  
}


void loop() {
  int length = notes[0].length() ;
  for (int i = 0; i < length; i++) {
     playNote(notes[0][i], beats[i] * tempo);
  }
}
 


//  =================== convenience routines ===================

//  These routines are some things you will need to use for
//  various functions of the hardware. Explanations are provided
//  to help you know when to use them.

//  isr() - quickly handle interrupts from the clock input
//  ------------------------------------------------------
void isr()
{
  // Note: you don't want to spend a lot of time here, because
  // it interrupts the activity of the rest of your program.
  // In most cases, you just want to set a variable and get
  // out.
  clkState = HIGH;
}

//  dacOutput(long) - deal with the DAC output
//  ------------------------------------------
void dacOutput(long v)
{
  // feed this routine a value between 0 and 255 and teh DAC
  // output will send it out.
  int tmpVal = v;
  for (int i=0; i<8; i++) {
    digitalWrite(pinOffset + i, tmpVal & 1);
    tmpVal = tmpVal >> 1;
  }
}


//  ===================== end of program =======================


