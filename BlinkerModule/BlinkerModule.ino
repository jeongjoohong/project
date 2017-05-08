#include "OneEuroFilter.h"
#include "TimerOne.h"

#define VOLUME_L    A0
#define VOLUME_R    A1
#define VOLUME_T    A2

#define LED_R       12
#define LED_L       11
#define POT_SELECT  10
#define POT_CLOCK   9
#define POT_MOSI    8
#define REG_DATA    7
#define REG_LATCH   6
#define REG_CLOCK   5

#define MILLISECONDS  1000L
#define MICROSECONDS  1000000L

const bool isMicrosecond  = true;
const bool isFiltering    = true;
const bool isDebugging    = true;

const float inputResolution   = 1024;
const float maxBlinkFrequency = 30;
const float minDigipotLevel   = 115;
const float maxDigipotLevel   = 255;
const float ledDutyCycle      = 0.5;

const double samplingFrequency = 1000;
const double minCutoff  = 1.0;
const double beta       = 0.007;
OneEuroFilter channel1(samplingFrequency, minCutoff, beta);
OneEuroFilter channel2(samplingFrequency, minCutoff, beta);
OneEuroFilter channel3(samplingFrequency, minCutoff, beta);

unsigned long long previousRead;
unsigned long long previousBlinkL;
unsigned long long previousBlinkR;

short volume1;
short volume2;
short volume3;
short segment[4];
bool isLedLOn;
bool isLedROn;
unsigned long long segmentIndex;

void setup() {
  Serial.begin(115200);
  pinMode(REG_DATA, OUTPUT);
  pinMode(REG_LATCH, OUTPUT);
  pinMode(REG_CLOCK, OUTPUT);
  pinMode(POT_SELECT, OUTPUT);
  pinMode(POT_CLOCK, OUTPUT);
  pinMode(POT_MOSI, OUTPUT);
  pinMode(LED_L, OUTPUT);
  pinMode(LED_R, OUTPUT);

  previousRead = 0;
  previousBlinkL = 0;
  previousBlinkR = 0;  
  isLedLOn = false;
  isLedROn = false;
  segmentIndex = 0;

  registerBlink(2, 500);

  Timer1.initialize(500);
  Timer1.attachInterrupt(displaySegment);
}

void loop() {
  unsigned long long currentTimestamp = isMicrosecond ? micros() : millis();
  if (currentTimestamp - previousRead >= (isMicrosecond ? MICROSECONDS : MILLISECONDS) / samplingFrequency) {
    previousRead = currentTimestamp;
    short input1 = analogRead(VOLUME_L);
    short input2 = analogRead(VOLUME_R);
    short input3 = analogRead(VOLUME_T);
    short filter1;
    short filter2;
    short filter3;
    short digipotLevel;
    
    if (isFiltering) {
      filter1 = round(channel1.filter(input1));
      filter2 = round(channel2.filter(input2));
      filter3 = round(channel3.filter(input3));
      volume1 = round(filter1 * (maxBlinkFrequency / inputResolution));
      volume2 = round(filter2 * (maxBlinkFrequency / inputResolution));
      digipotLevel = round(filter3 * ((maxDigipotLevel - minDigipotLevel) / inputResolution) + minDigipotLevel);
    } else {
      volume1 = round(input1 * (maxBlinkFrequency / inputResolution));
      volume2 = round(input2 * (maxBlinkFrequency / inputResolution));
      digipotLevel = round(input3 * ((maxDigipotLevel - minDigipotLevel) / inputResolution) + minDigipotLevel);
    }

    segment[0] = volume1 / 10;
    segment[1] = volume1 % 10;
    segment[2] = volume2 / 10;
    segment[3] = volume2 % 10;

    if (digipotLevel == minDigipotLevel) {
      digipotLevel = 0;
    }
    if (digipotLevel != volume3) {
      volume3 = digipotLevel;
      digitalWrite(POT_SELECT, LOW);
      shiftOut(POT_MOSI, POT_CLOCK, MSBFIRST, B00000000);
      shiftOut(POT_MOSI, POT_CLOCK, MSBFIRST, volume3);
      digitalWrite(POT_SELECT, HIGH);
      digitalWrite(POT_SELECT, LOW);
      shiftOut(POT_MOSI, POT_CLOCK, MSBFIRST, B00010000);
      shiftOut(POT_MOSI, POT_CLOCK, MSBFIRST, volume3);
      digitalWrite(POT_SELECT, HIGH);
    }
    
    if (isDebugging) {
      Serial.print(input1);
      Serial.print("\t");
      Serial.print(input2);
      Serial.print('\t');
      Serial.print(input3);
      if (isFiltering) {
        Serial.print('\t');
        Serial.print(filter1);
        Serial.print('\t');
        Serial.print(filter2);
        Serial.print('\t');
        Serial.print(filter3);
      }
      Serial.print('\n');
    }
  }

  currentTimestamp = isMicrosecond ? micros() : millis();
  if (volume1 == 0) {
    if (isLedLOn) {
      isLedLOn = false;
      digitalWrite(LED_L, LOW);
    }
  } else if (volume1 == maxBlinkFrequency) {
    if (!isLedLOn) {
      isLedLOn = true;
      digitalWrite(LED_L, HIGH);
    }
  } else {
    if (currentTimestamp - previousBlinkL >= (isMicrosecond ? MICROSECONDS : MILLISECONDS) / volume1) {
      previousBlinkL = currentTimestamp;
      isLedLOn = true;
      digitalWrite(LED_L, HIGH);
    } else {
      if (currentTimestamp - previousBlinkL >= (isMicrosecond ? MICROSECONDS : MILLISECONDS) / volume1 * ledDutyCycle) {
        if (isLedLOn == true) {
          isLedLOn = false;
          digitalWrite(LED_L, LOW);
        }
      }
    }
  }
  if (volume2 == 0) {
    if (isLedROn) {
      isLedROn = false;
      digitalWrite(LED_R, isLedROn);
    }
  } else if (volume2 == maxBlinkFrequency) {
    if (!isLedROn) {
      isLedROn = true;
      digitalWrite(LED_R, HIGH);
    }
  } else {
    if (currentTimestamp - previousBlinkR >= (isMicrosecond ? MICROSECONDS : MILLISECONDS) / volume2) {
      previousBlinkR = currentTimestamp;
      isLedROn = true;
      digitalWrite(LED_R, HIGH);
    } else {
      if (currentTimestamp - previousBlinkR >= (isMicrosecond ? MICROSECONDS : MILLISECONDS) / volume2 * ledDutyCycle) {
        if (isLedROn == true) {
          isLedROn = false;
          digitalWrite(LED_R, LOW);
        }
      }
    }
  }
}

void registerBlink(int times, int duration) {
  digitalWrite(REG_LATCH, LOW);
  shiftOut(REG_DATA, REG_CLOCK, MSBFIRST, B00000000);
  digitalWrite(REG_LATCH, HIGH);
  delay(200);
  for (int i = 0; i < times; i++) {
    digitalWrite(REG_LATCH, LOW);
    shiftOut(REG_DATA, REG_CLOCK, MSBFIRST, B11111111);
    digitalWrite(REG_LATCH, HIGH);
    delay(duration);
    digitalWrite(REG_LATCH, LOW);
    shiftOut(REG_DATA, REG_CLOCK, MSBFIRST, B00000000);
    digitalWrite(REG_LATCH, HIGH);
    delay(duration);
  }
}

void displaySegment() {
  int index = segmentIndex % 4;
  byte sequence = (B1111 ^ (1 << ((sizeof(segment) / sizeof(int) - 1) - index)));
  byte data = (sequence << 4) + segment[index];
  digitalWrite(REG_LATCH, LOW);
  shiftOut(REG_DATA, REG_CLOCK, MSBFIRST, data);
  digitalWrite(REG_LATCH, HIGH);
  segmentIndex++;
}

