#include "DataTypes.h"
#include "InputAcquisition.h"

void initGraphics() {}
void updateGraphics(Buffer &CH1, Buffer &CH2, DisplayAdjust &scale, Trigger &triggerSettings) {}

Buffer CH1, CH2;
DisplayAdjust display;
Trigger trigger;

// Variables to track the frame timing
unsigned long lastUpdate = 0;  // Store time of the last frame update
const unsigned long frameInterval = 50;  // Time interval between frames (50 ms = 20 FPS)

void setup() {
  initInput();
  initGraphics();
  Serial.begin(115200);
  Serial.println("Setup complete");
}

void loop() {
  if(acquireInput(CH1, CH2, display, trigger)) {
    Serial.println("Channel 1");
    Serial.println(display.CH1Scale);
    Serial.println(display.CH1Shift);

    Serial.println("Channel 2");
    Serial.println(display.CH2Scale);
    Serial.println(display.CH2Shift);

    Serial.println("Time, Trigger");
    Serial.println(display.timeScale);
    Serial.println(trigger.val);

    Serial.println("Enables: 1, 2");
    Serial.println(CH1.enabled());
    Serial.println(CH2.enabled());

    Serial.println("Trigger: Enable, decrease, CH2");
    Serial.println(trigger.enable);
    Serial.println(trigger.decrease);
    Serial.println(trigger.CH2);
  }
  /*
    if (millis() - lastUpdate >= frameInterval) {
      updateGraphics(CH1, CH2, display, trigger);
      lastUpdate = millis();
    }
    */
}
