import processing.serial.*;

int channel = 1;
boolean isFiltered = false;
boolean isFiltering = true;
int niddle = 10;
float minCutoff = 1.0f;  // decrease this to get rid of slow speed jitter
float beta = 0.007f;     // increase this to get rid of high speed lag

Serial port;
int inputSize;
int display;
int xPosition;
int[] signal;
int[][] signals;
boolean isUpdated;

SignalFilter filter;
float[][] filterings;

PrintWriter output;
boolean isRecording = false;
int startTime = 0;

void setup() {
  size(640, 360);
  String[] portList = Serial.list();
  int select = 0;
  for (int i = 0; i < portList.length; i++) {
    String[] match = match(portList[i], "tty.usb");
    if (match != null) {
      select = i;
      break;
    }
  }
  String portName = Serial.list()[select];
  port = new Serial(this, portName, 115200);
  port.bufferUntil('\n');

  inputSize = (isFiltered ? 2 : 1) * channel;
  display = width - niddle;
  xPosition = 0;
  signal = new int[inputSize];
  signals = new int[inputSize][display];

  if (isFiltering) {
    filterings = new float[channel][display];
    filter = new SignalFilter(this, channel);
    filter.setMinCutoff(minCutoff);
    filter.setBeta(beta);
  }
}

void draw() {
  if (isUpdated) {
    isUpdated = false;

    for (int i = 0; i < inputSize; i++) {
      if (xPosition < display) {
        signals[i][xPosition] = signal[i];
      } else {
        for (int j = 1; j < display; j++) {
          signals[i][j-1] = signals[i][j];
        }
        signals[i][display-1] = signal[i];
      }
    }

    if (isFiltering) {
      float[] raw = new float[channel];
      for (int i = 0; i < channel; i++) {
        raw[i] = float(signal[i]);
      }
      float[] filtering = filter.filterUnitArray(raw);
      for (int i = 0; i < channel; i++) {
        if (xPosition < display) {
          filterings[i][xPosition] = filtering[i];
        } else {
          for (int j = 1; j < display; j++) {
            filterings[i][j-1] = filterings[i][j];
          }
          filterings[i][display-1] = filtering[i];
        }
      }
    }

    background(55);
    for (int i = 0; i < inputSize; i++) {
      for (int j = 0; j < xPosition; j++) {
        color signalColor = color(0);
        switch (i % channel) {
          case 0: {
            if (isFiltered) {
              if (i / 2 == 0) {
                signalColor = color(120, map(j, 0, display, 0, 120), 0);
              } else {
                signalColor = color(255, map(j, 0, display, 0, 255), 0);
              }
            } else {
              signalColor = color(255, map(j, 0, display, 0, 255), 0);
            }
            break;
          }
          case 1: {
            if (isFiltered) {
              if (i / 2 == 0) {
                signalColor = color(0, 120, map(j, 0, display, 120, 0));
              } else {
                signalColor = color(0, 255, map(j, 0, display, 255, 0));
              }
            } else {
              signalColor = color(0, 255, map(j, 0, display, 255, 0));
            }
            break;
          }
        }
        stroke(signalColor);
        strokeWeight(2);
        point(j, map(signals[i][j], 0, 1023, 0, height));
      }        
    }
    
    if (isFiltering) {
      for (int i = 0; i < channel; i++) {
        for (int j = 0; j < xPosition; j++) {
          stroke(120);
          strokeWeight(2);
          point(j, map(filterings[i][j], 0, 1023, 0, height));
        }
      }
    }
    
    for (int i = 0; i < channel; i++) {
      color niddleColor = color(255);
        switch (i % channel) {
          case 0: {
            niddleColor = color(255, 200, 0);
            break;
          }
          case 1: {
            niddleColor = color(0, 200, 0);
            break;
          }
        }
        stroke(niddleColor);
        strokeWeight(5);
        line(display, map(signal[i], 0, 1023, 0, height), display + niddle, map(signal[i], 0, 1023, 0, height));
        strokeWeight(1);
        line(0, map(signal[i], 0, 1023, 0, height), width, map(signal[i], 0, 1023, 0, height));
    }

    if (xPosition < display) {
      xPosition++;
    }
  }
  
  if (isRecording) {
    textSize(20);
    fill(250, 10, 10);
    text("Rec", width - 60, 40);
  } else {
    textSize(16);
    fill(255, 255, 255);
    text("Press the spacebar to start recording.", width - 320, 40);
  }
}

void serialEvent(Serial serial) { 
  String stream = serial.readString();
  if(isRecording) {
    int elapseTime = millis() - startTime;
    int milliseconds = elapseTime % 1000;
    int seconds = elapseTime / 1000;
    int minutes = seconds / 60;
    int hours = minutes / 60;
    String timestamp = nf(minutes, 2) + ":" + nf(seconds, 2) + "." + nf(milliseconds, 4);
    output.print(timestamp + "\t" + stream);
  }
  String[] buffer = split(stream, '\t');
  if (inputSize <= buffer.length) {
    for (int i = 0; i < inputSize; i++) {
      signal[i] = int(float(trim(buffer[i])));
    }
    isUpdated = true;
  }
}

void keyReleased() {
  if (key == ' ') {
    if (isRecording) {
      println("close");
      output.flush();
      output.close();
      isRecording = !isRecording;
    } else {
      println("start");
      String filename = "data_" + year() + nf(month(), 2) + nf(day(), 2) + "_" + nf(hour(), 2) + nf(minute(), 2) + nf(second(), 2) + ".txt";
      output = createWriter(filename);
      startTime = millis();
      isRecording = !isRecording;
    }
  }
}

class LowPassFilter {
  double y, a, s;
  boolean initialized;
  void setAlpha(double alpha) throws Exception {
    if (alpha <= 0.0 || alpha > 1.0) {
      throw new Exception("alpha should be in (0.0, 1.0) and is now "+ alpha);
    }
    a = alpha;
  }
  
  LowPassFilter(double alpha) throws Exception {
    init(alpha, 0);
  }
  
  LowPassFilter(double alpha, double initval) throws Exception {
    init(alpha, initval);
  }
  
  void init(double alpha, double initval) throws Exception {
    y = s = initval;
    setAlpha(alpha);
    initialized = false;
  }

  double filter(double value) {
    double result;
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

  double filterWithAlpha(double value, double alpha) throws Exception {
    setAlpha(alpha);
    return filter(value);
  }
  
  boolean hasLastRawValue() {
    return initialized;
  }
  
  double lastRawValue() {
    return y;
  }
}

class OneEuroFilter {
  double freq;
  double mincutoff;
  double beta_;
  double dcutoff;
  LowPassFilter x;
  LowPassFilter dx;
  double lasttime;
  double UndefinedTime = -1;
  
  double alpha(double cutoff) {
    double te = 1.0 / freq;
    double tau = 1.0 / (2 * Math.PI * cutoff);
    return 1.0 / (1.0 + tau / te);
  }
  
  void setFrequency(double f) throws Exception {
    if (f <= 0) {
      throw new Exception("freq should be >0");
    }
    freq = f;
  }
  
  void setMinCutoff(double mc) throws Exception {
    if (mc <= 0) {
      throw new Exception("mincutoff should be >0");
    }
    mincutoff = mc;
  }
  
  void setBeta(double b) {
    beta_ = b;
  }
  
  void setDerivateCutoff(double dc) throws Exception {
    if (dc <= 0) {
      throw new Exception("dcutoff should be >0");
    }
    dcutoff = dc;
  }
  
  OneEuroFilter(double freq) throws Exception {
    init(freq, 1.0, 0.0, 1.0);
  }
  
  OneEuroFilter(double freq, double mincutoff) throws Exception {
    init(freq, mincutoff, 0.0, 1.0);
  }
  
  OneEuroFilter(double freq, double mincutoff, double beta_) throws Exception {
    init(freq, mincutoff, beta_, 1.0);
  }
  
  OneEuroFilter(double freq, double mincutoff, double beta_, double dcutoff) throws Exception {
    init(freq, mincutoff, beta_, dcutoff);
  }
  
  
  void init(double freq, double mincutoff, double beta_, double dcutoff) throws Exception {
    setFrequency(freq);
    setMinCutoff(mincutoff);
    setBeta(beta_);
    setDerivateCutoff(dcutoff);
    x = new LowPassFilter(alpha(mincutoff));
    dx = new LowPassFilter(alpha(dcutoff));
    lasttime = UndefinedTime;
  }
  
  double filter(double value) throws Exception {
    return filter(value, UndefinedTime);
  }
  
  double filter(double value, double timestamp) throws Exception {
    if (lasttime != UndefinedTime && timestamp != UndefinedTime) {
      freq = 1.0 / (timestamp - lasttime);
    }
    lasttime = timestamp;
    double dvalue = x.hasLastRawValue() ? (value - x.lastRawValue()) * freq : value; // FIXME: 0.0 or value?
    double edvalue = dx.filterWithAlpha(dvalue, alpha(dcutoff));
    double cutoff = mincutoff + beta_ * Math.abs(edvalue);
    return x.filterWithAlpha(value, alpha(cutoff));
  }
}

public class SignalFilter {
  double freq       = 120.0;
  double mincutoff  = 3.0;
  double beta_      = 0.007;
  double dcutoff    = 1.0;
  PApplet myParent;
  ArrayList<OneEuroFilter> channels;
  int size;
  
  SignalFilter(PApplet theParent) {
    myParent = theParent;
    try {
      init(1);
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
  
  SignalFilter(PApplet theParent, int size) {
    if (size <= 0) {
      System.out.println("Error in SignalFilter(): The number of channels cannot be " + size + ". The size should be at least 1");
    }
    myParent = theParent;
    try {
      init(size);
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
  
  PVector filterUnitVector(PVector noisyVector) {
    if (channels.size() < 3) {
      System.out.println("Error in filterUnit(): The number of channels cannot be " + size + ". You need 3 channels to filter a PVector (even if you only use the x and y values).");
    }
    float[] noisyValues = noisyVector.array();
    float[] filteredValues = new float[3];
    try {
      filteredValues = filterValues(noisyValues);
    } catch (Exception e) {
      e.printStackTrace();
    }
    PVector filteredVector = new PVector(0, 0, 0);
    try {
      filteredVector = toVector(filteredValues);
    } catch (Exception e) {
      e.printStackTrace();
    }
    return filteredVector;
  }
  
  float filterUnitFloat(float noisyFloat) {
    float[] noisyArray = new float[1];
    float[] filteredArray = new float[1];
    noisyArray[0] = noisyFloat;
    try {
      filteredArray = filterValues(noisyArray);
    } catch (Exception e) {
      e.printStackTrace();
    }
    float filteredFloat = filteredArray[0];
    return filteredFloat;
  }
  
  float[] filterUnitArray(float[] noisyArray) {
    int valueCount = noisyArray.length;
    float[] filteredArray = new float[valueCount];
    try {
      filteredArray = filterValues(noisyArray);
    } catch (Exception e) {
      e.printStackTrace();
    }
    return filteredArray;
  }
  
  PVector filterCoord2D(PVector noisyVector, float scaleX, float scaleY) {
    PVector unitVector = new PVector(0, 0);
    unitVector.x = noisyVector.x / scaleX;
    unitVector.y = noisyVector.y / scaleY;
    PVector filteredVector = filterUnitVector(unitVector);
    filteredVector.x = filteredVector.x * scaleX;
    filteredVector.y = filteredVector.y * scaleY;
    return filteredVector;    
  }
  
  PVector filterCoord2D(float coordX, float coordY, float scaleX, float scaleY) {    
    PVector unitVector = new PVector(0, 0);
    unitVector.x = coordX / scaleX;
    unitVector.y = coordY / scaleY;
    PVector filteredVector = filterUnitVector(unitVector);
    filteredVector.x = filteredVector.x * scaleX;
    filteredVector.y = filteredVector.y * scaleY;
    return filteredVector;
  }
  
  PVector filterCoord3D(PVector noisyVector, float scaleX, float scaleY, float scaleZ) {
    PVector unitVector = new PVector(0, 0, 0);
    unitVector.x = noisyVector.x / scaleX;
    unitVector.y = noisyVector.y / scaleY;
    unitVector.z = noisyVector.z / scaleZ;
    PVector filteredVector = filterUnitVector(unitVector);
    filteredVector.x = filteredVector.x * scaleX;
    filteredVector.y = filteredVector.y * scaleY;
    filteredVector.z = filteredVector.z * scaleZ;
    return filteredVector;
  }
  
  PVector filterCoord3D(float coordX, float coordY, float coordZ, float scaleX, float scaleY, float scaleZ) {
    PVector unitVector = new PVector(0, 0);
    unitVector.x = coordX / scaleX;
    unitVector.y = coordY / scaleY;
    unitVector.z = coordZ / scaleZ;
    PVector filteredVector = filterUnitVector(unitVector);
    filteredVector.x = filteredVector.x * scaleX;
    filteredVector.y = filteredVector.y * scaleY;
    filteredVector.z = filteredVector.z * scaleZ;
    return filteredVector;    
  }
  
  void setFrequency(float f) {
    freq = (double) f;        
    for (final OneEuroFilter filter : channels) {
      try {
        filter.setFrequency(freq);
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
  }
  
  void setMinCutoff(float mc) {  
    mincutoff = (double) mc;
    for (final OneEuroFilter filter : channels) {
      try {
        filter.setMinCutoff(mincutoff);
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
  }
  
  void setBeta(float b) {
    beta_ = (double) b;
    for (final OneEuroFilter filter : channels) {
      try {
        filter.setBeta(beta_);
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
  }
  
  void setDerivateCutoff(float dc) {
    dcutoff = (double) dc;
    for (final OneEuroFilter filter : channels) {
      try {
        filter.setDerivateCutoff(dcutoff);
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
  }
  
  void init(int channelCount) throws Exception {
    if (channelCount <= 0) {
      throw new Exception("Exception in init(): The number of channels cannot be " + channelCount + ". The size should be at least 1");
    }
    try {
      createChannels(channelCount);
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
  
  void createChannels(int channelCount) throws Exception {
    if (channelCount <= 0) {
      throw new Exception("Exception in createChannels(): The number of channels cannot be " + channelCount + ". The size should be at least 1");
    }
    channels = new ArrayList<OneEuroFilter>(channelCount);
    for (int i = 0; i < channelCount; i++) {
      try {
        channels.add(new OneEuroFilter(freq, mincutoff, beta_, dcutoff)); // Create a default filter for this channel
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
  }
  
  float[] filterValues(float[] noisyValues) throws Exception {
    int valueCount   = noisyValues.length;
    int channelCount = channels.size();
    if (valueCount != channelCount) {
      throw new Exception("Exception in filterValues(): The number of filtering channels ("+ channelCount +") must match the number of signals you want to filter (" + valueCount + ")");
    }
    float[] filteredValues = new float[valueCount];
    double timestamp = myParent.frameCount / freq;
    for (int i = 0; i < valueCount; i++) {
      OneEuroFilter f = channels.get(i);
      double value = (double) noisyValues[i];
      filteredValues[i] = (float) f.filter(value, timestamp);
    }
    return filteredValues;
  }
  
  PVector toVector(float[] array) throws Exception {
    int valueCount = array.length;
    if (valueCount > 3) {
      throw new Exception("Exception in toVector(): An array of length " + valueCount + " cannot be converted to PVector. The maximum number of values is 3");
    } else if (valueCount < 2) {
      throw new Exception("Exception in toVector(): An array of length " + valueCount + " cannot be converted to PVector. The minimum number of values is 2");
    }
    float[] a = array;
    float x = a[0];
    float y = a[1];
    float z = a[2];
    PVector v = new PVector(x, y, z);
    return v;
  }
}