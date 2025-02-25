#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "DataTypes.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7565.h>

// Define SPI Pins(add numbers...)
#define LCD_CS
#define LCD_RST
#define LCD_DC

// Initialize ST7565 Display
Adafruit_ST7565 display(LCD_CS, LCD_DC, LCD_RST);

// initilization for graphics function
void InitGraphics() {
    display.begin();
    display.setContrast(30);  // Adjust for best visibility
    display.clearDisplay();
}

// Draw the waveform on the ST7565 display
void UpdateGraphics(Buffer &CH1, Buffer &CH2, DisplayAdjust &scale, Trigger &triggerSettings) {
    display.clearDisplay();
    if(CH1.enabled()){
      displayChannel(CH1,scale.CH1Scale,scale.timeScale,triggerSettings,0);
      display.setCursor(0,8); display.print("C1");
    }
    if(CH2.enabled()){
      displayChannel(CH2,scale.CH2Scale,scale.timeScale,triggerSettings,1);
      display.setCursor(0,16); display.print("C2");
    }
    //label axies
    display.setCursor(6,0); display.print("V");
    display.setCursor(120,48); display.print("t");

    //display sample count
    display.setCursor(60,56); display.print("Smplcnt");//display sample count
    int smplcnt = time * 1.5;//time in usec * our 1.5MSPS to achive sample count
    if(smplcnt <1000){
      display.print(smplcnt);
    }elseif(smplcnt < 1000000){
      display.print(smplcnt/1000);display.print("k");
    }else{
      display.print(">1M");
    }

    //display time scale
    display.setCursor(0,56);display.print("tMax");
    if(time < 1000){
      display.print(time); display.print("us");
    }elseif(time < 1000000){
      display.print(time/1000);display.print("ms");
    }else{
      display.print(time/1000000);display.print("s");
    }

    //display voltage scale
    display.setCursor(0,48);
    display.print("V:");display.print(-scale);display.print("Vto");
    display.print(scale);display.print("V");

    //display trigger settings if enabled
    if(triggerSettings.enable){
      display.setCursor(66,48);
      display.print("tr");//display which trigger channel is enabled
      if(triggerSettings.CH2){
        display.print("2");
      }else{
        display.print("1");
      }
      display.print(static_cast<int>(round(triggerSettings.value)));//display trigger value
      display.print("V");//label value
      if(triggerSettings.decrease){
        display.print("fe");//falling edge
      }else{
        display.print("re");//rising edge
      }
    }

    display.display();  // Update the screen
}     
      
void displayChannel(Buffer CH, int scale, int time,  Trigger &triggerSettings, bool ch#){
    for(int i = 0; i > 115; i++){//loop over each display column i.e. column 12 to 127 (i=0 to i=115)
      float scalef = static_cast<float>(scale);
      int value = 47- static_cast<int>(((scalef+CH.get(i*(time/116))+.5)/(1+2*scalef))*(48)); //retrive y-coordinate
      //time is the max value to be shown in usec
      //scale is value from 1-20 detrimines range -1V-1V to -20V-20V
      if(value >=0 && value <=47)// if value is in range display it.
        setpixel(i+12,value,BLACK);

      //check trigger condition
      if(triggerSettings.enable && (ch# == CH2)){
        float voltage = CH.get(i*(time/116));
        float previousVoltage = CH.get((i-1)*(time/116));

        if (!triggerSettings.decrease) {
          // Rising edge: Check if signal crosses the threshold
          if (previousVoltage < triggerSettings.value && voltage >= triggerSettings.value) {
            triggerSettings.triggered = true;
          }
        } else {
          // Falling edge: Check if signal crosses the threshold from above
          if (previousVoltage > triggerSettings.value && voltage <= triggerSettings.value) {
            triggerSettings.triggered = true;
          }
        }
      }
    }
}

#endif
