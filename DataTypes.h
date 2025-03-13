#ifndef DATA_TYPES_H
#define DATA_TYPES_H

// Definitions

#define BUFFER_LEN 5000
class Buffer {
  private:
    bool enable, change, increasing;

    int time[BUFFER_LEN]; // microseconds
    float volt[BUFFER_LEN];

    unsigned int min, max, lastSample, lastPeriod;

  public:
    Buffer();
    void setEnable(bool enable);
    void insert(uint16_t adcOut);

    bool enabled();
    bool changed(); // Disables on check
    float get(int us); // Time since beginning of most recent wave

};

struct DisplayAdjust {
  int timeScale,           // Width of screen (1 - 10,000 us)
      CH1Scale, CH2Scale,  // Half of height of screen (1 - 20,000 mV)
      CH1Shift, CH2Shift;  // Vertical shift of screen (-20,000 - 20,000 mV)
  
  DisplayAdjust();
};

struct Trigger {
  bool enable, decrease, CH2, // decrease=false means increase, CH2 = false means CH1
       triggered;
  int val; // Voltage setting of trigger (-20,000 - 20,000 mV)

  Trigger();
};



// Implementations

Buffer::Buffer(): enable(false), change(false), increasing(false),
  time(), volt(), lastSample(0), lastPeriod(0) {}

void Buffer::setEnable(bool enable) { this->enable = enable; }
bool Buffer::enabled() { return this->enable; }
bool Buffer::changed() { return this->change; }

void Buffer::insert(uint16_t adcOut) {
  unsigned int lastLastSample = lastSample;
  lastSample = (lastSample + 1) % BUFFER_LEN;
  time[lastSample] = micros();
  volt[lastSample] = ((float)adcOut - 1.5) * 20;

  // Update min/max if derivative flips
  bool increase = volt[lastSample] - volt[lastLastSample] > 0;
  if(increase != increasing) {
    if(increasing) max = lastSample;
    else min = lastSample;

    increasing = increase;
  }

  // Update lastPeriod if a 3/4 period has passed and crossed 0
  if(time[lastSample] - time[lastPeriod] > abs(time[min] - time[max]) * 1.5) { // 3/4 of a period has passed
    if(volt[lastSample] > 0 != volt[lastLastSample] > 0) { // If last sample crossed 0
      lastPeriod = lastSample;
      change = true;
    }
  }
}

float Buffer::get(int us) {
  for(unsigned int i = 0; i < BUFFER_LEN; i++){
    unsigned int index = (lastSample + i) % BUFFER_LEN;
    if(time[index] >= us){
      return volt[index];
    }
  }
  return 0.0f;
}



DisplayAdjust::DisplayAdjust():
  timeScale(1), CH1Scale(1), CH2Scale(1), CH1Shift(0), CH2Shift(0) {}

Trigger::Trigger(): enable(false), decrease(false), CH2(false), triggered(false), val(0) {}

#endif
