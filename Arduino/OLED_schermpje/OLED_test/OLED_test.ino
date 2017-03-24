#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "moon1.h"
#include "sun.h"
//#include "wifi3.h"
#include "wifi5.h"
#include "A.h"
#include "Auto.h"

#include "OLED.h"
#include "NetworkInit.h"

const float R1 = 71000;
const float R2 = 9500;
const float ResRatio = R2 / (R1 + R2);

const float minVoltage = 2.4;
const float maxVoltage = 3.27;

const char *ssid         = "Team 310";
const char *password     = "pieter2016";

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Setup");
  WiFi.mode(WIFI_AP);
  WiFi.softAP (ssid, password);

  display.init();
  display.flipScreenVertically();
  display.setContrast(255);

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);

  startOTA();
}

unsigned long refresh = 50;
unsigned long nextRefresh = millis();

unsigned long interval = 2000;
unsigned long nextInterval = millis();

int lights = 0;
int movement = 0;

void loop() {
  ArduinoOTA.handle();
  if (millis() > nextRefresh) {
    display.clear();
    float voltage = analogRead(A0) / ResRatio / 1024.0;
    display.drawString(DISPLAY_WIDTH / 2, 0, "HAL 9310");
    //display.setTextAlignment(TEXT_ALIGN_RIGHT);
    //display.drawString(DISPLAY_WIDTH, 0, String(voltage) + "V");
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    int batLevel = round(3.0 * (voltage - minVoltage) / (maxVoltage - minVoltage));
    drawBattery(0, 1, batLevel);

    if (WiFi.softAPgetStationNum() > 0) {
      display.drawXbm(106, 0, 11, 9, wifi5_bits);
    }

    if (lights == 0)
      display.drawXbm(119, 0, 9, 9, moon1_bits);
    else if (lights == 1)
      display.drawXbm(119, 0, 9, 9, sun_bits);
    else
      display.drawXbm(119, 0, 9, 9, A_bits);

    if (movement < 4)
      drawArrow(1 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 2 + 8, movement);
    else
      display.drawXbm(0, 13, Auto_width, Auto_height, Auto_bits);

    drawMeter(3 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 2 + 12, voltage / maxVoltage, 27);
    display.drawString(3 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT - 12, String(voltage * 3));

    display.display();
    nextRefresh = millis() + refresh;
  }

  if (millis() > nextInterval) {
    lights += 1;
    lights %= 3;
    movement += 1;
    movement %= 8;
    nextInterval = millis() + interval;
  }
}


