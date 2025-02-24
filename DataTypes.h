#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#define BUFFER_LEN 10000
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

} CH1, CH2;

struct DisplayAdjust {
  int timeScale,
      CH1Scale, CH2Scale,
      CH1Shift, CH2Shift;
};

struct Trigger {
  bool enable, decrease, CH2; // decrease=false means increase, CH2 = false means CH1
  float val;
};

#endif