#define DEBUG_Serial Serial  // Use Serial1 for debugging, and Serial for communication with the ATmega
#define ATMEGA_Serial Serial

#define STATION // connect to a WiFi network, as well as creating an access point
#define DVD // use the DVD setting on the remote
//#define FLIP // flip the OLED display upside down

#include <ESP8266WiFi.h>

const char *APssid         = "HAL 9310";
const char *APpassword     = "pieter2001";
#ifdef STATION
#include "WiFiCredentials.h"
#endif

const char* WiFiHostname = "HAL-9310";      // Host name of the device
const char* dnsName = "HAL-9310";           // Domain name for the mDNS responder
const byte DNS_PORT = 53;                   // The port of the DNS server
IPAddress apIP(3, 1, 0, 1);                 // The IP address of the access point

const unsigned int localPort = 9310;        // local port to listen for UDP packets

const char *OTAName = "HAL 9310";           // A name and a password for the OTA service, to upload new firmware
const char *OTAPassword = "esp8266";

const float R1 = 68100;                     // values of the voltage divider to measure the battery voltage
const float R2 = 9900;
const float ResRatio = R2 / (R1 + R2);      // Elektrische netwerken ftw :P
const float voltageCalib = 1.08;            // Analog input voltage differs from actual voltage

const float minVoltage = 2.0;               // minimum battery voltage
const float maxVoltage = 6.5;               // voltage of fully charged battery

#include <WebSocketsServer.h>

// function prototype
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght);

#include "Images/moon1.h" // include some XBM images
#include "Images/sun.h"
#include "Images/wifi.h"
#include "Images/Auto.h"

#include "OLED.h" // some custom display functions
#include "NetworkInit.h" // Connect WiFi, start OTA, DNS, mDNS ...
#include "ServerInit.h" // HTTP server, WebSocket, server handlers ...

#include "Commands.h" // HEX codes of the IR remote

#define NB_OF_COMMANDS 9 // commands to send to the ATmega
const uint8_t commands[NB_OF_COMMANDS] = { FORWARD, BACKWARD, LEFT, RIGHT, BRAKE, SPEEDUP, SPEEDDOWN, CHG_STATION, MANUAL};

/*__________________________________________________________SETUP__________________________________________________________*/

void setup() {
  DEBUG_Serial.begin(115200);        // Start the Serial communication to send messages to the computer
  ATMEGA_Serial.begin(115200);              // Start the Serial communication with the ATmega328P
  delay(10);
  DEBUG_Serial.println("\r\nI read you, Dave.");

  startDisplay();              // initialise the display

  startOTA();                  // start the Over The Air update services

  startWiFi();                 // Start an access point, and connect to the network specified above

  startSPIFFS();               // Start the SPIFFS and list all contents

  startWebSocket();            // Start a WebSocket server

  startMDNS();                 // Start the mDNS responder

  startDNS();                  // Start the DNS server

  startServer();               // Start a HTTP server with a file read handler and an upload handler

  startUDP();                  // Start listening for UDP packets
}

/*__________________________________________________________LOOP__________________________________________________________*/

unsigned long refresh = 50; // refresh rate of the OLED display, value in milliseconds
unsigned long nextRefresh = millis();

int lights = 0;
int movement = 0;

void loop() {
  ArduinoOTA.handle();                        // check for over the air updates
  server.handleClient();                      // run the HTTP server
  webSocket.loop();                           // check for websocket events
  dnsServer.processNextRequest();             // process DNS requests

  if (millis() > nextRefresh) {
    drawAll();
  }

  checkUDP();
  printIP();
  delay(5);
}

/*__________________________________________________________WEBSOCKET__________________________________________________________*/

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { // When a WebSocket message is received
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      DEBUG_Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        DEBUG_Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:                     // if new text data is received
      DEBUG_Serial.printf("[%u] get Text: %s: \t", num, payload);
      int index = strtol((const char *) &payload[0], NULL, 16) % NB_OF_COMMANDS; // convert from HEX string to number
      DEBUG_Serial.println(commands[index], HEX);
      ATMEGA_Serial.write(commands[index] | (1 << 7));
      switch (commands[index]) {
        case FORWARD:
          movement = 0;
          break;
        case LEFT:
          movement = 1;
          break;
        case BACKWARD:
          movement = 2;
          break;
        case RIGHT:
          movement = 3;
          break;
        default:
          movement = 4;
          break;
      }
      break;
  }
}

/*__________________________________________________________UDP__________________________________________________________*/

void checkUDP() {
  unsigned int udpSize =  udp.parsePacket(); // Load the contents of the UDP packet into the buffer
  if (udpSize > 0) { // if there's new bytes available:
    lights += 1;
    lights %= 3;
    uint8_t data[1];
    udp.read(data, 1); // read 1 byte from the buffer
    switch (commands[data[0]]) {
      case FORWARD:
        movement = 0;
        break;
      case LEFT:
        movement = 1;
        break;
      case BACKWARD:
        movement = 2;
        break;
      case RIGHT:
        movement = 3;
        break;
      default:
        movement = 4;
        break;
    }
  }
}

/*__________________________________________________________DISPLAY__________________________________________________________*/

void drawAll() {
  display.clear(); // clear the frame buffer

  display.drawString(DISPLAY_WIDTH / 2, 0, "HAL 9310");                              // Print "HAL 9310" in the center of the top bar

  float voltage = analogRead(A0) * voltageCalib / ResRatio / 1024.0;                 // measure the voltage of the battery
  int batLevel = round(3.0 * (voltage - minVoltage) / (maxVoltage - minVoltage));    // convert voltage to battery level
  drawBattery(0, 1, batLevel);                                                       // show the battery level in the top bar

  if (WiFi.softAPgetStationNum() > 0 || WiFi.status() == WL_CONNECTED) {             // If there are stations connected to the access point
    display.drawXbm(106, 1, 11, 9, wifi_bits[3]);                                      // Show the Wi-Fi icon in the top bar
    display.drawString(122, 0, String(WiFi.softAPgetStationNum()));                    // print the number of stations that are connected directly to the ESP's AP
  }

  if (lights == 0)                                                                   // if the lights are off
    display.drawXbm(20, 1, 9, 9, moon1_bits);                                          // show a moon symbol in the top bar
  else if (lights == 1)                                                              // if the lights are on
    display.drawXbm(20, 1, 9, 9, sun_bits);                                            // show a sun symbol in the top bar
  else                                                                               // if the lights are in automatic mode
    display.drawString(20 + 4, 0, "A");                                                // show the letter A in the top bar

  if (movement < 4)
    drawArrow(1 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 2 + 8, movement);              // show an arrow indicating the direction of the movement
  else
    display.drawXbm(0, 13, Auto_width, Auto_height, Auto_bits);                      // Indicate that the wheelchair is navigating on autopilot

  drawMeter(3 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 2 + 12, voltage / maxVoltage, 27); // draw the speedometer (based on the voltage level)
  display.drawString(3 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT - 12, String(voltage));     // draw the number under the speed gauge

  display.display(); // send the frame buffer to the display
  nextRefresh = millis() + refresh;
}

/*__________________________________________________________MISC__________________________________________________________*/

void printIP() {
  static boolean printed = false;
  if (WiFi.status() != WL_DISCONNECTED) {
    if (printed)
      return;
    DEBUG_Serial.println("Connected");
    DEBUG_Serial.print("IP address:\t");
    DEBUG_Serial.println(WiFi.localIP());
    startMDNS();
    printed = true;
  } else {
    printed = false;
  }
}