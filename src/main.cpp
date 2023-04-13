#include "S9706_test.h"
#include <Arduino.h>

const int LEDPIN = 33;
uint8_t Num_sensor = 1;
uint8_t dataPin[] = {26}; // No.1 Dout connected to digital pin 10
const int rangePin = 12; // No.4 Range connected to digital pin 4
const int clockPin = 14; // No.5 CK connected to digital pin 5
const int gatePin = 27;  // No.6 Gate connected to digital pin 6

S9706 colorSensor(dataPin, Num_sensor, rangePin, clockPin, gatePin);

void setup() {
  pinMode(LEDPIN,OUTPUT);
  ledcSetup(0,12800,8); 
  ledcAttachPin(LEDPIN,0);

  Serial.begin(9600);
  colorSensor.begin();
  Serial.println(dataPin[0]);
  
}

void loop() {
  
  ledcWrite(0,10);
  // S9706::update(mode, durationMs, durationUs = 0)
  //    mode:         specify S9706_MODE_HIGH or S9706_MODE_LOW
  //    durationMs:   specify sensor integration process time (milliseconds)
  //    durationUs:   specify sensor integration process time (microseconds) or not
  colorSensor.update(S9706_MODE_HIGH, 100);

  // get updated RGB (value min: 0, max: 4095)
  float percent[Num_sensor];
  uint8_t red[Num_sensor];
  uint8_t blue[Num_sensor];
  uint8_t green[Num_sensor];
  for(int i = 0; i < Num_sensor; i++){
    red[i] = colorSensor.getRed(i);
    green[i] = colorSensor.getGreen(i);
    blue[i] = colorSensor.getBlue(i);
    percent[i] = (float)red[i] / (red[i] + blue[i] + green[i]) * 100;
    Serial.print(red[i]);
    Serial.print("\t");
    Serial.print(green[i]);
    Serial.print("\t");
    Serial.print(blue[i]);
    Serial.println();
  }
  

//  if(percent > 30){
//    
//  }else if(percent > 22){
//    Serial.println("**********************************");
//    Serial.println("**     !!!    LEAK !!!          **");
//    Serial.println("**********************************");
//    Serial.println();
//  }else {
//    // Serial.println("**********************************");
//    // Serial.println("**            UNLEAK            **");
//    // Serial.println("**********************************");
//    // Serial.println();
//  }
  for(uint8_t i = 0; i < Num_sensor; i++){
    Serial.print(percent[i]);
    Serial.print("\t");
    Serial.println(red[i] + green[i] + blue[i]);
  }
  
  ledcWrite(0,0);

  delay(500);
}