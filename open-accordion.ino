/*
   This examples shows how to make a simple seven keys MIDI keyboard with volume control

   Created: 4/10/2015
   Author: Arturo Guadalupi <a.guadalupi@arduino.cc>
   
   http://www.arduino.cc/en/Tutorial/MidiDevice
*/

#include "MIDIUSB.h"
#include "PitchToNote.h"
#define NUM_BUTTONS  7

const uint8_t button1 = 22;   
const uint8_t button2 = 24;   //A1 = 55. Overdefined by PitchToNote.h
const uint8_t button3 = 26;   //A2 = 56. Overdefined by PitchToNote.h
const uint8_t button4 = 5;
const uint8_t button5 = 6;
const uint8_t button6 = 7;
const uint8_t button7 = 8;

const int intensityPot = 54;  //A0 = 54. Overdefined by PitchToNote.h

int cc = 0;

const uint8_t buttons[NUM_BUTTONS] = {button1, button2, button3, button4, button5, button6, button7};
byte buttonState[NUM_BUTTONS] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW};
byte prevButtonState[NUM_BUTTONS] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW};

const byte notePitches[NUM_BUTTONS] = {C3, D3, E3, F3, G3, A3, B3};

uint8_t notesTime[NUM_BUTTONS];
uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;
uint8_t intensity;

void setup() {
  for (int i = 0; i < NUM_BUTTONS; i++)
    pinMode(buttons[i], INPUT_PULLUP);

 
  Serial.begin(9600);
}

void loop() {
  readButtons();
  readIntensity();
  playNotes();
  //noteOn(0, 48, 64);
  //delay(1000);
  //noteOff(0, 48, 64);
  //delay(1000);
  Serial.println(intensity);
  //Serial.println(buttonState[1]);
  //Serial.println(buttonState[2]);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

// Value = 0-127
void setVolume(byte channel, byte value) {
  controlChange(channel, 0x07, value);
}

void readButtons()
{
  //Serial.println(analogRead(buttons[1]));
  buttonState[0] = digitalRead(buttons[0]);
  buttonState[1] = digitalRead(buttons[1]);
  buttonState[2] = digitalRead(buttons[2]);
 /* for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (digitalRead(buttons[i]) == LOW)
    {
      bitWrite(pressedButtons, i, 1);
      delay(50);
    }
    else
      bitWrite(pressedButtons, i, 0);
  }*/
}

void readIntensity()
{
  int val = analogRead(intensityPot);
  intensity = (uint8_t) (map(val, 0, 1023, 0, 127));
}

void playNotes()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (buttonState[i] != prevButtonState[i]) 
    {
      prevButtonState[i] = buttonState[i];
      if (buttonState[i]) {    
        noteOn(0, notePitches[i], 64);
        //MidiUSB.flush();
      } else {
        noteOff(0, notePitches[i], 0);
        //MidiUSB.flush();
      }
    }
  }
  
  setVolume(0,intensity);
  
}

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}


void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}
