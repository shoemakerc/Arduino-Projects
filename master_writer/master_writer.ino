// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

#define BUTTON_PIN 7  // the pin number for input (for me a push button)


void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
}

byte x = 0;

void loop() {
//  
//  count++;
  byte current = digitalRead(BUTTON_PIN);
//  click_type = press_type(current);
//  if (current != previous) {
//    click_count++;
//    count = 0;
//    previous == current;
//  }
  Wire.beginTransmission(8); // transmit to device #8
  if(current == 0){
//  Wire.write("x : ");        // sends five bytes
//  Wire.write(x);              // sends one byte
  Wire.write("down ");
  } else {
    Wire.write("up ");
  }
  Serial.println("Hello");
  Wire.endTransmission();    // stop transmitting

 // x = x+2;
  delay(100);
}


//int press_type (byte current){
//  long click_elaps = hold_time*sample_rate;
//  if ((current == LOW) {
//    hold_time++;
//    if ((click_elaps)<=500)) {
//      click_count |= 1 << 0 ;
//    } else {
//      click_count |= 1 << 1 ;
//      return 2;
//    }
//  } else {
//    hold_time = 0;
//    return 0;
//  }
//}


