#include "OneEuroFilter.h"

#define VOLUME_L    A0
#define VOLUME_R    A1
#define VOLUME_C    A2

#define REG_DATA    10 // DIGITAL(PWM~)
#define REG_LATCH   9
#define REG_CLOCK   8
#define POT_SELECT  7
#define POT_CLOCK   6
#define POT_MOSI    5
#define LED_L       4
#define LED_R       3

#define MILLISECONDS  1000L
#define MICROSECONDS  1000000L

const bool isMicrosecond    = true;
const bool isDebugging      = true;
const bool isFiltering      = false;
const float ledDutyCycle    = 0.5;
const double minCutoff      = 1.0;
const double beta           = 0.007;
const double inputFrequency = 50;

unsigned long long updatePrevious;
OneEuroFilter channel1(inputFrequency, minCutoff, beta);
OneEuroFilter channel2(inputFrequency, minCutoff, beta);

short level1;
short level2;
short level3;
short number[4];
unsigned long long level1Previous;
unsigned long long level2Previous;
unsigned long long level3Previous;
bool isLed1On;
bool isLed2On;
bool isUpdated;
unsigned long long segmentIndex;

void setup() {
  Serial.begin(115200);
  pinMode(REG_DATA, OUTPUT);
  pinMode(REG_LATCH, OUTPUT);
  pinMode(REG_CLOCK, OUTPUT);
  pinMode(POT_MOSI, OUTPUT);
  pinMode(POT_SELECT, OUTPUT);
  pinMode(POT_CLOCK, OUTPUT);
  pinMode(LED_L, OUTPUT);
  pinMode(LED_R, OUTPUT);

  updatePrevious = 0;
  level1Previous = 0;
  level2Previous = 0;
  level3Previous = 0;
  isLed1On = false;
  isLed2On = false;
  isUpdated = false;
  segmentIndex = 0;

  registerBlink(2, 500);
}

void loop() {
  unsigned long long current = isMicrosecond ? micros() : millis();
  if (current - updatePrevious >= (isMicrosecond ? MICROSECONDS : MILLISECONDS) / inputFrequency) {
    updatePrevious = current;
    short received1 = analogRead(VOLUME_L);
    short received2 = analogRead(VOLUME_R);
    short received3 = analogRead(VOLUME_C);
    short filtered1;
    short filtered2;

    isUpdated = true;

    if (isFiltering) {
      filtered1 = round(channel1.filter(received1));
      filtered2 = round(channel2.filter(received2));
      level1 = round(filtered1 * (30.0 / 1024.0));
      level2 = round(filtered2 * (30.0 / 1024.0));
      level3 = round(received3 * (256.0 / 1024.0));
    } else {
      level1 = round(received1 * (30.0 / 1024.0));
      level2 = round(received2 * (30.0 / 1024.0));
      level3 = round(received3 * (256.0 / 1024.0));
    }

    if (isDebugging) {
      Serial.print(received1);
      Serial.print('\t');
      Serial.print(received2);
      if (isFiltering) {
        Serial.print('\t');
        Serial.print(filtered1);
        Serial.print('\t');
        Serial.print(filtered2);
      }
      Serial.print('\n');
    }
    
    number[0] = level1 / 10;
    number[1] = level1 % 10;
    number[2] = level2 / 10;
    number[3] = level2 % 10;
  }

  current = isMicrosecond ? micros() : millis();
  if (level1 == 0) {
    isLed1On = false;
    digitalWrite(LED_L, LOW);
  } else {
    if (current - level1Previous >= (isMicrosecond ? MICROSECONDS : MILLISECONDS) / level1) {
      level1Previous = current;
      isLed1On = true;
      digitalWrite(LED_L, HIGH);
    } else {
      if (current - level1Previous >= (isMicrosecond ? MICROSECONDS : MILLISECONDS) / level1 * ledDutyCycle) {
        if (isLed1On == true) {
          isLed1On = false;
          digitalWrite(LED_L, LOW);
        }
      }
    }
  }
  if (level2 == 0) {
    isLed2On = false;
    digitalWrite(LED_R, isLed2On);
  } else {
    if (current - level2Previous >= (isMicrosecond ? MICROSECONDS : MILLISECONDS) / level2) {
      level2Previous = current;
      isLed2On = true;
      digitalWrite(LED_R, HIGH);
    } else {
      if (current - level2Previous >= (isMicrosecond ? MICROSECONDS : MILLISECONDS) / level2 * ledDutyCycle) {
        if (isLed2On == true) {
          isLed2On = false;
          digitalWrite(LED_R, LOW);
        }
      }
    }
  }
  if (isUpdated) {
    isUpdated = false;
    digitalWrite(POT_SELECT, LOW);
    shiftOut(POT_MOSI, POT_CLOCK, MSBFIRST, B00000000);
    shiftOut(POT_MOSI, POT_CLOCK, MSBFIRST, level3);
    digitalWrite(POT_SELECT, HIGH);
    digitalWrite(POT_SELECT, LOW);
    shiftOut(POT_MOSI, POT_CLOCK, MSBFIRST, B00010000);
    shiftOut(POT_MOSI, POT_CLOCK, MSBFIRST, level3);
    digitalWrite(POT_SELECT, HIGH);
  }

  {
    int index = segmentIndex % 4;
    byte segment = (B1111 ^ (1 << ((sizeof(number) / sizeof(int) - 1) - index)));
    byte data = (segment << 4) + number[index];
    digitalWrite(REG_LATCH, LOW);
    shiftOut(REG_DATA, REG_CLOCK, MSBFIRST, data);
    digitalWrite(REG_LATCH, HIGH);
    segmentIndex++;
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

