int sensorPin = A0;
int sensorValue = 0;

float samplingRate = 100;
const unsigned int samplingTimeMicroSec = (1 / samplingRate) * (1e6);
const int vecterSize = 64;
int inputArray[vecterSize] = {};
float vector[vecterSize] = {};

void setup() {
  Serial.begin(115200);

  float centerFrequency = 13.0;
  
  float bandWidth = 0.5;
  generationSincSignals(centerFrequency, samplingRate, bandWidth, vecterSize, vector);

//  for (int i = 0; i < vecterSize; i++) {
//    Serial.println(vector[i] * 100);
//  }
//Serial.println(samplingTimeMicroSec);
//Serial.println((1 / samplingRate));
//Serial.println(1e6); 
}

void loop() {
  unsigned long ticTime = micros();
  // reading new value

// sampling
for (int i = 0; i<vecterSize; i++)
{
  while(true)
  {
    if (micros() - ticTime > samplingTimeMicroSec*(i+1))
    {
      inputArray[i] = analogRead(sensorPin);
      break;
    }
  }  
}

  unsigned long tocTime2 = micros() - ticTime;
//  Serial.print(tocTime2);
//  Serial.print(", ");


  // convolution
  float ouputArray[vecterSize] = {};
  convolution(inputArray, vecterSize, vector, vecterSize, ouputArray, true);

  // power culculation
  float power1 = 0.0;
  for (int i = 0; i < vecterSize; i++) {
    power1 = power1 + pow(ouputArray[i], 2);
  }
  unsigned long tocTime = micros() - ticTime;


  // view power
//  Serial.print(tocTime);
//  Serial.print(", ");
  Serial.println(power1);
  
  
  // waiting time
  
  while(true) {
    if (micros() - ticTime > 1e6) {
      break;
    }
  }
  
  




//  Serial.println("step1");
//  Serial.println(micros() - ticTime);
  

//  Serial.print("0, 1024, ");
//  Serial.print(sensorValue);

}

//--------------------------------------------------------------------------
void generationSincSignals(float centerFrequency, float samplingRate, float bandWidth, int vecterSize, float vector[]) {
  // sinc generation
  for (int j = 0; j < vecterSize; j++) {
    float timeVector = j / samplingRate;
    float x = bandWidth * (timeVector - (vecterSize / samplingRate * 0.5));
    float sinc;
    if (x == 0) {
      sinc = 1;
    } else {
      sinc = sin(x * PI) / (x * PI);
    }

    // frequency shift
    vector[j] = sinc * cos(2 * PI * centerFrequency * timeVector);
  }

  // sinc normalization
  float summation = 0;
  for (int j = 0; j < vecterSize; j++) {
    summation = summation + pow(vector[j], 2);
  }
  for (int j = 0; j < vecterSize; j++) {
    vector[j] = vector[j] / summation;
  }
}


//--------------------------------------------------------------------------
void convolution(int u[], unsigned int uLength, float v[], unsigned int vLength, float w[], bool isSame) {
//  unsigned long ticTime = micros();
  
  
  float tmpArr[uLength + vLength - 1] = {};
  
  for (unsigned int i = 0; i < uLength + vLength - 1; i++) {
    unsigned int index = i;
    float tmpVal = 0;
    for (unsigned int j = 0; j < vLength; j++) {
      if (index >= 0 && index < uLength) {
        tmpVal += float(u[index]) * v[j];
      }
      index--;
      tmpArr[i] = tmpVal;
    }
  }
//  Serial.println("step2");
//  Serial.println(micros() - ticTime);

  
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
//  Serial.println("step3");
//  Serial.println(micros() - ticTime);

  
}
