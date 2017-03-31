#include "OneEuroFilter.h"

const int volum1Pin = A0;
const int volum2Pin = A1;
const int clockPin  = 8;
const int latchPin  = 9;
const int dataPin   = 10; // DIGITAL(PWM~)
const int led1Pin   = 11;
const int led2Pin   = 12;

const boolean isMicrosecond = true;
const boolean isDebugging = true;
const boolean isFiltering = false;
unsigned long updateInterval = 11;  // around 90Hz
double minCutoff = 1.0;
double beta = 0.007;

const long milliseconds = 1000;
const long microseconds = 1000000;
unsigned long updatePrevious;
double readFrequency = milliseconds / updateInterval;
OneEuroFilter channel1(readFrequency, minCutoff, beta);
OneEuroFilter channel2(readFrequency, minCutoff, beta);

int level1;
int level2;
int number[4];
unsigned long level1Previous;
unsigned long level2Previous;
int led1State;
int led2State;
double ledDutyCycle = 0.5;

void setup() {
  Serial.begin(9600);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  
  updatePrevious = 0;
  level1Previous = 0;
  level2Previous = 0;
  led1State = LOW;
  led2State = LOW;

  registerBlink(2, 500);
}

void loop() {
  unsigned long current = isMicrosecond ? micros() : millis();
  if (current - updatePrevious >= (isMicrosecond ? 1000 : 1) * updateInterval) {
    updatePrevious = current;
    int received1 = analogRead(volum1Pin);
    int received2 = analogRead(volum2Pin);
    int filtered1;
    int filtered2;

    if (isFiltering) {
      filtered1 = round(channel1.filter(received1));
      filtered2 = round(channel2.filter(received2));
      level1 = round(filtered1 * (30.0 / 1024.0));
      level2 = round(filtered2 * (30.0 / 1024.0));
    } else {
      level1 = round(received1 * (30.0 / 1024.0));
      level2 = round(received2 * (30.0 / 1024.0));
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
    led1State = LOW;
    digitalWrite(led1Pin, led1State);
  } else {
    if (current - level1Previous >= (isMicrosecond ? microseconds : milliseconds) / level1) {
      level1Previous = current;
      led1State = HIGH;
      digitalWrite(led1Pin, led1State);
    } else {
      if (current - level1Previous >= (isMicrosecond ? microseconds : milliseconds) / level1 * ledDutyCycle) {
        if (led1State == HIGH) {
          led1State = LOW;
          digitalWrite(led1Pin, led1State);
        }
      }
    }
  }
  if (level2 == 0) {
    led2State = LOW;
    digitalWrite(led2Pin, led2State);
  } else {
    if (current - level2Previous >= (isMicrosecond ? microseconds : milliseconds) / level2) {
      level2Previous = current;
      led2State = HIGH;
      digitalWrite(led2Pin, led2State);
    } else {
      if (current - level2Previous >= (isMicrosecond ? microseconds : milliseconds) / level2 * ledDutyCycle) {
        if (led2State == HIGH) {
          led2State = LOW;
          digitalWrite(led2Pin, led2State);
        }
      }
    }
  }
  
  for (int i = 0; i < sizeof(number) / sizeof(int); i++) {
    byte segment = (0b1111 ^ (1 << ((sizeof(number) / sizeof(int) - 1) - i)));
    byte data = (segment << 4) + number[i];
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, data);
    digitalWrite(latchPin, HIGH);
    delay(2);
  }
}

void registerBlink(int times, int duration) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
  digitalWrite(latchPin, HIGH);
  delay(200);
  for (int i = 0; i < times; i++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11111111);
    digitalWrite(latchPin, HIGH);
    delay(duration);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 0b00000000);
    digitalWrite(latchPin, HIGH);
    delay(duration);
  }
}

