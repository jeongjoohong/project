//#define LED_R   12
//#define LED_L   11
#define SELECT  10
#define CLOCK   9
#define MOSI    8

byte index = 0;
bool increase;
uint8_t top = 144;

void setup() {
  Serial.begin(115200);
//  pinMode(LED_L, OUTPUT);
//  pinMode(LED_R, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SELECT, OUTPUT);

  if (index == 0) {
    increase = true;
  } else {
    increase = false;
  }
}

void loop() {
//  digitalWrite(LED_L, HIGH);
//  digitalWrite(LED_R, HIGH);
  if (increase) {
    if (index < top) {
      index++;
    } else {
      increase = false;
      index--;
    }
  } else {
    if (index > 0) {
      index--;
    } else {
      increase = true;
      index++;
    }
  }
//  index = 100;

  digitalWrite(SELECT, LOW);
  shiftOut(MOSI, CLOCK, MSBFIRST, B00000000);
  shiftOut(MOSI, CLOCK, MSBFIRST, 255-index);
  digitalWrite(SELECT, HIGH);

  digitalWrite(SELECT, LOW);
  shiftOut(MOSI, CLOCK, MSBFIRST, B00010000);
  shiftOut(MOSI, CLOCK, MSBFIRST, 255-index);
  digitalWrite(SELECT, HIGH);

  Serial.print(index);
  Serial.println("");
  delay(10);
}

