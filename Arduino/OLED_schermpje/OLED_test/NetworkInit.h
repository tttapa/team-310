#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

void startWiFi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(APssid, APpassword);             // Start the access point
  DEBUG_Serial.print("Access Point \"");
  DEBUG_Serial.print(APssid);
  DEBUG_Serial.println("\" started\r\n");
#ifdef STATION
  WiFi.begin(ssid, password);             // Connect to the network
  DEBUG_Serial.print("Connecting to ");
  DEBUG_Serial.print(ssid); DEBUG_Serial.println(" ...");

  int i = 0;
  unsigned long nextWiFiFrame = millis();
  unsigned long frameTime = 200;
  
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    if(millis() > nextWiFiFrame) {
      display.clear();
      display.drawXbm(106, 0, 11, 9, wifi_bits[i]); // Show the Wi-Fi icon in the top bar
      display.display(); // send the frame buffer to the display
      
      i++;
      i%=4;
      nextWiFiFrame = millis()+frameTime;
    }
    ArduinoOTA.handle();
    yield();
  }
#endif
}

void startOTA() { // start the Over The Air update services
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
}
