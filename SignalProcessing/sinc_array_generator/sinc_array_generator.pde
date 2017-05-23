int channel = 30;

int bufferSize = 60;
float samplingRate = 180;
float updatingRate = 1;
float bandWidth = 1;

void setup() {
  float[][] signals = sincSignals(channel, bufferSize, samplingRate, bandWidth);
  println("const uint8_t bufferSize" + "\t\t\t\t\t\t\t\t" + "= " + bufferSize + ";");
  println("const float samplingFrequency " + "\t\t\t\t\t" + "= " + int(samplingRate) + ";");
  println("const float updatingFrequency " + "\t\t\t\t\t" + "= " + int(updatingRate) + ";");
  println("const float bandwidth" + "\t\t\t\t\t\t\t\t\t\t" + "= " + int(bandWidth) + ";");
  for (int i = 0; i < channel-1; i++) {
    print("const PROGMEM float sinc_" + nf(i + 1, 2) + "[bufferSize] = {");
    
    for (int j = 0; j < bufferSize; j++) {
      print(signals[i][j]);
      if (j != bufferSize - 1) {
        print(", ");
      }
    }
    print("};");
    print("\n");
  }
}

float[][] sincSignals(int channel, int bufferSize, float samplingRate, float bandWidth) {
  float vector[][] = new float[channel][bufferSize];
  for (int i = 0; i < channel; i++) {
    for (int j = 0; j < bufferSize; j++) {
      float timeVector = j / samplingRate;
      float x = bandWidth * (timeVector - (bufferSize / samplingRate * 0.5));
      float sinc;
      if (x == 0) {
        sinc = 1;
      } else {
        sinc = sin(x * PI) / (x * PI);
      }
      vector[i][j] = sinc * cos(2 * PI * float(i + 1) * timeVector);
    }
    
    float summation = 0;
    for (int j = 0; j < bufferSize; j++) {
      summation = summation + pow(vector[i][j], 2);
    }
    for (int j = 0; j < bufferSize; j++) {
      vector[i][j] = vector[i][j] / summation;
    }
  }
  return vector;
}