#ifndef LowPassFilter_h
#define LowPassFilter_h

#include "Arduino.h"

class LowPassFilter {
  public:
    LowPassFilter(double alpha);
    LowPassFilter(double alpha, double initval);
    double filter(double value);
    double filterWithAlpha(double value, double alpha);
    bool hasLastRawValue(void);
    double lastRawValue(void);
  protected:
    double y, a, s;
    bool initialized;
    void init(double alpha, double initval);
    void setAlpha(double alpha);
};

#endif

