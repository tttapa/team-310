#define DEBUG_Serial Serial1  // Use Serial1 for debugging, and Serial for communication with the ATmega
#define ATMEGA_Serial Serial

//#define DEBUG_ESP_PORT Serial1

//#define STATION // connect to a WiFi network, as well as creating an access point
#define DVD // use the DVD setting on the remote
//#define FLIP // flip the OLED display upside down

#include <ESP8266WiFi.h>

const char *APssid         = "HAL 9310";
const char *APpassword     = "pieter2001";
#ifdef STATION
#include "WiFiCredentials.h"
#endif

const char* WiFiHostname = "hal-9310";      // Host name of the device
const char* dnsName = "hal-9310";           // Domain name for the mDNS responder
const byte DNS_PORT = 53;                   // The port of the DNS server
IPAddress apIP(3, 1, 0, 1);                 // The IP address of the access point

const unsigned int localPort = 9310;        // local port to listen for UDP packets

const char *OTAName = "HAL 9310";           // A name and a password for the OTA service, to upload new firmware
const char *OTAPassword = "espetss";

const float R1 = 68100;                     // values of the voltage divider to measure the battery voltage
const float R2 = 9900;
const float ResRatio = R2 / (R1 + R2);      // Elektrische netwerken ftw :P
const float voltageCalib = 1.08;            // Analog input voltage differs from actual voltage

const float minVoltage = 5.0;               // minimum battery voltage
const float maxVoltage = 7.5;               // voltage of fully charged battery

const unsigned long remote_timeout = 200;

#include <WebSocketsServer.h>

// function prototype
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght);

#include "Images/moon1.h" // include some XBM images
#include "Images/sun.h"
#include "Images/wifi.h"
#include "Images/Auto.h"
#include "Images/reboot.h"

#include "OLED.h" // some custom display functions
#include "NetworkInit.h" // Connect WiFi, start OTA, DNS, mDNS ...
#include "ServerInit.h" // HTTP server, WebSocket, server handlers ...

#include "Commands.h" // HEX codes of the IR remote

/*__________________________________________________________SETUP__________________________________________________________*/

void setup() {
  DEBUG_Serial.begin(115200);        // Start the Serial communication to send messages to the computer
  ATMEGA_Serial.begin(115200);              // Start the Serial communication with the ATmega328P
  delay(10);
  DEBUG_Serial.println("\r\n\r\nI read you, Dave.\r\n");

  startDisplay();              // initialise the display

  startWiFi();                 // Start an access point, and connect to the network specified above

  startOTA();                  // start the Over The Air update services

  startDNS();                  // Start the DNS server

  startSPIFFS();               // Start the SPIFFS and list all contents

  startWebSocket();            // Start a WebSocket server

  startServer();               // Start a HTTP server with a file read handler and an upload handler

  startUDP();                  // Start listening for UDP packets
}

/*__________________________________________________________LOOP__________________________________________________________*/

IPAddress UDP_remoteIP;
int UDP_remotePort = 0;

const unsigned long UDP_interval = 500;
unsigned long nextUDP = millis();

const unsigned long refresh = 50; // refresh rate of the OLED display, value in milliseconds (50 ms → 20 fps
unsigned long nextRefresh = millis();

int lights = 2;
int movement = 0;
int speed = 0;
float actualSpeed = 0;
boolean autopilot = false;

void loop() {
  ArduinoOTA.handle();                        // check for over the air updates
  server.handleClient();                      // run the HTTP server
  webSocket.loop();                           // check for websocket events
  dnsServer.processNextRequest();             // process DNS requests
  checkSerial();                              // Check if Serial data from the ATmega has arrived

  if (millis() > nextRefresh) {
    drawAll();
  }

  if (millis() > nextUDP) {
    sendUDP();
    sendWebSocket();
    nextUDP = millis() + UDP_interval;
  }

  checkUDP();
#ifdef STATION
  printIP();
#endif
  printStations();
}

/*__________________________________________________________WEBSOCKET__________________________________________________________*/

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) { // When a WebSocket message is received
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      DEBUG_Serial.printf("[%u] Disconnected!\r\n", num);
      ATMEGA_Serial.write(BRAKE + 0x80);
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        DEBUG_Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:                     // if new text data is received
      DEBUG_Serial.printf("[%u] get Text: %s: \t", num, payload);
      int cmd = strtol((const char *) payload, NULL, 16); // convert from HEX string to number
      DEBUG_Serial.println(cmd, HEX);
      ATMEGA_Serial.write(cmd | (1 << 7));
      if (length > 4) { // there's a data byte as well
        int cmd = strtol((const char *) &payload[5], NULL, 16); // convert from HEX string to number
      }

      break;
  }
}

void sendWebSocket() {
  // send UDP packet with battery information back to client
  float voltage = analogRead(A0) * voltageCalib / ResRatio / 1024.0;                 // measure the voltage of the battery
  voltage = voltage > minVoltage ? voltage - minVoltage : 0.0;                       // convert to a 7-bit number relative to the minimum and maximum voltage
  uint8_t voltageCode = voltage < maxVoltage ? voltage * 127 / maxVoltage : 127;
  //udp.beginPacket(UDP_remoteIP, UDP_remotePort);
  //udp.write(voltageCode);
  //udp.endPacket();
  char voltage_str[9];
  generateHexStr(BATVOLTAGE, voltageCode, voltage_str);
  webSocket.broadcastTXT(voltage_str, 9);
  DEBUG_Serial.write(voltage_str, 9);
  DEBUG_Serial.println();
  DEBUG_Serial.printf("%d\t%d\t%d\r\n", voltageCode, int(voltage*1000), analogRead(A0));

}

/*__________________________________________________________UDP__________________________________________________________*/

void checkUDP() {
  unsigned int udpSize =  udp.parsePacket(); // Load the contents of the UDP packet into the buffer
  if (udpSize > 0) { // if there's new bytes available:
    uint8_t data[1];
    udp.read(data, 1); // read 1 byte from the buffer
    ATMEGA_Serial.write(data[0] | (1 << 7));

    UDP_remoteIP = udp.remoteIP();
    UDP_remotePort = udp.remotePort();
  }
}

void sendUDP() {
  if (UDP_remotePort) {
    // send UDP packet with battery information back to client
    float voltage = analogRead(A0) * voltageCalib / ResRatio / 1024.0;                 // measure the voltage of the battery
    voltage = voltage > minVoltage ? voltage - minVoltage : 0.0;                       // convert to a 7-bit number relative to the minimum and maximum voltage
    uint8_t voltageCode = voltage < maxVoltage ? voltage * 127 / maxVoltage : 127;
    udp.beginPacket(UDP_remoteIP, UDP_remotePort);
    udp.write(voltageCode);
    udp.endPacket();
  }
}

/*__________________________________________________________SERIAL__________________________________________________________*/

uint8_t serialMessage[2];
boolean messageDone = false;

void checkSerial() {
  if (ATMEGA_Serial.available() > 0) {
    uint8_t data = ATMEGA_Serial.read();
    DEBUG_Serial.print("Serial data:\t0x");
    DEBUG_Serial.println(data & ~(1 << 7), HEX);
    if (data >> 7) {
      serialMessage[0] = data;
      uint8_t cmd = data & ~(1 << 7);
      if ((cmd != SETSPEED) && (cmd != actualSpeed)) { // if it's not a speed message, the packet is only one byte long
        messageDone = true;
      }
    } else {
      serialMessage[1] = data;
      messageDone = true;
    }
  }
  if (messageDone) {
    DEBUG_Serial.printf("Data 1: %02X\tData 2: %02X\r\n", serialMessage[0], serialMessage[1]);
    handleSerial();
    messageDone = false;
  }
}

/*__________________________________________________________DISPLAY__________________________________________________________*/

int wififrame = 0;
unsigned long nextWiFiFrame = millis();
unsigned long frameTime = 250;

void drawAll() {
  display.clear(); // clear the frame buffer

  display.drawString(DISPLAY_WIDTH / 2, 0, "HAL 9310");                              // Print "HAL 9310" in the center of the top bar

  float voltage = analogRead(A0) * voltageCalib / ResRatio / 1024.0;                 // measure the voltage of the battery
  int batLevel = round(3.0 * (voltage - minVoltage) / (maxVoltage - minVoltage));    // convert voltage to battery level
  drawBattery(0, 1, batLevel);                                                       // show the battery level in the top bar
#ifdef STATION
  if (WiFi.status() == WL_CONNECTED) {                                               // If there are stations connected to the access point
    display.drawXbm(106, 1, 11, 9, wifi_bits[round(3.0 * getQuality() / 100) % 4]);    // Show the Wi-Fi icon in the top bar
  } else {                                                                           // Wi-Fi not connected
    if (millis() > nextWiFiFrame) {
      wififrame++;
      wififrame %= 4;
      nextWiFiFrame = millis() + frameTime;
    }
    display.drawXbm(106, 1, 11, 9, wifi_bits[wififrame]); // Show the Wi-Fi icon in the top bar
  }
#endif
  if (WiFi.softAPgetStationNum())
    display.drawString(122, 0, String(WiFi.softAPgetStationNum()));                  // print the number of stations that are connected directly to the ESP's AP

  if (lights == 0)                                                                   // if the lights are off
    display.drawXbm(20, 1, 9, 9, moon1_bits);                                          // show a moon symbol in the top bar
  else if (lights == 1)                                                              // if the lights are on
    display.drawXbm(20, 1, 9, 9, sun_bits);                                            // show a sun symbol in the top bar
  else                                                                               // if the lights are in automatic mode
    display.drawString(20 + 4, 0, "A");                                                // show the letter A in the top bar

  if (movement == 0) {
    display.drawString(DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 2 + 10, "BRAKE");
  } else if (movement <= 4) {
    drawArrow(1 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 2 + 8, movement - 1);          // show an arrow indicating the direction of the movement
  } else {
    display.drawXbm(0, 13, Auto_width, Auto_height, Auto_bits);                      // Indicate that the wheelchair is navigating on autopilot
  }

  drawMeter(3 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 2 + 12, actualSpeed, 27);  // draw the speedometer (based on the WiFi signal level)
  display.drawString(3 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT - 12, String(actualSpeed)); // draw the number under the speed gauge

  display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 12, String(speed));

  display.display(); // send the frame buffer to the display
  nextRefresh = millis() + refresh;
}

/*__________________________________________________________MISC__________________________________________________________*/

#ifdef STATION
void printIP() {
  static boolean printed = false;
  if (WiFi.status() == WL_CONNECTED) {
    if (printed)
      return;
    DEBUG_Serial.println("Connected");
    DEBUG_Serial.print("IP address:\t");
    DEBUG_Serial.println(WiFi.localIP());
    startMDNS();                 // Start the mDNS responder

    printed = true;
  } else {
    printed = false;
  }
}
#endif

void printStations() {
  static int prevNumber = 0;
  if (WiFi.softAPgetStationNum() != prevNumber) {
    prevNumber = WiFi.softAPgetStationNum();
    DEBUG_Serial.print(prevNumber);
    DEBUG_Serial.println(" station(s) connected");
    
  }
}

int getQuality() {
  if (WiFi.status() != WL_CONNECTED)
    return 0;
  int dBm = WiFi.RSSI();
  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}

void handleSerial() {
  switch (serialMessage[0] & ~(1 << 7)) {
    case MANUAL:
      autopilot = false;
      break;
    case BRAKE:
      movement = 0;
      break;
    case FORWARD:
      movement = 1;
      break;
    case LEFT:
      movement = 4;
      break;
    case BACKWARD:
      movement = 3;
      break;
    case RIGHT:
      movement = 2;
      break;
    case CHG_STATION:
      autopilot = true;
      break;
    case LIGHTS_ON:
      lights = 1;
      break;
    case LIGHTS_OFF:
      lights = 0;
      break;
    case LIGHTS_AUTO:
      lights = 2;
      break;
    case SETSPEED:
      speed = serialMessage[1];
      break;
    case ACTUALSPEED:
      actualSpeed = ((uint8_t) serialMessage[1]) / 127.0;
      break;
    case RESET:
      restart();
      break;
    default:
      // no default
      break;
  }
}

boolean restart() {
  display.clear();
  display.drawXbm(DISPLAY_WIDTH / 4, 0, reboot_width, reboot_height, reboot_bits);
  display.display();
  DEBUG_Serial.println("Restarting ...");
  DEBUG_Serial.flush();
  ESP.reset();
}

void generateHexStr(uint8_t cmd, uint8_t data, char * string) { // cmd = 0x7F, data = 0x7F → string = "0xFF 0x7F"
  string[0] = '0';
  string[1] = 'x';
  byte2hex(cmd | (1<<7), &string[2]);
  string[4] = ' ';
  string[5] = '0';
  string[6] = 'x';
  byte2hex(data, &string[7]);
}

void byte2hex(uint8_t data, char * string) {
  string[1] = (data & 0xF) > 9 ? (data & 0xF) - 0xA + 'A' : (data & 0xF) + '0';
  string[0] = (data >> 4) > 9 ? (data >> 4) - 0xA + 'A' : (data >> 4) + '0';
}

