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

int currentStep = 1;
int count = 0;
int skipper = 0;
boolean switcher = false;
int pauses, per_pulse, remainder, steps, pulses, noskip, skipXTime;

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
  
    steps = analogRead(0) / 20; //about 50 possible values
    pulses = analogRead(1) / 20; //about 50 possible values
    
    if(pulses > steps){
      int tmpPulses = pulses;
      pulses = steps;
      steps = tmpPulses;
    }
    
    int pauses = steps - pulses;
    
    Serial.print('loop {');
    Serial.print(pulses);
    Serial.println(',');
    Serial.print(steps);
    Serial.print('}');
    Serial.println(); 
    
    switcher = false;
    if (pulses > pauses) {
	switcher = true;
	// XOR swap pauses and pulses
	pauses ^= pulses;
	pulses ^= pauses;
	pauses ^= pulses;
    }
		
    if(pulses == 0 || pauses == 0){
        writeStep(switcher);
        return;
    }else{
		
        per_pulse = (int)floor(pauses/pulses);
	remainder = pauses % pulses;

	noskip = (remainder == 0) ? 0 : (int)floor(pulses / remainder);
	skipXTime = (noskip == 0) ? 0 : (int)floor(pulses - remainder)/noskip;
			
    }
       
        if(switcher) {
            // XOR swap pauses and pulses
            pauses ^= pulses;
            pulses ^= pauses;
            pauses ^= pulses;
        }  
  
  
  if(currentStep > steps){
     count = 0;
     skipper = 0;
     currentStep = 1;
  }
  
  // check to see if the clock as been set
  if (clkState == HIGH) {
    clkState = LOW;
    
      currentStep++;

	if (count == 0) {
                writeStep(!switcher);
                 count = per_pulse;
                
                if (remainder > 0 && skipper == 0) {
		    count++;
	            remainder--;
                    skipper = (skipXTime > 0) ? noskip : 0;
                    skipXTime--;
                } else {
                    skipper--;
                }
	} else {
            writeStep(switcher);
  	    count--;
	}
  }

//TODO: Do something fun with this output    
//  dacOutput(oct[tempOct]);
  
  // do we have to turn off any of the digital outputs?
//  for (int i=0; i<2; i++) {
//    if ((digState[i] == HIGH) && (millis() - digMilli[i] > trigTime)) {
//      digState[i] = LOW;
//      digitalWrite(digPin[i], LOW);
//    }
//  }
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

void writeStep(boolean on){
    if(on){
        digitalWrite(digPin[0], HIGH);
        digitalWrite(digPin[1], LOW);
    }else{
        digitalWrite(digPin[0], LOW);
        digitalWrite(digPin[1], HIGH);
    }  
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

//  deJitter(int, int) - smooth jitter input
//  ----------------------------------------
int deJitter(int v, int test)
{
  // this routine just make sure we have a significant value
  // change before we bother implementing it. This is useful
  // for cleaning up jittery analog inputs.
  if (abs(v - test) > 8) {
    return v;
  }
  return test;
}

//  quantNote(int) - drop an incoming value to a note value
//  -------------------------------------------------------
int quantNote(int v)
{
  // feed this routine the input from one of the analog inputs
  // and it will return the value in a 0-64 range, which is
  // roughly the equivalent of a 0-5V range.
  return v >> 4;
}

//  ===================== end of program =======================
