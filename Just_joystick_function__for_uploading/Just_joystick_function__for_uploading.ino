/*
  Based on Sketch built by Gustavo Silveira (aka Music Nerd)
  Modified by Dolce Wang

  This code is only for Arduinos that use ATmega32u4 (like Micro, Pro Micro, Leonardo...)
  Remember to also assign the correct board in the IDE (like Tools / Boards / Sparkfun AVR / Pro Micro...)
NOTE; I HAVE DELETED UNUSED FUNCTIONS (E.G. FOR BUTTONS) FOR THE SAKE OF SIMPLICITY. THIS CODE IS JUST FOR THE MOTOR SCRUBBING WHEEL
*/

// Change any fields with //**


// LIBRARY

#include "MIDIUSB.h"  
#include "math.h"


// Motor
#define Motor_Pin A0
int midiWheelCC = 0;
int lastMidiWheelCC = 0 ;
int lastaverage;



//Motor Smoothening
const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;


// MIDI Assignments 
byte midiCh = 1; //* MIDI channel to be used
byte note = 36; //* Lowest note to be used; 36 = C2; 60 = Middle C
byte cc = 9; //* Lowest MIDI CC to be used


// SETUP
void setup() {

  // Baud Rate
  // 31250 for MIDI class compliant | 115200 for Hairless MIDI

  
  Serial.begin(9600); 

//  Motor smoothening
  for (int thisReading = 0; thisReading<numReadings; thisReading ++){
    readings[thisReading] = 0;
  }

  
}

////
// LOOP
void loop() {
  spinWheel();

}



  for (int i = 0; i < NButtons; i++) {

    buttonCState[i] = digitalRead(buttonPin[i]);  // read pins from arduino

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == HIGH) {

          // Sends the MIDI note ON 
          
         // use if using with ATmega32U4 (micro, pro micro, leonardo...)
          noteOn(midiCh, cc + i, 127);  // channel, note, velocity
          MidiUSB.flush();


        }
        else {

          // Sends the MIDI note OFF accordingly to the chosen board

          // use if using with ATmega32U4 (micro, pro micro, leonardo...)
          noteOn(midiCh, cc + i, 0);  // channel, note, velocity
          MidiUSB.flush();

        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}

////


// if using with ATmega32U4 (micro, pro micro, leonardo...)


// Arduino MIDI functions MIDIUSB Library
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

//Playhead wheel
void spinWheel() {


//   New Smoothening System
    total = total-readings[readIndex];
    readings[readIndex] = analogRead(Motor_Pin);
    total = total + readings[readIndex];
    readIndex = readIndex + 1;

    if (readIndex >= numReadings) {
      readIndex = 0;
    }
    average = (total/numReadings)/10;
//    Serial.println(average); //*debug print
    delay(1);
//  need to figure out a way to make this logorhythmic    
     if (average != lastaverage) { //* if average has changed, change midiwheelcc value
      if (average < 94) //* a motor value, and its 0 point
        midiWheelCC = midiWheelCC -1;

        if (midiWheelCC < 0)
          midiWheelCC = 0;
      else if (average > 94) //*a motor value and its 0 point
        midiWheelCC = midiWheelCC + 1;

        if (midiWheelCC > 127)
          midiWheelCC = 127;



   }  

     average = lastaverage;


//      Serial.println(midiWheelCC);
      delay(1);

     if (lastMidiWheelCC != midiWheelCC) { // Sends a midi message so long as the midiwheel value has changed.
      controlChange(midiCh, 9, midiWheelCC); //  (channel, CC number,  CC value) (slows down operation of wheel)
      MidiUSB.flush();
      Serial.println(midiWheelCC);
     }
      
     lastMidiWheelCC = midiWheelCC; 
      

}
  
