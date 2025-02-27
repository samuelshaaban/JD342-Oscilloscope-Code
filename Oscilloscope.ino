#include "DataTypes.h"
#include "InputAcquisition.h"
#include "Graphics.h"

Buffer CH1, CH2;
DisplayAdjust display;
Trigger trigger;

// Variables to track the frame timing
unsigned long lastUpdate = 0;  // Store time of the last frame update
const unsigned long frameInterval = 50;  // Time interval between frames (50 ms = 20 FPS)

void setup() {
  initInput();
  initGraphics();
}

void loop() {
  if(acquireInput(CH1, CH2, display, trigger))
    if (millis() - lastUpdate >= frameInterval) {
      updateGraphics(CH1, CH2, display, trigger);
      lastUpdate = millis();
    }
}
