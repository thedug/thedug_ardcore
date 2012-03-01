#include <math.h>

// ************************
//  EDIT YOUR MELODIES HERE!
// ************************

String notes1 = "ccggaagffeeddc "; // a space represents a rest
String beats1 = "111111211111124";
int tempo1 = 450;

String notes2 = "ccggaagffeeddc "; // a space represents a rest
String beats2 = "111111211111124";
int tempo2 = 450;

String notes3 = "ccggaagffeeddc "; // a space represents a rest
String beats3 = "111111211111124";
int tempo3 = 450;

String notes4 = "ccggaagffeeddc "; // a space represents a rest
String beats4 = "111111211111124";
int tempo4 = 450;

String notes5 = "ccggaagffeeddc "; // a space represents a rest
String beats5 = "111111211111124";
int tempo5 = 450;

String notes6 = "ccggaagffeeddc "; // a space represents a rest
String beats6 = "111111211111124";
int tempo6 = 450;

String notes7 = "ccggaagffeeddc "; // a space represents a rest
String beats7 = "111111211111124";
int tempo7 = 450;

String notes8 = "ccggaagffeeddc "; // a space represents a rest
String beats8 = "111111211111124";
int tempo8 = 450;

String notes9 = "ccggaagffeeddc "; // a space represents a rest
String beats9 = "111111211111124";
int tempo9 = 450;

String notes10 = "ccggaagffeeddc "; // a space represents a rest
String beats10 = "111111211111124";
int tempo10 = 450;

String notes11 = "ccggaagffeeddc "; // a space represents a rest
String beats11 = "111111211111124";
int tempo11 = 450;

String notes12 = "ccggaagffeeddc "; // a space represents a rest
String beats12 = "111111211111124";
int tempo12 = 450;

// ************************
//  EDIT YOUR MELODIES HERE!
// ************************


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

String allNotes[12] = {notes1,notes2,notes3,notes4,notes5,notes6,notes7,notes8,notes9,notes10,notes11,notes12};
String allBeats[12] = {beats1,beats2,beats3,beats4,beats5,beats6,beats7,beats8,beats9,beats10,beats11,beats12};
int allTempos[12] = {tempo1,tempo2,tempo3,tempo4,tempo5,tempo6,tempo7,tempo8,tempo9,tempo10,tempo11,tempo12};


char names[] = { 'a', 'A', 'b', 'c', 'C', 'd', 'D', 'e', 'f', 'F', 'g', 'G' };
int tones[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48};

int reset, octave, transpose, lengthMod, sequence;


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

void playNote(char note, int duration, int transpose, int octave) {

  if(note != /* REST */ ' ' ){

    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
  
    for (int i = 0; i < 12; i++) {
      
      if (names[i] == note) {
          //TODO: add octave
          dacOutput(tones[i] + transpose + (12 * octave * 4));
      }
    }
  }
  
  //hold the note 
  delay(duration/2);
  
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  
  //hold the note 
  delay(duration/2);  
  
}


void loop() {
  

  reset=digitalRead(clkIn); 
  
  // 
  int length = allNotes[sequence].length() ;
  for (int i = 0; i < length; i++) {

    sequence=analogRead(0) / 20; //12 sequences
    octave=analogRead(1) / 205; // 5 octaves
    transpose=analogRead(2) /86;  // 12 transpose values
    lengthMod=analogRead(3) / 86 + 1;      
    
     if(reset == HIGH){
       i = 0;
     }
     playNote(allNotes[sequence][i], (allBeats[sequence][i] - '0') * allTempos[sequence] / lengthMod, transpose, octave);

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



