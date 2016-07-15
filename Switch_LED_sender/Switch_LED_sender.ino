#include <Wire.h>

#define BUTTON 7
//#define LED_A 5
//#define LED_B 9
#define sample_rate 100
//#define State_blink_A 1
//#define State_blink_AB 2

int buttonState = LOW;
long count = 0;
int click_count = 0;
int press_count = 0;

int prev_click_type = 0;

byte previous = 0;
//byte hold_state = 0;
int hold_time = 0;

void setup() {
  Wire.begin();
  //  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLUP);
//  pinMode(LED_A, OUTPUT);
//  pinMode(LED_B, OUTPUT);
}

void loop() {
  count++;

  byte current = digitalRead(BUTTON);

  Wire.beginTransmission(8);

  int click_type = press_type(current);

  if (previous - current == 1) {
    count = 0;
    previous == current;
  }

  if (prev_click_type - click_type == 1) {
    click_count++;
  } else if (prev_click_type - click_type == 2) {
    press_count++;
  }

  long time_elaps = count * sample_rate;

  //Serial.println(String(click_count) + ", " + String(press_count) + ", " + String(time_elaps));

  if (click_count % 2 != 0) {
    Wire.write("up");
//    short_press(time_elaps);
    press_count = 0;
  } else if (press_count % 2 != 0) {
    Wire.write("long");
//    long_press(time_elaps);
    click_count = 0;
  } else {
    Wire.write("down");
  }

  prev_click_type = click_type;

  Wire.endTransmission();

  delay(sample_rate);
}

int press_type (byte current) {
  long click_elaps = hold_time * sample_rate;
  if (current == LOW) {
    hold_time++;
    if (click_elaps <= 400) {
      return 1;
    } else {
      return 2;
    }
  } else {
    hold_time = 0;
    return 0;
  }
}

//void short_press(int time_elaps) {
//  if ((time_elaps % 2000) <= 1000) {
//    digitalWrite(LED_A, HIGH);
//  } else {
//    digitalWrite(LED_A, LOW);
//  }
//}

//void long_press(int time_elaps) {
//    if ((time_elaps % 1000) <= 500){
//      digitalWrite(LED_A, HIGH);
//      digitalWrite(LED_B, LOW);
//    } else {
//      digitalWrite(LED_A, LOW);
//      digitalWrite(LED_B, HIGH);
//    }
//}
