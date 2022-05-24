//There's a floating voltage somewhere, because I can't keep Integral at 0. Sometimes it get's stuck at e.g. -10, which then breaks
//Midi integral, because that's mapped from 0 to 1023. Maybe this should change aswell.

#include "MIDIUSB.h"
    // Integrate motor voltage example
int Integral;
int aVal;
int lastMidiIntegral;
#define zeroOffset 517

void setup() {
  Serial.begin(115200);
}

void loop() {
  aVal = (analogRead(A0) -zeroOffset);
  Integral += aVal/2;
  Serial.print(analogRead(A0) -zeroOffset); //What's this for?
  Serial.print("  ");
//  Serial.println(Integral);
  delay(100);

  int midiIntegral = map(Integral,0, 1023, 0, 127);
//  constrain(midiIntegral, 0, 127);
  if (midiIntegral>127){
    midiIntegral = 127;
  }
  else if (midiIntegral <0){
    midiIntegral =0;
  }
  
  Serial.println(midiIntegral);
  delay(10);
  if(midiIntegral != lastMidiIntegral){
  controlChange(2, 9, midiIntegral); //  (channel, CC number,  CC value) (slows down operation of wheel)
  MidiUSB.flush();
  }
  lastMidiIntegral = midiIntegral;
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
