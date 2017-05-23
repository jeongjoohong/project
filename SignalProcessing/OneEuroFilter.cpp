#include "OneEuroFilter.h"

OneEuroFilter::OneEuroFilter(double freq) {
  init(freq, 1.0, 0.0, 1.0);
}

OneEuroFilter::OneEuroFilter(double freq, double mincutoff) {
  init(freq, mincutoff, 0.0, 1.0);
}

OneEuroFilter::OneEuroFilter(double freq, double mincutoff, double beta_) {
  init(freq, mincutoff, beta_, 1.0);
}

OneEuroFilter::OneEuroFilter(double freq, double mincutoff, double beta_, double dcutoff) {
  init(freq, mincutoff, beta_, dcutoff);
}

void OneEuroFilter::init(double freq, double mincutoff, double beta_, double dcutoff) {
  setFrequency(freq) ;
  setMinCutoff(mincutoff) ;
  setBeta(beta_) ;
  setDerivateCutoff(dcutoff) ;
  x = new LowPassFilter(alpha(mincutoff)) ;
  dx = new LowPassFilter(alpha(dcutoff)) ;
  lasttime = UndefinedTime ;
}

double OneEuroFilter::alpha(double cutoff) {
  double te = 1.0 / freq ;
  double tau = 1.0 / (2 * M_PI * cutoff) ;
  return 1.0 / (1.0 + tau / te) ;
}

void OneEuroFilter::setFrequency(double f) {
  if (f <= 0) {
    Serial.println("freq should be >0");
  }
  freq = f;
}

void OneEuroFilter::setMinCutoff(double mc) {
  if (mc <= 0) {
    Serial.println("mincutoff should be >0");
  }
  mincutoff = mc ;
}

void OneEuroFilter::setBeta(double b) {
  beta_ = b ;
}

void OneEuroFilter::setDerivateCutoff(double dc) {
  if (dc <= 0) {
    Serial.println("dcutoff should be >0");
  }
  dcutoff = dc;
}

double OneEuroFilter::filter(double value) {
  return filter(value, UndefinedTime);
}

double OneEuroFilter::filter(double value, double timestamp) {
  if (lasttime != UndefinedTime && timestamp != UndefinedTime) {
    freq = 1.0 / (timestamp - lasttime) ;
  }
  lasttime = timestamp ;
  double dvalue = x->hasLastRawValue() ? (value - x->lastRawValue()) * freq : 0.0; // FIXME: 0.0 or value?
  double edvalue = dx->filterWithAlpha(dvalue, alpha(dcutoff));
  double cutoff = mincutoff + beta_ * fabs(edvalue);
  return x->filterWithAlpha(value, alpha(cutoff));
}

