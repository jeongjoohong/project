#define VOLUME        A0

#define MILLISECONDS  1000L
#define MICROSECONDS  1000000L

#define BUFFER_SIZE   18

const bool isMicrosecond  = true;
const float samplingFrequency = 60;
unsigned long previousRead;

float inputs[BUFFER_SIZE] = {};
float sincF1[BUFFER_SIZE] = {};
float sincF2[BUFFER_SIZE] = {};
int inputsLength = sizeof(inputs) / sizeof(float);
int sincF1Length = sizeof(sincF1) / sizeof(float);
int sincF2Length = sizeof(sincF2) / sizeof(float);

void setup() {
  Serial.begin(115200);

  float bandwidth = 1;
  float targetFreq1 = 7;
  float targetFreq2 = 10;
  sincFilter(sincF1, BUFFER_SIZE, samplingFrequency, bandwidth, targetFreq1);
  sincFilter(sincF2, BUFFER_SIZE, samplingFrequency, bandwidth, targetFreq2);
}

void loop() {
  unsigned long currentTimestamp = isMicrosecond ? micros() : millis();
  if (currentTimestamp - previousRead >= (isMicrosecond ? MICROSECONDS : MILLISECONDS) / samplingFrequency) {
    previousRead = currentTimestamp;
    int input = analogRead(VOLUME);
    float sampling = ((1024 * 0.5 - 1) - input) / 1024;
    Serial.println(sampling);

    for (int i = 0; i < (sizeof(inputs) / sizeof(float)) - 1; i++) { 
      inputs[i] = inputs[i + 1];
    }
    inputs[(sizeof(inputs) / sizeof(float)) - 1] = sampling;

    bool isSame = true;
    int convF1Length = isSame ? inputsLength : inputsLength + sincF1Length - 1 ;
    float convF1[convF1Length] = {};
    int convF2Length = isSame ? inputsLength : inputsLength + sincF2Length - 1 ;
    float convF2[convF2Length] = {};
    
    convolution(inputs, inputsLength, sincF1, sincF1Length, convF1, isSame);
    convolution(inputs, inputsLength, sincF2, sincF2Length, convF2, isSame);
  }
}

void sincFilter(float* vector, int vectorSize, float samplingRate, float bandWidth, float shiftFrequency) {
  for (int i = 0; i < vectorSize; i++) {
    float timeVector = i / samplingRate;
    float x = bandWidth * (timeVector - (vectorSize / samplingRate * 0.5));
    float sinc;
    if (x == 0) {
      sinc = 1;
    } else {
      sinc = sin(x * PI) / (x * PI);
    }
    vector[i] = sinc * cos(2 * PI * shiftFrequency * timeVector);
  }
  
  float summation = 0;
  for (int i = 0; i < vectorSize; i++) {
    summation = summation + pow(vector[i], 2);
  }
  for (int i = 0; i < vectorSize; i++) {
    vector[i] = vector[i] / summation;
  }
}

void convolution(float *u, int uLength, float *v, int vLength, float *w, bool isSame) {
  float tmpArr[uLength + vLength - 1] = {};
  for (int i = 0; i < uLength + vLength - 1; i++) {
    int index = i;
    float tmpVal = 0;
    for (int j = 0; j < vLength; j++) {
      if (index >= 0 && index < uLength) {
        tmpVal += u[index] * v[j];
      }
      index--;
      tmpArr[i] = tmpVal;
    }
  }
  if (isSame) {
    int start = ceil((vLength - 1) / 2.0);
    for (int i = 0; i < uLength; i++) {
      w[i] = tmpArr[start+i]; 
    }
  } else {
    for (int i = 0; i < uLength + vLength - 1; i++) {
      w[i] = tmpArr[i]; 
    }
  }
}
