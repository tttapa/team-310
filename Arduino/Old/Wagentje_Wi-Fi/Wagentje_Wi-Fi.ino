#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <WebSocketsServer.h>

#define DVD

#define DEBUG_Serial Serial1

#include "Commands.h"

#define NB_OF_COMMANDS 9

ESP8266WebServer server = ESP8266WebServer(80);       // create a web server on port 80
WebSocketsServer webSocket = WebSocketsServer(81);    // create a websocket server on port 81

File fsUploadFile;                                    // a File variable to temporarily store the received file

const char *ssid = "Team 310"; // The name of the Wi-Fi network that will be created
const char *password = "thereisnospoon";   // The password required to connect to it

const char *OTAName = "ESP8266";           // A name and a password for the OTA service
const char *OTAPassword = "esp8266";

const uint8_t commands[NB_OF_COMMANDS] = { FORWARD, BACKWARD, LEFT, RIGHT, BRAKE, SPEEDUP, SPEEDDOWN, CHG_STATION, MANUAL};

const char* mdnsName = "team-310"; // Domain name for the mDNS responder

/*__________________________________________________________SETUP__________________________________________________________*/

void setup() {
  DEBUG_Serial.begin(115200);        // Start the Serial communication to send messages to the computer
  Serial.begin(115200);              // Start the Serial communication with the ATmega328P
  delay(10);
  DEBUG_Serial.println("\r\nI read you, Dave.");

  startWiFi();                 // Start a Wi-Fi access point. Then wait for a station connection
  
  startOTA();                  // Start the OTA service
  
  startSPIFFS();               // Start the SPIFFS and list all contents

  startWebSocket();            // Start a WebSocket server
  
  startMDNS();                 // Start the mDNS responder

  startServer();               // Start a HTTP server with a file read handler and an upload handler
  
}

/*__________________________________________________________LOOP__________________________________________________________*/

unsigned long prevMillis = millis();
int hue = 0;

void loop() {
  webSocket.loop();                           // constantly check for websocket events
  server.handleClient();                      // run the server
  ArduinoOTA.handle();                        // listen for OTA events
}

/*__________________________________________________________SETUP_FUNCTIONS__________________________________________________________*/

void startWiFi() { // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);             // Start the access point
  DEBUG_Serial.print("Access Point \"");
  DEBUG_Serial.print(ssid);
  DEBUG_Serial.println("\" started\r\n");
}

void startOTA() { // Start the OTA service
  ArduinoOTA.setHostname(OTAName);
  ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]() {
    DEBUG_Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    DEBUG_Serial.println("\r\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DEBUG_Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    DEBUG_Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) DEBUG_Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) DEBUG_Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) DEBUG_Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) DEBUG_Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) DEBUG_Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  DEBUG_Serial.println("OTA ready\r\n");
}

void startSPIFFS() { // Start the SPIFFS and list all contents
  SPIFFS.begin();                             // Start the SPI Flash File System (SPIFFS)
  DEBUG_Serial.println("SPIFFS started. Contents:");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DEBUG_Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    DEBUG_Serial.printf("\n");
  }
}

void startWebSocket() { // Start a WebSocket server
  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  DEBUG_Serial.println("WebSocket server started.");
}

void startMDNS() { // Start the mDNS responder
  MDNS.begin(mdnsName);                        // start the multicast domain name server
  DEBUG_Serial.print("mDNS responder started: http://");
  DEBUG_Serial.print(mdnsName);
  DEBUG_Serial.println(".local");
}

void startServer() { // Start a HTTP server with a file read handler and an upload handler
  server.on("/edit.html",  HTTP_POST, []() {  // If a POST request is sent to the /edit.html address,
    server.send(200, "text/plain", ""); 
  }, handleFileUpload);                       // go to 'handleFileUpload'

  server.onNotFound(handleNotFound);          // if someone requests any other file or page, go to function 'handleNotFound'
                                              // and check if the file exists

  server.begin();                             // start the HTTP server
  DEBUG_Serial.println("HTTP server started.");
}

/*__________________________________________________________SERVER_HANDLERS__________________________________________________________*/

void handleNotFound(){ // if the requested file or page doesn't exist, return a 404 not found error
  if(!handleFileRead(server.uri())){          // check if the file exists in the flash memory (SPIFFS), if so, send it
    server.send(404, "text/plain", "404: File Not Found");
  }
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  DEBUG_Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    DEBUG_Serial.println(String("\tSent file: ") + path);
    return true;
  }
  DEBUG_Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

void handleFileUpload(){ // upload a new file to the SPIFFS
  HTTPUpload& upload = server.upload();
  String path;
  if(upload.status == UPLOAD_FILE_START){
    path = upload.filename;
    if(!path.startsWith("/")) path = "/"+path;
    if(!path.endsWith(".gz")) {                          // The file server always prefers a compressed version of a file 
      String pathWithGz = path+".gz";                    // So if an uploaded file is not compressed, the existing compressed
      if(SPIFFS.exists(pathWithGz))                      // version of that file must be deleted (if it exists)
         SPIFFS.remove(pathWithGz);
    }
    DEBUG_Serial.print("handleFileUpload Name: "); DEBUG_Serial.println(path);
    fsUploadFile = SPIFFS.open(path, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    path = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      DEBUG_Serial.print("handleFileUpload Size: "); DEBUG_Serial.println(upload.totalSize);
      server.sendHeader("Location","/success.html");      // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

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
      int index = strtol((const char *) &payload[0], NULL, 16) % NB_OF_COMMANDS;
      DEBUG_Serial.println(commands[index], HEX);
      Serial.write(commands[index] | 1<<7);
      break;
  }
}

/*__________________________________________________________HELPER_FUNCTIONS__________________________________________________________*/

String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}

String getContentType(String filename) { // determine the filetype of a given filename, based on the extension
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
