// ProgressDisplay
// Drives an Adafruit RGB LED Matrix panel for ORVALMA
// Displays public messages from the ORVALMA host
// Displays diagnostics from the ORVALMA uCtrlr
// Communicates as a slave with ORVALMA uCtrlr via 2Wire I2C
// Runs only on an Arduino Mega compatible micro-controller

// Utilizes double-buffered scrolling on Adafruit's 64x32 RGB LED matrix:
// http://www.adafruit.com/products/2279  3mm pitch
// http://www.adafruit.com/products/2278  4mm pitch
// http://www.adafruit.com/products/2277  5mm pitch
// http://www.adafruit.com/products/2276  6mm pitch

// Parts of this program were derived from examples
// written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <Fonts\TomThumb.h>
#include <Fonts\FreeSans9pt7b.h>
#include <Fonts\FreeSerif9pt7b.h>
#include <Wire.h>  // 2Wire/I2C implementation

// This is coded for a Mega implementation
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#else
  setoffalarm now
#endif

// Comment this define out to use 16-line panel
#define Use32linePanel 1

// These are the pins being used
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

// Last parameter = 'true' enables double-buffering, for flicker-free,
// buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
// until the first call to swapBuffers().  This is normal.

#ifdef Use32linePanel
// Constuctor for 64x32 panel
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true, 64);
#else
// Constuctor for 32x16 panel
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);
#endif

String displayMessage = String("Test No.100");
int len = displayMessage.length()+1;
int textX   = matrix.width(),
    textMin = (len+1) * -6,
    hue     = 0;

int testNum = 1;

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

void loop()
{
  byte i;

  // Clear background
  matrix.fillScreen(0);
  matrix.setFont(0);

  if (testNum == 3){
    breakLine();
  }
  else
  {
    if (testNum>=5) {
      diagnosticDisplay();
      if (testNum>6)
        testNum = 1;
    } else {
    // Draw big scrolly text on top
    matrix.setTextColor(matrix.Color888(100,0,100));
    matrix.setCursor(textX, 0);

    char st[len];
    displayMessage.toCharArray(st,len);
    for (int i = 0; i< len; i++){
      if (i == 10){
        char num[1];
        String d = String(testNum);
        d.toCharArray(num,1);
        matrix.print(d[0]);
      }
      else
      {
        matrix.print(st[i]);
      }
    }
    testingLine();
    nameLine();
    }
  }

  // Move text left (w/wrap), increase hue
  if (--textX < textMin) {
    textX = matrix.width();
    testNum +=1;
  }
  Serial.println(textX);

  // Update display
  matrix.swapBuffers(true);
  delay(50);
}

void receiveEvent(int numBytes) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}


void diagnosticDisplay()
{
  matrix.setFont(0);
  matrix.setCursor(0,0);
  uint16_t grey = matrix.Color888(16,16,16);
  uint16_t bold = matrix.Color888(129,129,129);
  // dim green, bright red, med green, dim red,
  // med red, med blue, med yellow, med magneta
  // med cyan
  uint16_t dimgrn = matrix.Color888(0,16,0);
  uint16_t medgrn = matrix.Color888(0,97,0);
  uint16_t brtgrn = matrix.Color888(0,129,0);
  uint16_t dimred = matrix.Color888(16,0,0);
  uint16_t medred = matrix.Color888(97,0,0);
  uint16_t brtred = matrix.Color888(129,0,0);
  uint16_t dimblu = matrix.Color888(0,0,16);
  uint16_t medblu = matrix.Color888(0,0,97);
  uint16_t brtblu = matrix.Color888(0,0,255);
  uint16_t medmgt = matrix.Color888(65,0,65);
  uint16_t medylw = matrix.Color888(65,65,0);
  uint16_t medcyn = matrix.Color888(0,65,65);

  matrix.setTextColor(grey);
  // BIG STUFF = top 8 indicators
  // ESP  Hardware ready | E-Stop
  // Dim Green vs Bright Red
  printTightDefault("ESP", 0, 0, dimgrn);
  // DRO:1;  // Door open, 0=closed, 1=open
  // Dim Green vs Bright Red
  printTightDefault("DRO", 16, 0, dimred);
  // SBT:1;  // Seat Belt, 1=latched
  // Med Green vs Dim Red
  printTightDefault("SBT", 32, 0, brtred);
  // MBK:1;  // Mechanical Brake, 1=engaged
  // Dim Green vs Bright Red
  printTightDefault("MBK", 48, 0, medred);

  // TCR:2;  // Turn-Counter: 0=unknown, 1=low-turn, 2=mid-turn, 3=max-turn
  // Dim TC with Red/White/Blue number in bright, dim 'R' if unknown
  printTightDefault("TCR", 0, 8, medgrn);
  // R2R:1;  // HOST:Software ready: 1=ready to run
  // grey vs Yellow
  printTightDefault("R2R", 16, 8, dimblu);
  // SRO:1;  // HOST:Servo On: 1=on
  // grey vs Magenta
  printTightDefault("SRO", 32, 8, medblu);
  // IMN:1;  // HOST:In Motion: 1=moving
  // grey vs Blue
  printTightDefault("IMN", 48, 8, brtblu);

  // R2E:1;  // Request-To-Enter: 1=pressed
  // RES:2;  // Request-To-Enter Status: bit0=requested, bit1=diagnostics
  // grey vs Blue (pressed), Cyan (requested), Yellow (diagnostics)
  printTightDefault("R2E", 0, 16, brtgrn);
  // CMT:1;  // HOST:Commutate
  // CMH:1;  // Commutate Position: 1=at-home
  // grey vs Red (on-not-home), Blue (on-at-home)
  printTightDefault("CMT", 16, 16, medcyn);
  // MOS:1;  // Manual Override Switch, 1=on
  // MOR:1;  // HOST:ManualOverride: 1=enabled
  // grey vs Med Magenta (switched), Bright Red (enabled)
  printTightDefault("MOS", 32, 16, medmgt);

  // CVT  Control-Cabinet 24V Power
  // Just paint a red dot or 24V in TomThumb
  printTightDefault("24V", 0, 24, dimred);

  // SLT:3;  // StopLight: Red, Yellow, Green
  printTightDefault("RYG", 16, 24, medylw);

  // multi-bit fields
  // TCV:10;  // Turn-Counter Value 0-1023
  printTightDefault("TC", 32, 24, grey);
  matrix.setCursor(41,24);
  matrix.print(':');
  printTightDefault("1234", 44, 24, grey);

}

void printTightDefault(const char *str, int Xstart, int Yline, uint16_t color)
{
  matrix.setFont(0);
  matrix.setTextColor(color);
  for (int i=0; str[i]; i++) {
    matrix.setCursor(Xstart+(i*5),Yline);
    matrix.print(str[i]);
  }
}

void breakLine()
{
  const GFXfont *pfont = &FreeSans9pt7b;
  matrix.setFont(pfont);
  matrix.setCursor(2,16);
  matrix.setTextColor(matrix.Color888(0,100,100));
  matrix.print('B');
  matrix.print('r');
  matrix.print('e');
  matrix.print('a');
  matrix.print('k');
}


void testingLine()
{
  const GFXfont *pfont = &FreeSerif9pt7b;
  matrix.setFont(pfont);
  matrix.setTextColor(matrix.Color888(200,0,0));
  matrix.setCursor(4,16);
  matrix.print("Testink");
  // matrix.print('T');
  // matrix.print('e');
  // matrix.print('s');
  // matrix.print('t');
  // matrix.print('i');
  // matrix.print('n');
  // matrix.print('g');
}

void nameLine()
{
  const GFXfont *pfont = &TomThumb;
  matrix.setFont(pfont);
  int line = 32-(1*pfont->yAdvance+1);
  matrix.setTextColor(matrix.Color888(0,120,0));
  matrix.setCursor(0,line);
  matrix.print('R');
  matrix.print('a');
  matrix.print('v');
  matrix.print('i');
  matrix.print('s');
  matrix.print('h');
  matrix.setTextColor(matrix.Color888(40,40,40));
  matrix.setCursor(49,line);
  matrix.print('R');
  matrix.print('y');
  matrix.print('a');
  matrix.print('n');
  line += pfont->yAdvance;
  matrix.setTextColor(matrix.Color888(0,0,120));
  matrix.setCursor(6,line);
  matrix.print('T');
  matrix.print('o');
  matrix.print('m');
  matrix.setTextColor(matrix.Color888(100,100,0));
  matrix.setCursor(33,line);
  matrix.print('L');
  matrix.print('i');
  matrix.print('n');
  matrix.print('d');
  matrix.print('s');
  matrix.print('e');
  matrix.print('y');
}
