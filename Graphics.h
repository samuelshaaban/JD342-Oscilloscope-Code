#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "DataTypes.h"
#include <Arduino.h>
#include <U8g2lib.h>

// Define SPI Pins(adjust numbers...)
#define LCD_CLK 18  //Clock
#define LCD_DATA 23 //Data
#define LCD_RST 4 //Reset
#define LCD_DC 2 //Data/Command
#define LCD_CS 5 //chip select

// Initialize ST7565 Display
U8G2_ST7565_NHD_C12864_F_4W_SW_SPI u8g2(U8G2_R0, LCD_CLK, LCD_DATA, LCD_CS, LCD_DC, LCD_RST);

// initilization for graphics function
void initGraphics() {
    SPI.begin(LCD_SCK, -1, LCD_DATA, LCD_CS);  // Custom SPI pin mapping
    u8g2.begin();
    u8g2.setFont(u8g2_font_5x8_tf);//set font size to 5x8 (6x8 including spaces)
}

void displayChannel(Buffer CH, int scale, int shift, int time,  Trigger &triggerSettings, bool chNum);

// Draw the waveform on the ST7565 display
void updateGraphics(Buffer &CH1, Buffer &CH2, DisplayAdjust &scale, Trigger &triggerSettings) {
    u8g2.clearBuffer();
    if(CH1.enabled()){
      displayChannel(CH1,scale.CH1Scale,scale.CH1Shift,scale.timeScale,triggerSettings,0);
      u8g2.drawStr(0, 8, "C1");
      //display voltage scale
      u8g2.setCursor(0,48);
      u8g2.print("V1:");u8g2.print(scale.CH1Scale/1000);//CH1 scale
    }
    if(CH2.enabled()){
      displayChannel(CH2,scale.CH2Scale,scale.CH2Shift,scale.timeScale,triggerSettings,1);
      u8g2.drawStr(0, 16, "C2");
      //display voltage scale
      u8g2.setCursor(30,48);
      u8g2.print("V2:");u8g2.print(scale.CH2Scale/1000);//CH2 scale
    }
    //label axies
    u8g2.drawStr(6, 0, "V");
    u8g2.drawStr(120, 48, "t");

    //display sample count
    int time = scale.timeScale;
    u8g2.drawStr(60,56,"Smplcnt");//display sample count
    u8g2.setCursor(102,56);
    int smplcnt = static_cast<int>(time * 1.5);//time in usec * our 1.5MSPS to achive sample count
    if(smplcnt <1000){
      u8g2.print(smplcnt);
    }else if(smplcnt < 1000000){
      u8g2.print(smplcnt/1000);u8g2.print("k");
    }else{
      u8g2.print(">1M");
    }

    //display time scale
    u8g2.setCursor(0,56);u8g2.print("tMax");
    if(time < 1000){
      u8g2.print(time); u8g2.print("us");
    }else{
      u8g2.print(time/1000);u8g2.print("ms");
    }

    //display trigger settings if enabled
    if(triggerSettings.enable){
      u8g2.setCursor(66,48);
      u8g2.print("tr");//display which trigger channel is enabled
      if(triggerSettings.CH2){
        u8g2.print("2");
      }else{
        u8g2.print("1");
      }
      u8g2.print(static_cast<int>(round(triggerSettings.val)));//display trigger value
      u8g2.print("V");//label value
      if(triggerSettings.decrease){
        u8g2.print("fe");//falling edge
      }else{
        u8g2.print("re");//rising edge
      }
    }

    u8g2.sendBuffer();  // Update the screen
}  
      
void displayChannel(Buffer CH, int scale, int shift, int time,  Trigger &triggerSettings, bool chNum){
  if(triggerSettings.triggered == false){
    for(int i = 0; i < 115; i++){//loop over each display column i.e. column 12 to 127 (i=0 to i=115)
      float scalef = (scale)/1000.0;
      float shiftf = (shift)/1000.0;
      int value = 23 - static_cast<int>((CH.get(i*(time/116))/scalef)*(24)); //retrive y-coordinate
      value += 23 - static_cast<int>((shiftf)/(scalef)*(24));//performs shift 
      //time is the max value to be shown in usec
      //scale is value from 1-20k detrimines range -1mV to 1mV to -20V to 20V
      //shift is value from -20k-20k shifts screen display range anywhere between down 20V to up 20V
      if(value >=0 && value <=47)// if value is in range display it.
        u8g2.drawPixel(i+12,value);

      //check trigger condition
      if(triggerSettings.enable && (chNum == triggerSettings.CH2)){
        float voltage = CH.get(i*(time/116.0))*1000.0;
        float previousVoltage = CH.get((i-1)*(time/116.0))*1000.0;

        if (!triggerSettings.decrease) {
          // Rising edge: Check if signal crosses the threshold
          if (previousVoltage < triggerSettings.val && voltage >= triggerSettings.val) {
            triggerSettings.triggered = true;
          }
        } else {
          // Falling edge: Check if signal crosses the threshold from above
          if (previousVoltage > triggerSettings.val && voltage <= triggerSettings.val) {
            triggerSettings.triggered = true;
          }
        }
      }
    }
  }
}

#endif

