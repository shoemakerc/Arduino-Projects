#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Begin test");
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned int sender = 0;
  char sender_buffer[4];
  sender_buffer[0] = sender;
  sender_buffer[1] = 4;
  Serial.println(sender_buffer[0]);
  Wire.beginTransmission(13);
  Wire.write(sender_buffer);
  Wire.endTransmission();
  delay(100);
}
