#include "SSD1306.h"

const int8_t arrow[][2] = { { -4, -5}, { -4, -13}, { -10, -13}, {0, -23}, {10, -13}, {4, -13}, {4, -5}, { -4, -5}}; // a list of points of the arrow icon

//const char wifi[] = { 0x00, 0x00, 0xfe, 0x00, 0x01, 0x01, 0x7c, 0x00, 0x82, 0x00, 0x38, 0x00, 0x44, 0x00, 0x00, 0x00, 0x10, 0x00 };
//const char wifi2[] = { 0x00, 0x00, 0x38, 0x00, 0xc6, 0x00, 0x11, 0x01, 0x6c, 0x00, 0x82, 0x00, 0x38, 0x00, 0x44, 0x00, 0x10, 0x00 };
//const char wifi3[] = { 0x38, 0x00, 0xc6, 0x00, 0x11, 0x01, 0x6c, 0x00, 0x82, 0x00, 0x38, 0x00, 0x44, 0x00, 0x00, 0x00, 0x10, 0x00 };

SSD1306  display(0x3c, 4, 5); // The OLED display is connected to the ESP8266 via I²C: GPIO 4 = SDA, GPIO 5 = SCL

// function prototypes
void startDisplay();
void drawGoodCircle(int16_t x0, int16_t y0, int radius);
void drawBattery(int16_t x, int16_t y, int level);
void drawArrow(int16_t x, int16_t y, int orientation);
void drawMeter(int16_t x, int16_t y, float value, int radius);
void drawGoodCircle(int16_t x0, int16_t y0, int radius);
void fillGoodCircle(int16_t x0, int16_t y0, int radius);

void startDisplay() { // initialise the display
  display.init();
#ifdef FLIP
  display.flipScreenVertically();
#endif
  display.setContrast(255);

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
}

void drawBattery(int16_t x, int16_t y, int level) { // draws a battery indicator
  display.drawLine(x + 1, y, x + 13, y);
  display.drawLine(x + 1, y + 8, x + 13, y + 8);
  display.drawLine(x, y + 1, x, y + 7);
  display.drawLine(x + 14, y + 1, x + 14, y + 7);
  display.drawLine(x + 15, y + 3, x + 15, y + 5);
  //display.drawLine(x+16, y+3, x+16, y+5);
  if (level > 3)
    level = 3;
  for (int i = 0; i < level; i++) {
    display.fillRect(x + 2 + 4 * i, y + 2, 3, 5);
  }
}

void drawArrow(int16_t x, int16_t y, int orientation) { // draws an arrow to the display, with a given orientation (0 = top, 1 = left, 2 = bottom, 3 = right)
  switch (orientation % 4) {
    case 0:
      for (int i = 0; i < sizeof(arrow) / 2 - 1; i++) { // arrow is a two-dimensional array, and the second dimension is 2 bytes, so the first dimension is the total number of bytes divided by the 2 bytes of the second dimension
        display.drawLine(x + arrow[i][0], y + arrow[i][1], x + arrow[i + 1][0], y + arrow[i + 1][1]);
      }
      break;
    case 3:
      for (int i = 0; i < sizeof(arrow) / 2 - 1; i++) {
        display.drawLine(x + arrow[i][1], y + arrow[i][0], x + arrow[i + 1][1], y + arrow[i + 1][0]);
      }
      break;
    case 2:
      for (int i = 0; i < sizeof(arrow) / 2 - 1; i++) {
        display.drawLine(x + arrow[i][0], y - arrow[i][1], x + arrow[i + 1][0], y - arrow[i + 1][1]);
      }
      break;
    case 1:
      for (int i = 0; i < sizeof(arrow) / 2 - 1; i++) {
        display.drawLine(x - arrow[i][1], y + arrow[i][0], x - arrow[i + 1][1], y + arrow[i + 1][0]);
      }
      break;
  }
}

void drawMeter(int16_t x, int16_t y, float value, int radius) { // draws a speed gauge to the display
  //display.drawCircle(x, y, radius); // I didn't like the circles created by the SSD1306 lib, so I used custom circle drawing functions
  //display.fillCircle(x + 1, y, radius - 2);
  drawGoodCircle(x, y, radius);
  fillGoodCircle(x, y, radius - 3);
  display.setColor(BLACK);
  display.drawLine(x, y, x + (radius-7)*cos(value*PI + PI), y + (radius-7)*sin(value*PI + PI));
  fillGoodCircle(x, y + radius*1.6, radius*1.4);
  display.setColor(WHITE);
}

void drawGoodCircle(int16_t x0, int16_t y0, int radius) { // draws the outline of a circle
  // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    display.setPixel(x0 + x, y0 + y);
    display.setPixel(x0 + y, y0 + x);
    display.setPixel(x0 - y, y0 + x);
    display.setPixel(x0 - x, y0 + y);
    display.setPixel(x0 - x, y0 - y);
    display.setPixel(x0 - y, y0 - x);
    display.setPixel(x0 + y, y0 - x);
    display.setPixel(x0 + x, y0 - y);

    if (err <= 0) {
      y++;
      err += 2 * y + 1;
    }
    if (err > 0) {
      x--;
      err -= 2 * x + 1;
    }
  }
}

void fillGoodCircle(int16_t x0, int16_t y0, int radius) { // draws a solid, filled circle
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    display.drawLine(x0 + x, y0 + y, x0 - x, y0 + y);
    display.drawLine(x0 + y, y0 + x, x0 - y, y0 + x);
    display.drawLine(x0 + y, y0 - x, x0 - y, y0 - x);
    display.drawLine(x0 + x, y0 - y, x0 - x, y0 - y);

    if (err <= 0) {
      y++;
      err += 2 * y + 1;
    }
    if (err > 0) {
      x--;
      err -= 2 * x + 1;
    }
  }
}
