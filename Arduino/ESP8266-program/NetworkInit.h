#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <WiFiUdp.h>

DNSServer dnsServer;
WiFiUDP udp;

boolean reset();

void startWiFi() {
  WiFi.hostname(WiFiHostname);
#ifdef STATION
  WiFi.mode(WIFI_AP_STA);
#else
  WiFi.mode(WIFI_AP);
#endif
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(APssid, APpassword) || reset();             // Start the access point
  DEBUG_Serial.print("Access Point \"");
  DEBUG_Serial.print(APssid);
  DEBUG_Serial.println("\" started\r\n");
#ifdef STATION
  WiFi.begin(ssid, password) || reset();            // Connect to the network
  DEBUG_Serial.print("Connecting to ");
  DEBUG_Serial.print(ssid); DEBUG_Serial.println(" ...");
#endif
}

void startOTA() { // start the Over The Air update services
  ArduinoOTA.setHostname(OTAName);
  ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]() {
    if (WiFi.status() != WL_CONNECTED)
      WiFi.disconnect();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int prog = progress * 100 / total;
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    if (prog < 20) {
      display.setFont(ArialMT_Plain_10);
      display.drawString(DISPLAY_WIDTH / 2, 1 * DISPLAY_HEIGHT / 4, "Sending data to\nthe NSA ...");
    } else if (prog < 35) {
      display.setFont(ArialMT_Plain_10);
      display.drawString(DISPLAY_WIDTH / 2, 1 * DISPLAY_HEIGHT / 4, "Euh, I mean ...");
    } else {
      display.setFont(ArialMT_Plain_16);
      display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 - 15, "Updating ...");
      display.setFont(ArialMT_Plain_10);
    }
    display.drawProgressBar(3, DISPLAY_HEIGHT / 2, 122, 8, prog );
    display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + 16, String(prog) + "%");
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
  DEBUG_Serial.println("Starting OTA service ...");
  ArduinoOTA.begin();
}

void startMDNS() { // Start the mDNS responder
  DEBUG_Serial.println("Starting mDNS responder ...");
  if (WiFi.status() != WL_CONNECTED)
    return;
  MDNS.begin(dnsName) || reset();
  DEBUG_Serial.print("mDNS responder started: http://");
  DEBUG_Serial.print(dnsName);
  DEBUG_Serial.println(".local");
}

void startDNS() {
  DEBUG_Serial.println("Starting DNS server");
  dnsServer.start(DNS_PORT, String(dnsName) + ".kul", apIP) || reset();
  DEBUG_Serial.println("DNS server started on http://" + String(dnsName) + ".kul");
}

void startUDP() {
  DEBUG_Serial.println("Starting UDP service");
  udp.begin(localPort) || reset();
  DEBUG_Serial.print("Started listening for UDP packets on port ");
  DEBUG_Serial.println(localPort);
}

boolean reset() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.drawString(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, "Fatal error :(");
  display.display();
  DEBUG_Serial.println("Failed.\r\nRebooting ...");
  DEBUG_Serial.flush();
  ESP.reset();
}

