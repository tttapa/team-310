#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "SSD1306.h"

const float R1 = 71000;
const float R2 = 9500;
const float ResRatio = R2 / (R1 + R2);

const float minVoltage = 2.4;
const float maxVoltage = 3.3;

const char *ssid         = "Team 310";
const char *password     = "pieter2016";

//const int8_t arrow[][2] = { { -2, -3}, { -2, -7}, { -6, -7}, {0, -13}, {6, -7}, {2, -7}, {2, -3}, { -2, -3}};
const int8_t arrow[][2] = { { -4, -5}, { -4, -13}, { -10, -13}, {0, -23}, {10, -13}, {4, -13}, {4, -5}, { -4, -5}};

const int8_t wifi[][2] = { {4, 0}, {4, 0} };

SSD1306  display(0x3c, 4, 5);

void setup() {
  delay(200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP ( ssid, password );

  display.init();
  display.flipScreenVertically();
  display.setContrast(255);

  ArduinoOTA.begin();
  ArduinoOTA.onStart([]() {
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.setFont(ArialMT_Plain_16);
    display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 - 15, "Uploading ...");
    display.drawProgressBar(3, DISPLAY_HEIGHT / 2 - 3, 122, 8, progress / (total / 100) );
    display.setFont(ArialMT_Plain_10);
    display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + 16, String(progress * 100 / total) + "%");
    display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + 26, String(progress) + "/" + String(total));
    display.display();
  });

  ArduinoOTA.onEnd([]() {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, "Rebooting ...");
    display.display();
  });

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
}

unsigned long refresh = 5;
unsigned long nextRefresh = millis();

void loop() {
  ArduinoOTA.handle();
  if (millis() > nextRefresh) {
    display.clear();
    float voltage = analogRead(A0) / ResRatio / 1024.0;
    display.drawString(DISPLAY_WIDTH / 2, 0, "Team 310");
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(DISPLAY_WIDTH, 0, String(voltage) + "V");
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    int batLevel = round(3.0 * (voltage - minVoltage) / (maxVoltage - minVoltage));
    drawBattery(0, 1, batLevel);
    
    drawArrow(1 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 2 + 8, 4 * voltage);

    drawMeter(3 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 2 + 8, 4 * voltage, 20);

    display.display();
    nextRefresh = millis() + refresh;
  }
}

void drawBattery(int16_t x, int16_t y, int level) {
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

void drawArrow(int16_t x, int16_t y, int orientation) {
  switch (orientation % 4) {
    case 0:
      /*display.drawLine(x - 2, y - 3, x + 2, y - 3);
        display.drawLine(x - 2, y - 3, x - 2, y - 7);
        display.drawLine(x + 2, y - 3, x + 2, y - 7);
        display.drawLine(x - 2, y - 7, x - 6, y - 7);
        display.drawLine(x + 2, y - 7, x + 6, y - 7);
        display.drawLine(x - 6, y - 7, x, y - 13);
        display.drawLine(x + 6 , y - 7, x, y - 13);*/
      for (int i = 0; i < sizeof(arrow) / 2 - 1; i++) {
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

void drawMeter(int16_t x, int16_t y, float value, int radius) {
  display.drawCircle(x,y,radius);
  display.fillCircle(x,y,radius-4);
}

