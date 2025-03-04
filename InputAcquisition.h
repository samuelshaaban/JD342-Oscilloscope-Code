#ifndef INPUT_ACQUISITION_H
#define INPUT_ACQUISITION_H

#include "DataTypes.h"
#include "ADC.h"


// Toggle switches
#define ENABLE_CH1 14
#define ENABLE_CH2 15
#define TRIGGER_ENABLE 16
#define TRIGGER_DECREASE 17
#define TRIGGER_CH2 18

// Rotary encoder
#define ENCODER_CH 19 // Selects channel
#define ENCODER_SCALE_SHIFT 20 // Selects scale or shift
#define ENCODER_TIME_TRIGGER 21 // Off -> default, on -> scale=time, shift=trigger, CH ignored
#define ENCODER_COARSE 27// ON = 100x encoder speed
// Attached to interrupts, pins must support
#define ENCODER_A 22
#define ENCODER_B 23


// First pulse every sample cycle sets direction, other channel will pulse while the first is still on
// Encoder shorts pins to ground, a pulse is a low value
// Return if count should change
bool encoder(int ch1, int ch2) {
  while(digitalRead(ch2) == LOW); // After CH2 is released
  while(digitalRead(ch1) == LOW) // Before CH1 is released
    if(digitalRead(ch2) == LOW) return true; // If CH2 pulses, return to change

  return false; // else no change
}

// Running count, following functions lock the direction until updateEncoder() is called
int encoderChange = 0;

void encoderA() {
  if(encoderChange < 0) return;
  if(encoder(ENCODER_A, ENCODER_B)) encoderChange++;
}

void encoderB() {
  if(encoderChange > 0) return;
  if(encoder(ENCODER_B, ENCODER_A)) encoderChange--;
}


// Ensures int stays between min and max
void updateInt(int &dst, int min, int max, int change) {
  dst += change;
  if(dst > max) dst = max;
  if(dst < min) dst = min;
}

// uses encoderChange count
bool updateEncoder(DisplayAdjust &display, Trigger &trigger) {
  if(encoderChange == 0) return false;
  if(digitalRead(ENCODER_COARSE) == LOW) encoderChange *= 100; // Read switch for 100x

  // Read switches to determine what to change
  bool CH2 = digitalRead(ENCODER_CH) == LOW,
       shift = digitalRead(ENCODER_SCALE_SHIFT) == LOW,
       timeTrigger = digitalRead(ENCODER_TIME_TRIGGER) == LOW;
  if(timeTrigger) {
    if(shift) updateInt(trigger.val, 0, 1000, encoderChange);
    else      updateInt(display.timeScale, 1, 10000, encoderChange);
  } else { // CH voltage settings
    if(CH2) {
      if(shift) updateInt(display.CH2Shift, -20000, 20000, encoderChange);
      else      updateInt(display.CH2Scale, 1, 40000, encoderChange);
    } else { // CH1
      if(shift) updateInt(display.CH1Shift, -20000, 20000, encoderChange);
      else      updateInt(display.CH1Scale, 1, 40000, encoderChange);
    }
  }

  encoderChange = 0;
  return true;
}


void initControls() {
  pinMode(ENABLE_CH1, INPUT_PULLUP);
  pinMode(ENABLE_CH2, INPUT_PULLUP);
  pinMode(TRIGGER_ENABLE, INPUT_PULLUP);
  pinMode(TRIGGER_DECREASE, INPUT_PULLUP);
  pinMode(TRIGGER_CH2, INPUT_PULLUP);

  pinMode(ENCODER_CH, INPUT_PULLUP);
  pinMode(ENCODER_SCALE_SHIFT, INPUT_PULLUP);
  pinMode(ENCODER_TIME_TRIGGER, INPUT_PULLUP);
  pinMode(ENCODER_COARSE, INPUT_PULLUP);
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderA, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), encoderB, FALLING);
}


// Return if bool was changed
bool updateBool(bool &dst, bool src) {
  if(dst == src) return false;
  dst = src;
  return true;
}

bool updateTrigger(Trigger &trigger) {
  return updateBool(trigger.enable, digitalRead(TRIGGER_ENABLE) == LOW) ||
         updateBool(trigger.decrease, digitalRead(TRIGGER_DECREASE) == LOW) ||
         updateBool(trigger.CH2, digitalRead(TRIGGER_CH2));
}



void initInput() {
  initADCPins();
  initControls();

  initADCRegisters();
}

bool acquireInput(Buffer &CH1, Buffer &CH2, DisplayAdjust &display, Trigger &trigger) {
  // Check channel enable switches and get a reading from the ADC
  CH1.setEnable(digitalRead(ENABLE_CH1 == LOW));
  CH2.setEnable(digitalRead(ENABLE_CH2 == LOW));
  CRTRead(CH1, CH2);

  return updateEncoder(display, trigger) ||
         updateTrigger(trigger) ||
         CH1.changed() ||
         CH2.changed();
}

#endif