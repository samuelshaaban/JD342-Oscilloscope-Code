#ifndef DATA_TYPES_H
#define DATA_TYPES_H

// Definitions

#define BUFFER_LEN 5000
class Buffer {
  private:
    bool enable, change;

    int time[BUFFER_LEN]; // microseconds
    float volt[BUFFER_LEN];

    unsigned int lastSample, lastPeak;

  public:
    Buffer();
    void setEnable(bool enable);
    void insert(uint16_t adcOut);

    bool enabled();
    bool changed();
    float get(int us); // Time since beginning of most recent wave

};

struct DisplayAdjust {
  int timeScale,           // Width of screen (1 - 10,000 us)
      CH1Scale, CH2Scale,  // Height of screen (1 - 40,000 mV)
      CH1Shift, CH2Shift;  // Vertical shift of screen (-20,000 - 20,000 mV)
  
  DisplayAdjust();
};

struct Trigger {
  bool enable, decrease, CH2; // decrease=false means increase, CH2 = false means CH1
  int val; // 0.1% of screen height, 0-1000

  Trigger();
};



// Implementations

Buffer::Buffer(): enable(false), change(false), time(), volt(), lastSample(0), lastPeak(0) {}
void Buffer::setEnable(bool enable) { this->enable = enable; }
bool Buffer::enabled() { return this->enable; }
bool Buffer::changed() { return this->change; }



void Buffer::insert(uint16_t adcOut) {
  lastSample = (lastSample + 1) % BUFFER_LEN;
  time[lastSample] = micros();
  volt[lastSample] = ((float)adcOut - 1.5) * 20;

  // Set change
}

float Buffer::get(int us) { 
  int curTime = time[lastSample];
  unsigned int i = lastSample + 3 * us; // 3 MSPS = 3 samples per us
  unsigned int usPer10Samples = (time[i] - curTime) * 10 / (i - lastSample); 

  // Correct with calculated sample rate
  int error = time[i] - curTime - us;
  i += error / usPer10Samples / 10; //

  // Go in correct direction until we cross the time we are aiming for
  bool tooBig = time[i] - curTime > us;
  while((time[i] - curTime > us) == tooBig) i++;

  return volt[i];
}

DisplayAdjust::DisplayAdjust():
  timeScale(1), CH1Scale(1), CH2Scale(1), CH1Shift(0), CH2Shift(0) {}

Trigger::Trigger(): enable(false), decrease(false), CH2(false), val(0) {}

#endif
