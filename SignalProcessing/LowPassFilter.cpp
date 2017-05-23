#include "LowPassFilter.h"

LowPassFilter::LowPassFilter(double alpha) {
  init(alpha, 0);
}

LowPassFilter::LowPassFilter(double alpha, double initval) {
  init(alpha, initval);
}

void LowPassFilter::init(double alpha, double initval) {
  y = s = initval;
  setAlpha(alpha);
  initialized = false;
}

void LowPassFilter::setAlpha(double alpha) {
  if (alpha <= 0.0 || alpha > 1.0) {
      Serial.println("alpha should be in (0.0, 1.0)");
  }
  a = alpha;
}

double LowPassFilter::filter(double value) {
  double result ;
  if (initialized) {
    result = a * value + (1.0 - a) * s;
  } else {
    result = value;
    initialized = true;
  }
  y = value;
  s = result;
  return result;
}

double LowPassFilter::filterWithAlpha(double value, double alpha) {
  setAlpha(alpha);
  return filter(value);
}

bool LowPassFilter::hasLastRawValue(void) {
  return initialized;
}

double LowPassFilter::lastRawValue(void) {
  return y;
}

