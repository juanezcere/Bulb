#include "Zabavy.h"

void setup(void) {
  Serial.begin(115200);
  delay(100);
  printDebug(0);
  sync = zabavy.tiempo.y;
  pixels.begin();
  printDebug(1);
  for (unsigned int i = 0; i < NEO_C; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_SSID);
  WiFi.softAPConfig(IPAddress(10, 0, 0, 1), IPAddress(10, 0, 0, 1), IPAddress(255, 255, 255, 0));
  /*webServer.on("/", pageRoot);
  webServer.on("/wifi", pageWifi);
  webServer.on("/wifisave", saveWifi);
  webServer.on("/info", pageInfo);
  webServer.on("/state", pageState);
  webServer.on("/color", pageColor);
  webServer.on("/anim", pageAnim);
  webServer.on("/save", pageSave);
  webServer.on("/zabavy", pageZabavy);
  webServer.onNotFound(pageNoFound);*/
  webServer.begin();
  serverUDP.begin(localPort);
  sec.attach(1, tick);
}

void loop(void) {
  webServer.handleClient();
}
