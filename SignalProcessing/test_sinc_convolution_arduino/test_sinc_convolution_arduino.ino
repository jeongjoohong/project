#define VOLUME        A0

#define MILLISECONDS  1000L
#define MICROSECONDS  1000000L

#define BUFFER_SIZE   25

const bool isMicrosecond  = true;
const float samplingFrequency = 60;
unsigned long lastTimestamp;

const unsigned int inputsLength = BUFFER_SIZE;
const unsigned int sincF1Length = BUFFER_SIZE;
const unsigned int sincF2Length = BUFFER_SIZE;
volatile float inputs[inputsLength] = {};
volatile float sincF1[sincF1Length] = {};
volatile float sincF2[sincF2Length] = {};

void setup() {
  Serial.begin(115200);

  const float bandwidth = 1;
  const float targetFreq1 = 7;
  const float targetFreq2 = 10;
  sincFilter(sincF1, sincF1Length, samplingFrequency, bandwidth, targetFreq1);
  sincFilter(sincF2, sincF2Length, samplingFrequency, bandwidth, targetFreq2);
}

void loop() {
  unsigned long currentTimestamp = isMicrosecond ? micros() : millis();
  if (currentTimestamp - lastTimestamp >= (isMicrosecond ? MICROSECONDS : MILLISECONDS) / samplingFrequency) {
    lastTimestamp = currentTimestamp;
    unsigned int input = analogRead(VOLUME);
    float sampling = ((1024 * 0.5 - 1) - input) / 1024;
    Serial.println(sampling);

    for (unsigned int i = 0; i < inputsLength - 1; i++) { 
      inputs[i] = inputs[i + 1];
    }
    inputs[inputsLength - 1] = sampling;

    const bool isSame = true;
    unsigned int convF1Length = isSame ? inputsLength : inputsLength + sincF1Length - 1 ;
    volatile float convF1[convF1Length] = {};
    unsigned int convF2Length = isSame ? inputsLength : inputsLength + sincF2Length - 1 ;
    volatile float convF2[convF2Length] = {};
    
    convolution(inputs, inputsLength, sincF1, sincF1Length, convF1, isSame);
    convolution(inputs, inputsLength, sincF2, sincF2Length, convF2, isSame);
  }
}

void sincFilter(float vector[], unsigned int vectorSize, float samplingRate, float bandWidth, float shiftFrequency) {
  for (unsigned int i = 0; i < vectorSize; i++) {
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
  for (unsigned int i = 0; i < vectorSize; i++) {
    summation = summation + pow(vector[i], 2);
  }
  for (unsigned int i = 0; i < vectorSize; i++) {
    vector[i] = vector[i] / summation;
  }
}

void convolution(float u[], unsigned int uLength, float v[], unsigned int vLength, float w[], bool isSame) {
  float tmpArr[uLength + vLength - 1] = {};
  for (unsigned int i = 0; i < uLength + vLength - 1; i++) {
    unsigned int index = i;
    float tmpVal = 0;
    for (unsigned int j = 0; j < vLength; j++) {
      if (index >= 0 && index < uLength) {
        tmpVal += u[index] * v[j];
      }
      index--;
      tmpArr[i] = tmpVal;
    }
  }
  if (isSame) {
    unsigned int start = ceil((vLength - 1) / 2.0);
    for (unsigned int i = 0; i < uLength; i++) {
      w[i] = tmpArr[start+i]; 
    }
  } else {
    for (unsigned int i = 0; i < uLength + vLength - 1; i++) {
      w[i] = tmpArr[i]; 
    }
  }
}
