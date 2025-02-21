#define BUFFER_LEN 10000
class Buffer {
  private:
    bool enable;

    int time[BUFFER_LEN]; // microseconds
    float volt[BUFFER_LEN];
    unsigned int first;

  public:
    Buffer(): arr(), first(0) {}
    void setEnable(bool enable);
    void insert(uint16_t adcOut);

    bool enabled();
    float get(int us);

} CH1, CH2;

struct DisplayAdjust {
  int timeScale,
      CH1Scale, CH2Scale,
      CH1Shift, CH2Shift;
} displayAdjust;

struct Trigger {
  bool enable, decrease, CH2; // decrease=false means increase, CH2 = false means CH1
  float val;
} trigger;


void initInput();
void acquireInput(Buffer CH1, Buffer CH2, DisplayAdjust, Trigger);
