/*
   * gradient.ino
   * Source code by: Chris Shoemaker
   * Code adopted from: Yanshan Guo (2015)
   * Last version updated on: 7/5/2016
   * Drives an Adafruit RGB LED Matrix panel
   * Displays gradient ranges between primary and secondary colors
   
   * (For reference) The hue values for HSV (range of 0 to 1535, looping back to 0 if greater than 1535) and corresponding primary/secondary colors are as follows:
    * 0 -- 255 = red
    * 256 -- 511 = yellow
    * 512 -- 767 = green
    * 768 -- 1023 = cyan
    * 1024 -- 1279 = blue
    * 1280 -- 1535 = magenta
*/

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

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

#define SCREEN_UP_TIME 5
#define R_HSV 0
#define G_HSV 512
#define B_HSV 1024

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

void setup()
{
  // Setup host communication

  // Setup RGB LED Matrix
  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge
  matrix.setTextSize(1);

  // remove serial port debugger when releasing
  Serial.begin(9600);
}

void loop()
{
  unsigned long time_passed = millis();
  // Clear background
  matrix.fillScreen(0);
  matrix.setFont(0);

  // Display changes in each of the three HSV variables for red, green, and blue
  if ((time_passed / 1000) % (3 * SCREEN_UP_TIME) <= SCREEN_UP_TIME) {
    displayHSV(R_HSV);
  } else if ((time_passed / 1000) % (3 * SCREEN_UP_TIME) > SCREEN_UP_TIME && (time_passed / 1000) % (3 * SCREEN_UP_TIME) <= 2 * SCREEN_UP_TIME) {
    displayHSV(G_HSV);
  } else {
    displayHSV(B_HSV);
  }

  // Display change in brightness for primary and secondary colors (using the Color888 function)
//  if ((time_passed / 1000) % (4 * SCREEN_UP_TIME) <= SCREEN_UP_TIME) {
//    displayGradientRYG();
//  } else if ((time_passed / 1000) % (4 * SCREEN_UP_TIME) > SCREEN_UP_TIME && (time_passed / 1000) % (4 * SCREEN_UP_TIME) <= 2 * SCREEN_UP_TIME) {
//    displayGradientRYG2();
//  } else if ((time_passed / 1000) % (4 * SCREEN_UP_TIME) > 2 * SCREEN_UP_TIME && (time_passed / 1000) % (4 * SCREEN_UP_TIME) <= 3 * SCREEN_UP_TIME) {
//    displayGradientCBM();
//  } else {
//    displayGradientCBM2();
//  }

  // Display shift in between adjacent primary and secondary colors (using the Color888 function)
//  if ((time_passed / 1000) % (6 * SCREEN_UP_TIME) <= SCREEN_UP_TIME) {
//    displayRedToYellow();
//  } else if ((time_passed / 1000) % (6 * SCREEN_UP_TIME) > SCREEN_UP_TIME && (time_passed / 1000) % (6 * SCREEN_UP_TIME) <= 2 * SCREEN_UP_TIME) {
//    displayYellowToGreen();
//  } else if ((time_passed / 1000) % (6 * SCREEN_UP_TIME) > 2 * SCREEN_UP_TIME && (time_passed / 1000) % (6 * SCREEN_UP_TIME) <= 3 * SCREEN_UP_TIME) {
//    displayGreenToCyan();
//  } else if ((time_passed / 1000) % (6 * SCREEN_UP_TIME) > 3 * SCREEN_UP_TIME && (time_passed / 1000) % (6 * SCREEN_UP_TIME) <= 4 * SCREEN_UP_TIME) {
//    displayCyanToBlue();
//  } else if ((time_passed / 1000) % (6 * SCREEN_UP_TIME) > 4 * SCREEN_UP_TIME && (time_passed / 1000) % (6 * SCREEN_UP_TIME) <= 5 * SCREEN_UP_TIME) {
//    displayBlueToMagenta();
//  } else {
//    displayMagentaToRed();
//  }

  matrix.swapBuffers(true);
}

// Increases hue, saturation, and brightness HSV values separately by 4
void displayHSV(int hsv) {
  for (int i = 0; i < 256; i += 4) {
    matrix.fillRect((i / 4), 0, 1, 8, matrix.ColorHSV(hsv + i, 255, 255, true));
    matrix.fillRect((i / 4), 9, 1, 8, matrix.ColorHSV(hsv, hsv + i, 255, true));
    matrix.fillRect((i / 4), 18, 1, 8, matrix.ColorHSV(hsv, 255, hsv + i, true));
  }
}

void displayGradientRYG() {
  for (int i = 0; i < 256; i += 4) {
    matrix.fillRect((i / 4), 0, 1, 8, matrix.Color888(255 - i, 0, 0));
    matrix.fillRect((i / 4), 9, 1, 8, matrix.Color888(255 - i, 255 - i, 0));
    matrix.fillRect((i / 4), 18, 1, 8, matrix.Color888(0, 255 - i, 0));
  }
}

void displayGradientRYG2() {
  for (int i = 0; i < 128; i += 2) {
    matrix.fillRect((i / 2), 0, 1, 8, matrix.Color888(127 - i, 0, 0));
    matrix.fillRect((i / 2), 9, 1, 8, matrix.Color888(127 - i, 127 - i, 0));
    matrix.fillRect((i / 2), 18, 1, 8, matrix.Color888(0, 127 - i, 0));
  }
}

void displayGradientCBM() {
  for (int i = 0; i < 256; i += 4) {
    matrix.fillRect((i / 4), 0, 1, 8, matrix.Color888(0, 255 - i, 255 - i));
    matrix.fillRect((i / 4), 9, 1, 8, matrix.Color888(0, 0, 255 - i));
    matrix.fillRect((i / 4), 18, 1, 8, matrix.Color888(255 - i, 0, 255 - i));
  }
}

void displayGradientCBM2() {
  for (int i = 0; i < 128; i += 2) {
    matrix.fillRect((i / 2), 0, 1, 8, matrix.Color888(0, 127 - i, 127 - i));
    matrix.fillRect((i / 2), 9, 1, 8, matrix.Color888(0, 0, 127 - i));
    matrix.fillRect((i / 2), 18, 1, 8, matrix.Color888(127 - i, 0, 127 - i));
  }
}

/***BEGIN PRIMARY-SECONDARY COLOR SHIFTS (USING Color888)***/

// Displays color gradient from red to yellow
void displayRedToYellow() {
  for (int i = 0; i <= 255; i += 4) {
    matrix.fillRect((i / 4), 0, 1, 8, matrix.Color888(255, i, 0));
  }
}
// Displays color gradient from yellow to green
void displayYellowToGreen() {
  for (int i = 0; i <= 255; i += 4) {
    matrix.fillRect((i / 4), 8, 1, 8, matrix.Color888(255 - i, 255, 0));
  }
}

// Displays color gradient from green to cyan
void displayGreenToCyan() {
  for (int i = 0; i <= 255; i += 4) {
    matrix.fillRect((i / 4), 16, 1, 8, matrix.Color888(0, 255, i));
  }
}

// Displays color gradient from cyan to blue
void displayCyanToBlue() {
  for (int i = 0; i <= 255; i += 4) {
    matrix.fillRect((i / 4), 24, 1, 8, matrix.Color888(0, 255 - i, 255));
  }
}

// Displays color gradient from blue to magenta
void displayBlueToMagenta() {
  for (int i = 0; i <= 255; i += 4) {
    matrix.fillRect((i / 4), 0, 1, 8, matrix.Color888(i, 0, 255));
  }
}

// Displays color gradient from magenta to red
void displayMagentaToRed() {
  for (int i = 0; i <= 255; i += 4) {
    matrix.fillRect((i / 4), 8, 1, 8, matrix.Color888(255, 0, 255 - i));
  }
}

/***END PRIMARY-SECONDARY COLOR SHIFTS***/

