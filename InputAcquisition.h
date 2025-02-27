#ifndef INPUT_ACQUISITION_H
#define INPUT_ACQUISITION_H

#include "DataTypes.h"
#include <bitset>



// ADC commands
#define SET_BIT 0b0101<<12

// ADC registers
#define PROTOCOL_CFG  2<<8
#define BUS_WIDTH     3<<8
#define CRT_CFG       4<<8

// ADC register field bit shifts
#define SDO_PROTOCOL    4
#define SDO_WIDTH       0
#define CRT_CLK_SELECT  0


// ADC control pins
#define CS 1 // held high during register programming, held low to send STROBE to CONVST when looping
#define SCLK 2
#define SDI 3
#define CONVST 4
#define READY_STROBE 5

// ADC data out bus
#define SDO0A 6
#define SDO1A 7
#define SDO2A 8
#define SDO3A 9
#define SDO0B 10
#define SDO1B 11
#define SDO2B 12
#define SDO3B 13

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
// Attached to interrupts, must be 
#define ENCODER_A 22
#define ENCODER_B 23

// Delays in ns
#define NOP() asm("nop\n") // One 240 MHz clock cycle = 4.167 ns
#define DELAY15NS() NOP(); NOP(); NOP(); NOP() // 4 NOP = 16.6 ns
void pulse(int pin) {
  digitalWrite(pin, HIGH);
  DELAY15NS();
  digitalWrite(pin, LOW);
}

// First pulse every sample cycle sets direction, conunt direction pulse, all pulses of other channel are ignored
int encoderChange = 0;
void encoderInc() {
  if(encoderChange < 0) return;
  encoderChange++;
}

void encoderDec() {
  if(encoderChange > 0) return;
  encoderChange--;
}

void initADCPins() {
  pinMode(CS, OUTPUT); 
  pinMode(SCLK, OUTPUT);
  pinMode(SDI, OUTPUT);
  pinMode(CONVST, OUTPUT);
  digitalWrite(CS, HIGH); //CS stays HIGH when not communicating
  digitalWrite(SCLK, LOW);
  digitalWrite(CONVST, HIGH); // Held high for init

  pinMode(READY_STROBE, INPUT);
  pinMode(SDO0A, INPUT);
  pinMode(SDO1A, INPUT);
  pinMode(SDO2A, INPUT);
  pinMode(SDO3A, INPUT);
  pinMode(SDO0B, INPUT);
  pinMode(SDO1B, INPUT);
  pinMode(SDO2B, INPUT);
  pinMode(SDO3B, INPUT);
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
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderInc, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), encoderDec, FALLING);
}

void SPISend(std::bitset<16> command) {
  digitalWrite(CS, LOW);
  
  for(int i = 0; i < 16; i++) {
    digitalWrite(SDI, command[i]); // Write bit of command to SDI
    DELAY15NS();
    pulse(SCLK); // Tell ADC to read bit from SDI
  }

  digitalWrite(CS, HIGH);
}

void initADCRegisters() {
  while(digitalRead(READY_STROBE) != LOW); // Wait for READY to go low

  //     Op-Code(4) Register(4)    Value(8) Field Shift -> 16-bit command line
  SPISend(SET_BIT | PROTOCOL_CFG | 0b011 << SDO_PROTOCOL);    // CRT-DDR
  SPISend(SET_BIT | BUS_WIDTH    | 0b11  << SDO_WIDTH);       // Quad data width
  SPISend(SET_BIT | CRT_CFG      | 0b01  << CRT_CLK_SELECT);  // Set to fastest internal clock (15 ns period)

  digitalWrite(CONVST, LOW);
}



unsigned int read4bit(int SDO0, int SDO1, int SDO2, int SDO3) {
  return (digitalRead(SDO0) == HIGH) |
        ((digitalRead(SDO1) == HIGH) << 1) |
        ((digitalRead(SDO2) == HIGH) << 2) |
        ((digitalRead(SDO3) == HIGH) << 3);
}

void CRTRead(Buffer &CH1, Buffer &CH2) {
  pulse(CONVST); // Tell ADC to capture sample
  while(digitalRead(READY_STROBE) != HIGH); // Wait for READY to go HIGH
  digitalWrite(CS, LOW); // Switch to STROBE output

  bool nextEdge = HIGH;
  uint16_t read1 = 0, read2 = 0;
  for(int i = 0; i < 4; i++) {
    while(digitalRead(READY_STROBE) != nextEdge); // Wait for next edge of STROBE
    nextEdge = (nextEdge == HIGH) ? LOW : HIGH; // Invert nextEdge

    // Read in 4 bits for each channel
    read1 |= read4bit(SDO0A, SDO1A, SDO2A, SDO3A) << (4 * i);
    read2 |= read4bit(SDO0B, SDO1B, SDO2B, SDO3B) << (4 * i);
  }

  digitalWrite(CS, HIGH); // Done communicating
  CH1.insert(read1);
  CH2.insert(read2);
}

// Return if bool was changed
bool updateBool(bool &dst, bool src) {
  if(dst == src) return false;
  dst = src;
  return true;
}

bool updateTrigger(Trigger &trigger) {
  bool change = trigger.changed ||
    updateBool(trigger.enable, digitalRead(TRIGGER_ENABLE) == LOW) ||
    updateBool(trigger.decrease, digitalRead(TRIGGER_DECREASE) == LOW) ||
    updateBool(trigger.CH2, digitalRead(TRIGGER_CH2));
  
  return out;
}

// Ensures int stays between min and max
void updateInt(int &dst, int min, int max, int change) {
  dst += change;
  if(dst > max) dst = max;
  if(dst < min) dst = min;
}

// uses encoderChange count
bool updateEncoder(Display &display, Trigger &trigger) {
  if(encoderChange == 0) return false;

  // Read switches to determine what to change
  bool CH2 = digitalRead(ENCODER_CH, INPUT_PULLUP) == LOW,
       shift = digitalRead(ENCODER_SCALE_SHIFT, INPUT_PULLUP) == LOW,
       timeTrigger = digitalRead(ENCODER_TIME_TRIGGER, INPUT_PULLUP) == LOW
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