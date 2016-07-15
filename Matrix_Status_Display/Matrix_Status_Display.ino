#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <Fonts\TomThumb.h>
#include <Fonts/FreeSerifBoldItalic9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Wire.h>
#include "CtrlState.h"
#include "Header.h" //Sets up the header on sending data

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#else
setoffalarm now
#endif

#define Use32linePanel 1

#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)


#if Use32linePanel
#define LAT 10
#else
#define LAT A3
#endif
#define OE  9   // Output Enable
#define A   A0
#define B   A1
#define C   A2
#define D   A3

#define SENSOR_HEADER 0
#define TEXT_HEADER   1
#define TEST_HEADER   2

#ifdef Use32linePanel
// Constuctor for 64x32 panel
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true, 64);
#else
// Constuctor for 32x16 panel
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);
#endif

//<-----------StopLight States----------->
#define SLT_Off         0
#define SLT_Red         1
#define SLT_Yellow      2
#define SLT_Green       4
#define SLT_BlinkRed  (SLT_Red | SLT_Green)
#define SLT_BlinkYlw  (SLT_Yellow | SLT_Red)
#define SLT_BlinkGrn  (SLT_Green | SLT_Yellow)
#define SLT_Rotating  (SLT_Red | SLT_Green | SLT_Yellow)


ORVCtrlUnion orvcu = {0,0}; //Sets up the sensor data array
HeaderUnion data   = {0}  ; //Sets up the header
char textdata[]    = ""   ; //Array of characters to display.
int  numberdata    = 0    ; //current test number
int  timedata      = 0    ; //desired countdown time
int  current       = 0    ; //last requested countdown time
int  x             = 0    ; //marks the beginning of the countdown
bool new_data      = false; //flags if new data has come in
unsigned long start= 0    ; //Keeps track of start time.
int  i             = 0    ; //for scrolling text

int x_cor = 0; //TODO
int y_cor = 0;//TODO
int col = 0;//TODO

char buffer[32];            //Creates a buffer to hold 32 bytes of text data


void setup()
{
  // Setup host communication
  Wire.begin(13);  // use I2C device address 13
  Wire.onReceive(receiveEvent);  // register event handler

  // Setup RGB LED Matrix
  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge
  matrix.setTextSize(1);

  // remove serial port debugger when releasing
  Serial.begin(9600);
}

void loop() {
  if (new_data) {
    matrix.fillScreen(0);
    matrix.setFont(0);
    matrix.setCursor(0, 0);
    if(data.bytes[0] == SENSOR_HEADER){      
      display_status();
    } else if (data.bytes[0] == TEXT_HEADER){ 
      display_text();
    } else if (data.bytes[0] == TEST_HEADER){
      display_test();
    }
    delay(10);
    matrix.swapBuffers(true);
  }
  new_data = false;
}

void receiveEvent(int howMany) {
  new_data = true; 
  //Checks for data 
  if (Wire.available()) {
    data.bytes[0] = Wire.read();              //First checks the header
    if(data.bytes[0] == SENSOR_HEADER){       //Reads in sensor data
      orvcu.bytes[0] = Wire.read();
      orvcu.bytes[1] = Wire.read();
      orvcu.bytes[2] = Wire.read();
      orvcu.bytes[3] = Wire.read();
    } else if (data.bytes[0] == TEXT_HEADER){ //reads in test data
      Wire.readBytes(buffer, (howMany-1));    //Stores the text into the buffer
      x_cor = Wire.read();
      y_cor = Wire.read();
      col   = Wire.read();
    } else if (data.bytes[0] == TEST_HEADER){ //reads in countdown data
      numberdata = Wire.read();
      timedata = (Wire.read() | Wire.read() << 8); // read two bytes and merge them into an int
      if (timedata != current){
        x = 0;    //Resets the timer
        current = timedata;
      }
    }
  }
}

void display_status() {

  uint16_t black  = matrix.Color888(0  , 0  , 0  );
  uint16_t dimgry = matrix.Color888(16 , 16 , 16 );
  uint16_t medgry = matrix.Color888(32 , 32 , 32 );
  uint16_t brtgry = matrix.Color888(48 , 48 , 48 );
  uint16_t bold   = matrix.Color888(129, 129, 129);
  uint16_t dimgrn = matrix.Color888(0  , 16 , 0  );
  uint16_t medgrn = matrix.Color888(0  , 97 , 0  );
  uint16_t brtgrn = matrix.Color888(0  , 129, 0  );
  uint16_t dimred = matrix.Color888(16 , 0  , 0  );
  uint16_t medred = matrix.Color888(97 , 0  , 0  );
  uint16_t brtred = matrix.Color888(129, 0  , 0  );
  uint16_t dimblu = matrix.Color888(0  , 0  , 16 );
  uint16_t medblu = matrix.Color888(0  , 0  , 97 );
  uint16_t brtblu = matrix.Color888(0  , 0  , 255);
  uint16_t medmgt = matrix.Color888(65 , 0  , 65  );
  uint16_t dimylw = matrix.Color888(16 , 16 , 0   );
  uint16_t medylw = matrix.Color888(65 , 65 , 0   );
  uint16_t medcyn = matrix.Color888(0  , 65 , 65  );

  printTightDefault((orvcu.bit.CVT ? "24V" : "0 V"), 16, 8, (orvcu.bit.CVT ? dimgrn : dimred)); 
  printTightDefault("MOS", 16, 16, (orvcu.bit.MOS ? (orvcu.bit.MOR ? bold : medylw) : dimblu));
  printTightDefault("R2E", 0 , 24, (orvcu.bit.R2E ? medblu : dimgry));
  printTightDefault("ESP", 0 , 8 , (orvcu.bit.ESP ? dimgrn : brtred)); 
  printTightDefault("DRO", 16, 0 , (orvcu.bit.DRO ? brtred : dimgrn)); ///unclear
  printTightDefault("SBT", 0 , 16, (orvcu.bit.SBT ? brtred : dimgrn));
  printTightDefault("MBK", 48, 0 , (orvcu.bit.MBK ? bold   : dimblu)); 
  printTightDefault("R2R", 32, 0 , (orvcu.bit.R2R ? dimgrn : dimred)); 
  printTightDefault("SRO", 32, 8 , (orvcu.bit.SRO ? dimgrn : dimred)); 
  printTightDefault("CMT", 48, 16, (orvcu.bit.CMT ? brtred : dimgrn)); //Not clear
  printTightDefault("CMH", 48, 8 , (orvcu.bit.CMH ? dimgrn : dimred)); 
  printTightDefault("RES", 16, 24, (orvcu.bit.R2E ? medcyn : medmgt));
  
  switch (orvcu.bit.IMN) {
    case 0 :
      printTightDefault("IMN", 32, 16, dimred);
      break;
    case 1:
      if ( (millis() % 1000) <= 500) {
        printTightDefault("IMN", 32, 16, dimylw);
      } else {
        printTightDefault("IMN", 32, 16, medylw);
      } break;
  }
  
  String temp = String(orvcu.bit.TCV);
  printTightDefault(temp.c_str(), 43, 24, dimblu);

  String temp1 = String(orvcu.bit.TCR);
  printTightDefault("T", 32, 24, dimblu);
  printTightDefault(temp1.c_str(), 37, 24, medblu); 
  
  switch (orvcu.bit.SLT) {
    case SLT_Off :
      printTightDefault("SLT", 0, 0, dimgry);
      break;
    case SLT_Green:
      printTightDefault("SLT", 0, 0, dimgrn);
      break;
    case SLT_Yellow:
      printTightDefault("SLT", 0, 0, medylw);
      break;
    case SLT_Red:
      printTightDefault("SLT", 0, 0, brtred);
      break;
    case SLT_BlinkRed:
      if ( (millis() % 1000) <= 500) {
        printTightDefault("SLT", 0, 0, brtred);
      } else {
        printTightDefault("SLT", 0, 0, dimred);
      }
      break;
    case SLT_BlinkYlw:
      if ( (millis() % 1000) <= 500) {
        printTightDefault("SLT", 0, 0, medylw);
      } else {
        printTightDefault("SLT", 0, 0, dimylw);
      }
      break;
    case SLT_BlinkGrn:
      if ( (millis() % 1000) <= 500) {
        printTightDefault("SLT", 0, 0, dimgrn);
      } else {
        printTightDefault("SLT", 0, 0, dimgrn);
      }
      break;
    case SLT_Rotating:
      if ( (millis() % 1000) <= 333) {
        printTightDefault("SLT", 0, 0, brtred);
      } else if ( (millis() % 1000) > 333
                  && (millis() % 1000) <= 666) {
        printTightDefault("SLT", 0, 0, medylw);
      } else {
        printTightDefault("SLT", 0, 0, dimgrn);
      }
      break;
  }
}

void display_text(){//TODO
  Serial.println(y_cor);
  matrix.setCursor(i + x_cor , y_cor);
  if (i > 64){ //screen is 64 pixels wide
    i = -96; //32 characters, 3 pixels a character
  }
  const GFXfont *pfont = &TomThumb;
  matrix.setFont(pfont);
  matrix.setTextColor(col);
  for(int j = 0; j < 32; j++){
    matrix.print(buffer[j]);
  }
  i++;
}

void display_test(){
  print_testnum();
  print_clock();
}

void print_testnum(){
  matrix.setCursor(i , 0);
  if (i > 64){ //screen is 64 pixels wide
    i = -96; //32 characters, 3 pixels a character
  }
  const GFXfont *pfont = &TomThumb;
  matrix.setFont(pfont);
  matrix.print("Test in progress");
  i++;
  matrix.setCursor(0,13);
  matrix.print("Number       of 150");
  matrix.setCursor(25,13);
  matrix.print(numberdata);
}

void print_clock(){
  if(x == 0){
    start = millis();
  }
  matrix.setCursor(4,30);
  long time_remaining = timedata - ((millis()-start)/1000);
  if(time_remaining <= 0){
    matrix.setTextColor(matrix.Color888(150,0,0));
    matrix.print("Testing complete");
  } else {
    const GFXfont *pfont = &FreeSerifBoldItalic9pt7b;
    matrix.setFont(pfont);
    matrix.print(
      String( time_remaining / 3600) +
      ":" + String((time_remaining / 60  )%60) + 
      ":" + String( time_remaining % 60  )   ); //keeps counting even is screen is switched.
  }
  x = 1;
}

void printTightDefault(const char *str, int Xstart, int Yline, uint16_t color)
{
  matrix.setFont(0);
  matrix.setTextColor(color);
  for (int i = 0; str[i]; i++) {
    matrix.setCursor(Xstart + (i * 5), Yline);
    matrix.print(str[i]);
  }
}

