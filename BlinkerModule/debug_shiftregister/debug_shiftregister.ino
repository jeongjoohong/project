int dataPin = 7;
int latchPin = 6;
int clockPin = 5;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
//  for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, B11111100);  
    digitalWrite(latchPin, HIGH);
//    delay(1000000);
//  }
}

