#include "DataTypes.h"
#include "InputAcquisition.h"
#include "Graphics.h"

Buffer CH1, CH2;
DisplayAdjust display;
Trigger trigger;

void setup() {
  initInput();
  initGraphics();
}

void loop() {
  if(acquireInput(CH1, CH2, display, trigger))
    updateGraphics(CH1, CH2, display, trigger);
}