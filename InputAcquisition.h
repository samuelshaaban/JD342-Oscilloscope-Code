#ifndef INPUT_ACQUISITION_H
#define INPUT_ACQUISITION_H

#include "DataTypes.h"

void initInput();
void acquireInput(Buffer CH1, Buffer CH2, DisplayAdjust display, Trigger trigger);

#endif