#ifndef OneEuroFilter_h
#define OneEuroFilter_h

#include "Arduino.h"
#include "LowPassFilter.h"

class OneEuroFilter {
  public:
    OneEuroFilter(double freq);
    OneEuroFilter(double freq, double mincutoff);
    OneEuroFilter(double freq, double mincutoff, double beta_);
    OneEuroFilter(double freq, double mincutoff, double beta_, double dcutoff);
    double filter(double value);
    double filter(double value, double timestamp);
  protected:
    double freq ;
    double mincutoff ;
    double beta_ ;
    double dcutoff ;
    LowPassFilter *x ;
    LowPassFilter *dx ;
    double lasttime ;
    double UndefinedTime = -1;
    void init(double freq, double mincutoff, double beta_, double dcutoff);
    double alpha(double cutoff);
    void setFrequency(double f);
    void setMinCutoff(double mc);
    void setBeta(double b);
    void setDerivateCutoff(double dc);
};

#endif

