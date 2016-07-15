/* This sketch controls a single LED's behavior using 4 toggle buttons.
      The LL button gives a 3 second blink.
      The LC button gives a 1 second blink.
      The RC button gives a triplet blink.
      The RR button toggles on/off.
      The LL & RR buttons in combination give a syncopated blink.

   Authors: Samuel McHugh, Chris Shoemaker
   Date of last revision: 6/28/2016

   Original: Sketch uses 3,174 bytes (9%) of program storage space.
             Global variables use 190 bytes (9%) of dynamic memory.
   New:      Sketch uses 1,656 bytes (5%) of program storage space. 
             Global variables use 12 bytes (0%) of dynamic memory.
*/

#define BUTTONPIN_RR 5  // right-right
#define BUTTONPIN_RC 4  // right-center
#define BUTTONPIN_LC 3  // left-center
#define BUTTONPIN_LL 2  // left-left
#define LED 9 // Arduino Ethernet built in LED
#define delay_time 20 // gives 20ms delay time

unsigned int count = 0;
byte previous = 0;

void setup() {
  pinMode(BUTTONPIN_RR, INPUT);
  pinMode(BUTTONPIN_RC, INPUT);
  pinMode(BUTTONPIN_LC, INPUT);
  pinMode(BUTTONPIN_LL, INPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  count++;

  // preforms a read of the current state
  byte current = 8 * digitalRead(BUTTONPIN_LL) + 4 * digitalRead(BUTTONPIN_LC)
  + 2 * digitalRead(BUTTONPIN_RC) + 1 * digitalRead(BUTTONPIN_RR);

  /*
  byte current = 0;
  current |= digitalRead(BUTTONPIN_RR) << 0;
  current |= digitalRead(BUTTONPIN_RC) << 1;
  current |= digitalRead(BUTTONPIN_LC) << 2;
  current |= digitalRead(BUTTONPIN_LL) << 3;
  */
  
  // checks if the current read state is the same as the previous state
  if (current != previous) {
    count = 0;
    }
    
  // updates the previous state to the now current state
  previous = current;

  unsigned long secs_elapsed = count * delay_time;
  
  // preforms an action based on the state
  if (current == 1){
    button_RR_routine();
  } else if(current == 2) {
    button_RC_routine(count);
  } else if(current == 4) {
    button_LC_routine(count);
  } else if(current == 8) {
    button_LL_routine(count);
  } else if(current == 9) {
    button_LL_RR_routine(count);
  } else {
    digitalWrite(LED,LOW); 
  }
  switch (current) {
    case STATE_A:  button_RR_routine(); break;
    case STATE_B:  button_RC_routine(secs_elapsed); break;
    case STATE_C:  button_LC_routine(); break;
    case STATE_D:  button_LL_routine(); break;
    default: digitalWrite(LED,LOW); 
  }
  
  delay(delay_time);
}

// turns on the LED
void button_RR_routine() {
  digitalWrite(LED,HIGH);
}

// 3 second blink
void button_LL_routine(int count) {
  if (count * delay_time % 6000 <= 3000){
    digitalWrite(LED,HIGH);
  } else {
    digitalWrite(LED,LOW);
  }
}

// 2 second blink
void button_LC_routine(int count) {
  if ( ( (count * delay_time) % 4000) <= 2000) {
    digitalWrite(LED,HIGH);
  } else {
    digitalWrite(LED,LOW);
  }
}

// triplet blink
void button_RC_routine(int count) {
  if ( ( ( (count * delay_time) % 200 ) <= 100 ) 
    && ( ( (count * delay_time) % 1500) <= 500) ) {
    digitalWrite(LED,HIGH);
  } else {
    digitalWrite(LED,LOW);
  }
}

// short short long blink
void button_LL_RR_routine(int count) {
  if (count * delay_time % 200 <= 100 ||
  ((400 <= count * delay_time % 1500) && 
  (count * delay_time % 1500 <= 1400))) {
    digitalWrite(LED,HIGH);
  } else {
    digitalWrite(LED,LOW);
  }
}
