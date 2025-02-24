#ifndef INPUT_ACQUISITION_H
#define INPUT_ACQUISITION_H

#include "DataTypes.h"

// ADC commands
#define SET_BIT 0b0101<<12

// ADC registers
#define PROTOCOL_CFG  2<<8
#define BUS_WIDTH     3<<8
#define DATA_AVG_CFG  6<<8

// ADC register field bit shifts
#define SDO_PROTOCOL  4
#define SDO_WIDTH     0
#define EN_DATA_AVG   0


// ADC control pins
#define CS 1
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

// Rotary encoders
#define TRIGGER_VAL_A 19
#define TRIGGER_VAL_B 20
#define SCALE_TIME_A 21
#define SCALE_TIME_B 22
#define SCALE_CH1_A 23
#define SCALE_CH1_B 24
#define SCALE_CH2_A 25
#define SCALE_CH2_B 26
#define SHIFT_CH1_A 27
#define SHIFT_CH1_B 28
#define SHIFT_CH2_A 29
#define SHIFT_CH2_B 30


void initADCPins() {
  pinMode(CS, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(SDI, OUTPUT);
  pinMode(CONVST, OUTPUT);
  digitalWrite(CS, HIGH);
  digitalWrite(SCLK, LOW);
  digitalWrite(SDI, LOW);
  digitalWrite(CONVST, HIGH);

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
  
  pinMode(TRIGGER_VAL_A, INPUT_PULLUP);
  pinMode(TRIGGER_VAL_B, INPUT_PULLUP);
  pinMode(SCALE_TIME_A, INPUT_PULLUP);
  pinMode(SCALE_TIME_B, INPUT_PULLUP);
  pinMode(SCALE_CH1_A, INPUT_PULLUP);
  pinMode(SCALE_CH1_B, INPUT_PULLUP);
  pinMode(SCALE_CH2_A, INPUT_PULLUP);
  pinMode(SCALE_CH2_B, INPUT_PULLUP);
  pinMode(SHIFT_CH1_A, INPUT_PULLUP);
  pinMode(SHIFT_CH1_B, INPUT_PULLUP);
  pinMode(SHIFT_CH2_A, INPUT_PULLUP);
  pinMode(SHIFT_CH2_B, INPUT_PULLUP);
}

void SPISend(uint16_t command) {}

void initADCRegisters() {
  while(digitalRead(READY) != LOW); // Wait for READY to go low

  SPISend(SET_BIT | PROTOCOL_CFG | 0b011<<SDO_PROTOCOL); // CRT-DDR
  SPISend(SET_BIT | BUS_WIDTH | 0b11<<SDO_WIDTH); // Quad data width
  SPISend(SET_BIT | DATA_AVG_CFG | 0b10<<EN_DATA_AVG); // Average every 2 readings
}

void initInput() {
  initADCPins();
  initControls();

  initADCRegisters();
}

bool acquireInput(Buffer &CH1, Buffer &CH2, DisplayAdjust &display, Trigger &trigger) {
  return true;
}

#endif