#include "Arduino.h"

#include "S9706_test.h"

// please refer to the sensor datasheet.
//
// EN: http://www.hamamatsu.com/resources/pdf/ssd/s9706_kpic1060e.pdf
//     (Cat. No. KPIC1060E13 Aug. 2015 DN)
// JP: http://www.hamamatsu.com/resources/pdf/ssd/s9706_kpic1060j.pdf
//     (Cat. No. KPIC1060J14 Aug.2015 DN)

// Hold Time (micro seconds)
#define HOLD_TIME_US_T1   4
#define HOLD_TIME_US_T2   3
#define HOLD_TIME_US_T3   3
#define HOLD_TIME_US_T4   2000
#define HOLD_TIME_US_T5   3

// Clock Time (micro seconds)
#define CLOCK_TIME_US_POSITIVE_PULSE   1
#define CLOCK_TIME_US_NEGATIVE_PULSE   1



S9706::S9706(uint8_t *dataPin, uint8_t Num_sensor, 
             uint8_t rangePin, uint8_t clockPin, uint8_t gatePin) 
{
  num_sensor = Num_sensor;
  for(uint8_t i = 0; i < num_sensor; i++){
    _dataPin[i] = dataPin[i];
  }
  _rangePin = rangePin;
  _clockPin = clockPin;
  _gatePin = gatePin;

  for (int i = 0; i < num_sensor; i++) {
    for(int j = 0; j < 3; j++){
       _rgb[i][j] = 0;
    }
  }
}

void S9706::begin() {

  for(uint8_t i = 0; i < num_sensor; i++){
    pinMode(_dataPin[i], INPUT);
  }
  pinMode(_rangePin, OUTPUT);
  pinMode(_clockPin, OUTPUT);
  pinMode(_gatePin, OUTPUT);

  digitalWrite(_rangePin, LOW);
  digitalWrite(_clockPin, LOW);
  digitalWrite(_gatePin, LOW);
}

void S9706::update(uint8_t mode, unsigned long durationMs, unsigned long durationUs) {
  // initial pin output
  //
  // maybe previous integration process is finished.
  digitalWrite(_gatePin, LOW);
  digitalWrite(_clockPin, LOW);

  // wait t5
  //
  // should not change Range until integration time.
  // wait Hold Time after previous integraton process.
  delayMicroseconds(HOLD_TIME_US_T5);

  // change Range
  if (mode == S9706_MODE_HIGH) {
    digitalWrite(_rangePin, HIGH);
  } else {
    digitalWrite(_rangePin, LOW);
  }

  // wait t4
  delayMicroseconds(HOLD_TIME_US_T4);

  // start integration
  digitalWrite(_gatePin, HIGH);

  // wait specified integration time
  if (durationMs > 0) {
    delay(durationMs);
  }
  if (durationUs > 0) {
    if (durationUs < 1000) {
      delayMicroseconds(durationUs);
    } else {
      delay(durationUs / 1000);
      delayMicroseconds( durationUs % 1000);
    }
  }

  // stop integration
  digitalWrite(_gatePin, LOW);

  // wait t1
  delayMicroseconds(HOLD_TIME_US_T1);

  // read red, green, blue
  for (int i = 0; i < 3; i++) {
    uint16_t value[10];
    shiftInBits(LSBFIRST, 12, value);
    for(uint8_t j = 0; j < num_sensor; j++){
      Serial.println(value[j]);
      _rgb[j][i] = value[j];
    }
    // wait t2
    delayMicroseconds(HOLD_TIME_US_T2);
  }

  // wait t4
  delayMicroseconds(HOLD_TIME_US_T4);
}

uint16_t S9706::getRed(uint8_t i) {
  return _rgb[i][0];
}

uint16_t S9706::getGreen(uint8_t i) {
  return _rgb[i][1];
}

uint16_t S9706::getBlue(uint8_t i) {
  return _rgb[i][2];
}

uint16_t S9706::shiftInBits(uint8_t bitOrder, uint8_t bitLength, uint16_t * value) {
  

  for (uint8_t i = 0; i < bitLength; i++) {
    digitalWrite(_clockPin, HIGH);
    delayMicroseconds(CLOCK_TIME_US_POSITIVE_PULSE);
    if (bitOrder == LSBFIRST) {
      for(uint8_t j = 0; j < num_sensor; j++){
        value[j] |= digitalRead(_dataPin[j]) << i;
      }
    } else {
      for(uint8_t j = 0; j < num_sensor; j++){
        value[j] |= digitalRead(_dataPin[j]) << (bitLength - i);
      }
    }
    digitalWrite(_clockPin, LOW);
    delayMicroseconds(CLOCK_TIME_US_NEGATIVE_PULSE);
  }

  return 0;
}
