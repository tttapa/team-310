#define DEBUG_Serial Serial1

#define STATION

const char *APssid         = "Team 310";
const char *APpassword     = "pieter2016";
#ifdef STATION
#include "WiFiCredentials.h"
#endif

#include "Images/moon1.h" // include some XBM images
#include "Images/sun.h"
//#include "Images/wifi3.h"
//#include "Images/wifi5.h"
#include "Images/wifi.h"
#include "Images/A.h"
#include "Images/Auto.h"

#include "OLED.h" // some custom display functions           
#include "NetworkInit.h"

const float R1 = 71000; // values of the voltage divider to measure the battery voltage
const float R2 = 9500;
const float ResRatio = R2 / (R1 + R2);

const float minVoltage = 2.4; // minimum battery voltage
const float maxVoltage = 3.27; // voltage of fully charged battery

void setup() {
  DEBUG_Serial.begin(115200);
  delay(20);
  DEBUG_Serial.println("\n\r\nSetup");

  startDisplay();

  startOTA(); // start the Over The Air update services

  startWiFi();  

}

unsigned long refresh = 50;
unsigned long nextRefresh = millis();

unsigned long interval = 2000;
unsigned long nextInterval = millis();

int lights = 0;
int movement = 0;

void loop() {
  ArduinoOTA.handle(); // check for over the air updates

  if (millis() > nextRefresh) {
    display.clear(); // clear the frame buffer

    display.drawString(DISPLAY_WIDTH / 2, 0, "HAL 9310");

    float voltage = analogRead(A0) / ResRatio / 1024.0;
    //display.setTextAlignment(TEXT_ALIGN_RIGHT);
    //display.drawString(DISPLAY_WIDTH, 0, String(voltage) + "V");
    //display.setTextAlignment(TEXT_ALIGN_CENTER);
    int batLevel = round(3.0 * (voltage - minVoltage) / (maxVoltage - minVoltage)); // convert voltage to battery level
    drawBattery(0, 1, batLevel); // show the battery level in the top bar

    if (WiFi.softAPgetStationNum() > 0) { // If there are stations connected to the access point
      display.drawXbm(106, 0, 11, 9, wifi_bits[3]); // Show the Wi-Fi icon in the top bar
    }

    if (lights == 0) // if the lights are off
      display.drawXbm(119, 0, 9, 9, moon1_bits); // show a moon symbol in the top bar
    else if (lights == 1) // if the lights are on
      display.drawXbm(119, 0, 9, 9, sun_bits); // show a sun symbol in the top bar
    else // if the lights are in automatic mode
      display.drawXbm(119, 0, 9, 9, A_bits); // show the letter A in the top bar

    if (movement < 4)
      drawArrow(1 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 2 + 8, movement); // show an arrow indicating the direction of the movement
    else
      display.drawXbm(0, 13, Auto_width, Auto_height, Auto_bits); // Indicate that the wheelchair is navigating on autopilot

    drawMeter(3 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 2 + 12, voltage / maxVoltage, 27); // draw the speedometer (based on the voltage level)
    display.drawString(3 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT - 12, String(voltage)); // draw the number under the speed gauge

    display.display(); // send the frame buffer to the display
    nextRefresh = millis() + refresh;
  }

  if (millis() > nextInterval) { // increment some of the variables to test the different states and symbols
    lights += 1;
    lights %= 3;
    movement += 1;
    movement %= 8;
    nextInterval = millis() + interval;
  }
}


