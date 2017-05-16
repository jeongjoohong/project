int channel = 30;

int bufferSize = 65;
float samplingRate = 100;
float bandWidth = 1;

void setup() {
  float[][] signals = sincSignals(channel, bufferSize, samplingRate, bandWidth);
  
  println("const float bandwidth = " + bandWidth + ";");
  for (int i = 0; i < channel-1; i++) {
    print("const PROGMEM float sinc_" + nf(i + 1, 2) + "[BUFFER_SIZE] = {");
    
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