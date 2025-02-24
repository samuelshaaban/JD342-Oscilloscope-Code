#ifndef DATA_TYPES_H
#define DATA_TYPES_H

// Definitions

#define BUFFER_LEN 6500
class Buffer {
  private:
    bool enable;

    int time[BUFFER_LEN]; // microseconds
    float volt[BUFFER_LEN];

    unsigned int lastSample, lastPeak;

  public:
    Buffer();
    void setEnable(bool enable);
    void insert(uint16_t adcOut);

    bool enabled();
    float get(int us); // Time since beginning of most recent wave

};

struct DisplayAdjust {
  unsigned int timeScale,           // Width of screen (1 - 10,000 us)
               CH1Scale, CH2Scale;  // Height of screen (1 - 40,000 mV)
  int CH1Shift, CH2Shift;           // Vertical shift of screen (-20,000 - 20,000 mV)
  
  DisplayAdjust();
};

struct Trigger {
  bool enable, decrease, CH2; // decrease=false means increase, CH2 = false means CH1
  float val;

  Trigger();
};



// Implementations

Buffer::Buffer(): enable(false), time(), volt(), lastSample(0), lastPeak(0) {}
void Buffer::setEnable(bool enable) { this->enable = enable; }
bool Buffer::enabled() { return this->enable; }

void Buffer::insert(uint16_t adcOut) {}
float Buffer::get(int us) {}

DisplayAdjust::DisplayAdjust():
  timeScale(1), CH1Scale(1), CH2Scale(1), CH1Shift(0), CH2Shift(0) {}

Trigger::Trigger(): enable(false), decrease(false), CH2(false), val(0) {}

#endif