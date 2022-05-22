#include "Zabavy.h"

void setup(void) {
  Serial.begin(115200);
  delay(1000);
  sync = tiempo.y;
  if (zabavy.debug) Serial.println(F("\nINICIALIZANDO PIXELES."));
  pixels.begin();
  for (unsigned int i = 0; i < NEO_C; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
  if (zabavy.debug) Serial.println(F("INICIALIZANDO MEMORIA."));
  EEPROM.begin(1024);
  //formatMemory();
  getConfig();
  if (zabavy.debug) Serial.println(F("INICIALIZANDO SERVIDOR."));
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_SSID);
  WiFi.softAPConfig(IPAddress(10, 0, 0, 1), IPAddress(10, 0, 0, 1), IPAddress(255, 255, 255, 0));
  webServer.on("/", pageRoot);
  webServer.on("/wifi", pageWifi);
  webServer.on("/wifisave", saveWifi);
  webServer.on("/info", pageInfo);
  webServer.on("/state", pageState);
  webServer.on("/color", pageColor);
  webServer.on("/anim", pageAnim);
  webServer.on("/save", pageSave);
  webServer.on("/zabavy", pageZabavy);
  webServer.onNotFound(pageNoFound);
  webServer.begin();
  serverUDP.begin(localPort);
  if (zabavy.debug) wifiInfo();
  delay(3000);
  if (WiFi.status() == WL_CONNECTED) {
    sendNTP(timeServer);
    if (zabavy.debug) Serial.println(F("SOLICITANDO HORA."));
    sync = tiempo.y;
  } else {
    if (zabavy.debug) Serial.println(F("SIN CONEXION"));
  }
  sec.attach(1, tick);
}

void loop(void) {
  webServer.handleClient();
  if (zabavy.on) {
    if (zabavy.anim > 0) {
      startShow();
    } else {
      for (unsigned int i = 0; i < NEO_C; i++) {
        pixels.setPixelColor(i, pixels.Color(zabavy.r, zabavy.g, zabavy.b));
      }
      pixels.show();
    }
  } else {
    for (unsigned int i = 0; i < NEO_C; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }
  if (Serial.available() > 0) {
    zabavy.debug = true;
    Serial.println(F("\nMODO DEBUG HABILITADO."));
    while (Serial.available() > 0) {
      Serial.print(Serial.read());
    }
  }
}
