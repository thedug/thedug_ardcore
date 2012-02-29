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

volatile byte badc0;
volatile byte badc1;

int icnt;

int cnt2;
int iw1;

int iw;

byte bb;
byte dd[512];  // Audio Memory Array 8-Bit



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


void loop()
{
  

  bb=dd[icnt] ;              // read the delay buffer
  iw = 127-bb ;              // substract offset
  iw = iw * badc0 / 255;     // scale delayed sample with potentiometer

  iw1 = 127 - badc1;          // substract offset from new sample
  iw1=iw1+iw;                 // add delayed sample and new sample
  if (iw1 < -127) iw1=-127;   // Audio limiter 
  if (iw1 > 127) iw1=127;     // Audio limiter 

  bb= 127+iw1;                // add offset
  dd[icnt]=bb;                // store sample in audio buffer

  icnt++;
  icnt = icnt & 511;         // limit bufferindex 0..511

  cnt2++;               // let the led blink about every second
  if (cnt2 >= 15360){
    cnt2=0;
    PORTB = PORTB ^ 32;   // Toggle LED on Pin 11
  }

  OCR2A=bb;            // Sample Value to PWM Output

  PORTD = PORTD  ^ 128;   // Test Output on pin 7


} // loop
//******************************************************************
void fill_sinewave(){
  float pi = 3.141592;
  float dx ;
  float fd ;
  float fcnt;
  dx=2 * pi / 512;                    // fill the 512 byte bufferarry
  for (iw = 0; iw <= 511; iw++){      // with  50 periods sinewawe
    fd= 127*sin(fcnt);                // fundamental tone
    fcnt=fcnt+dx;                     // in the range of 0 to 2xpi  and 1/512 increments
    bb=127+fd;                        // add dc offset to sinewawe 
    dd[iw]=bb;                        // write value into array

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


