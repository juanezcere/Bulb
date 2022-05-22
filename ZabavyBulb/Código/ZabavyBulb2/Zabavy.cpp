#include "Zabavy.h"

Zabavy::Zabavy(){
    config.version = 1;
    config.str_SSID = "";
    config.str_PASS = "";
    config.str_CODE = "";
    config.str_NAME = "";
    config.str_DESC = "";
    config.str_PAGE = "";
    config.on = false;
    config.r = 0;
    config.g = 0;
    config.b = 0;
    config.debug = true;
    config.anim = 0;
    config.sync = 0;
    tiempo.h = 1;
    tiempo.m = 0;
    tiempo.s = 0;
    tiempo.z = 5;
    tiempo.y = 600;
    tiempo.t = false;
    tiempo.w = false;
    pixels = Adafruit_NeoPixel(NEO_CANT, NEO_PIN,  NEO_GRB + NEO_KHZ800);
    webServer = ESP8266WebServer(80);
    timeServer = IPAddress(129, 6, 15, 28);
}

Zabavy::~Zabavy(void){}

void Zabavy::begin(void){
    Serial.begin(115200);
    delay(1000);
    config.sync = tiempo.y;
    if(config.debug) Serial.println(F("\nINICIALIZANDO PIXELES."));
    pixels.begin();
    for (unsigned int i = 0; i < NEO_CANT; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
    if(config.debug) Serial.println(F("INICIALIZANDO MEMORIA."));
    EEPROM.begin(1024);
    //formatMemory();
    getConfig();
    if(config.debug) Serial.println(F("INICIALIZANDO SERVIDOR."));
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
    if(config.debug) wifiInfo();

}

void Zabavy::start(void){

}

void Zabavy::looper(void){

}

void Zabavy::getConfig(void){
    if(config.debug) Serial.println(F("BUSCANDO CONFIGURACION."));
    if ((EEPROM.read(0) == 0x00) && (EEPROM.read(2) == 0x00) && (EEPROM.read(9) == 0x00) && (EEPROM.read(138) == 0x00) && (EEPROM.read(267) == 0x00) && (EEPROM.read(288) == 0x00) && (EEPROM.read(417) == 0x00) && (EEPROM.read(674) == 0x00) && (EEPROM.read(676) == 0x00) && (EEPROM.read(678) == 0x00) && (EEPROM.read(680) == 0x00) && (EEPROM.read(682) == 0x00) && (EEPROM.read(684) == 0x00) && (EEPROM.read(1) == 0x01)) {
        if ((EEPROM.read(3) == 'Z') && (EEPROM.read(4) == 'A') && (EEPROM.read(5) == 'B') && (EEPROM.read(6) == 'A') && (EEPROM.read(7) == 'V') && (EEPROM.read(8) == 'Y')) {
            if(config.debug) Serial.println(F("CONFIGURACION ENCONTRADA."));
            uint16_t i = 10;
            while (EEPROM.read(i) != 0x00) {
                config.str_SSID += char(EEPROM.read(i));
                i++;
            }
            i = 139;
            while (EEPROM.read(i) != 0x00) {
                config.str_PASS += char(EEPROM.read(i));
                i++;
            }
            i = 268;
            while (EEPROM.read(i) != 0x00) {
                config.str_CODE += char(EEPROM.read(i));
                i++;
            }
            i = 289;
            while (EEPROM.read(i) != 0x00) {
                config.str_NAME += char(EEPROM.read(i));
                i++;
            }
            i = 418;
            while (EEPROM.read(i) != 0x00) {
                config.str_DESC += char(EEPROM.read(i));
                i++;
            }
            config.on = (EEPROM.read(675) == 1);
            config.r = EEPROM.read(677);
            config.g = EEPROM.read(679);
            config.b = EEPROM.read(681);
            config.anim = EEPROM.read(683);
        } else {
            if(config.debug) Serial.println(F("NO FUE ENCONTRADA NINGUNA CONFIGURACION."));
        }
    } else {
        if(config.debug) Serial.println(F("ERROR EN LA MEMORIA."));
        formatMemory();
    }
    delay(1000);
}

void Zabavy::formatMemory(void){
    if(config.debug) Serial.println(F("FORMATEANDO MEMORIA INTERNA."));
    for (uint16_t i = 0; i < 1024; i++) {
        EEPROM.write(i, 0x00);
    }
    EEPROM.write(1, 0x01);
    EEPROM.commit();
}

int16_t Zabavy::getRSSIasQuality(int16_t RSSI) {
    int16_t quality = 0;
    if (RSSI <= -100) {
        quality = 0;
    } else if (RSSI >= -50) {
        quality = 100;
    } else {
        quality = 2 * (RSSI + 100);
    }
    return quality;
}

void pageNoFound(void) {
    if(Zabavy.isDebug()) Serial.println(F("PAGINA NO ENCONTRADA."));
    Zabavy.setPage(FPSTR(HTTP_HEAD) + FPSTR(HTTP_STYLE) + FPSTR(HTTP_HEADEND) + FPSTR(HTTP_HEADER) + FPSTR(HTTP_NOFOUND) + FPSTR(HTTP_FOOTER) + FPSTR(HTTP_END));
    Zabavy.sendWeb(false, Zabavy.getPage());
    if(Zabavy.isDebug()) Serial.println(F("PAGINA NO ENCONTRADA ENVIADA."));
}

void pageRoot(void) {
    if(Zabavy.isDebug()) Serial.println(F("CARGANDO PAGINA INICIAL."));
    Zabavy.setPage(FPSTR(HTTP_HEAD) + FPSTR(HTTP_STYLE) + FPSTR(HTTP_HEADEND) + FPSTR(HTTP_HEADER) + FPSTR(HTTP_ROOT) + FPSTR(HTTP_FOOTER) + FPSTR(HTTP_END));
    Zabavy.sendWeb(true, Zabavy.getPage());
    if(Zabavy.isDebug()) Serial.println(F("PAGINA INICIAL ENVIADA."));
}

void pageWifi(void) {
    if(Zabavy.isDebug()) Serial.println(F("CARGANDO PAGINA DE CONFIGURACION DEL WIFI."));
    Zabavy.setPage(FPSTR(HTTP_HEAD) + FPSTR(HTTP_SCRIPT) + FPSTR(HTTP_STYLE) + FPSTR(HTTP_HEADEND) + FPSTR(HTTP_HEADER));
    if(Zabavy.isDebug()) Serial.println(F("BUSCANDO REDES WIFI."));
    uint16_t n = WiFi.scanNetworks();
    if (n == 0) {
        Zabavy.addPage(FPSTR(HTTP_NONET));
    } else {
        uint16_t indices[n];
        for (uint16_t i = 0; i < n; i++) {
            indices[i] = i;
        }
        for (uint16_t i = 0; i < n; i++) {
            for (uint16_t j = i + 1; j < n; j++) {
                if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
                    std::swap(indices[i], indices[j]);
                }
            }
        }
        String cssid;
        for (uint16_t i = 0; i < n; i++) {
            if (indices[i] == -1) continue;
            cssid = WiFi.SSID(indices[i]);
            for (uint16_t j = i + 1; j < n; j++) {
                if (cssid == WiFi.SSID(indices[j])) {
                    indices[j] = -1;
                }
            }
        }
        Zabavy.addPage(F("<section><table>"));
        if(Zabavy.isDebug()) Serial.println(F("ORDENANDO REDES WIFI."));
        for (int16_t i = 0; i < n; i++) {
            if (indices[i] == -1) continue;
            int16_t quality = Zabavy.getRSSIasQuality(WiFi.RSSI(indices[i]));
            if (quality > -1) {
                String item = FPSTR(HTTP_ITEM);
                String rssiQ;
                rssiQ += quality;
                item.replace("{v}", WiFi.SSID(indices[i]));
                item.replace("{r}", rssiQ);
                if (WiFi.encryptionType(indices[i]) != ENC_TYPE_NONE) item.replace("{i}", "l");
                else item.replace("{i}", "");
                Zabavy.addPage(item);
            }
        }
        Zabavy.addPage(F("</table>"));
    }
    Zabavy.addPage(FPSTR(HTTP_FORM) + F("</section>") + FPSTR(HTTP_FOOTER) + FPSTR(HTTP_END));
    Zabavy.sendWeb(true, Zabavy.getPage());
    if(Zabavy.isDebug()) Serial.println(F("PAGINA DE CONFIGURACION WIFI ENVIADA."));
}

void saveWifi(void) {
    if(Zabavy.isDebug()) Serial.println(F("CARGANDO PAGINA DE GUARDADO DE WIFI."));
    Zabavy.setSSID(Zabavy.getArg("S"));
    Zabavy.setPASS(Zabavy.getArg("P"));
    Zabavy.setCODE(Zabavy.getArg("C"));
    Zabavy.setNAME(Zabavy.getArg("N"));
    Zabavy.setDESC(Zabavy.getArg("D"));
    Zabavy.setST(false);
    Zabavy.setR(0);
    Zabavy.setG(0);
    Zabavy.setB(0);
    if(Zabavy.isDebug()) Zabavy.info();
    Zabavy.setPage(FPSTR(HTTP_HEAD) + FPSTR(HTTP_STYLE) + FPSTR(HTTP_HEADEND) + FPSTR(HTTP_HEADER));
    delay(5000);
    if (WiFi.status() == WL_CONNECTED) {
        if(Zabavy.isDebug()) Serial.println(F("CONECTADO"));
        Zabavy.formatMemory();
        for (uint8_t i = 0; i < 128; i++) {
            if (Zabavy.getSSID().charAt(i) == '\0') break;
            else EEPROM.write(i + 10, Zabavy.getSSID().charAt(i));
        }
        for (uint8_t i = 0; i < 128; i++) {
            if (Zabavy.getPASS().charAt(i) == '\0') break;
            else EEPROM.write(i + 139, Zabavy.getPASS().charAt(i));
        }
        for (uint8_t i = 0; i < 20; i++) {
            if (Zabavy.getCODE().charAt(i) == '\0') break;
            else EEPROM.write(i + 268, Zabavy.getCODE().charAt(i));
        }
        for (uint8_t i = 0; i < 10; i++) {
            if (Zabavy.getNAME().charAt(i) == '\0') break;
            else EEPROM.write(i + 289, Zabavy.getNAME().charAt(i));
        }
        for (uint16_t i = 0; i < 256; i++) {
            if (Zabavy.getDESC().charAt(i) == '\0') break;
            else EEPROM.write(i + 418, Zabavy.getDESC().charAt(i));
        }
        EEPROM.write(675, (Zabavy.getST() ? 0x01 : 0x00));
        EEPROM.write(677, Zabavy.getB());
        EEPROM.write(679, Zabavy.getG());
        EEPROM.write(681, Zabavy.getB());
        EEPROM.write(3, 'Z');
        EEPROM.write(4, 'A');
        EEPROM.write(5, 'B');
        EEPROM.write(6, 'A');
        EEPROM.write(7, 'V');
        EEPROM.write(8, 'Y');
        EEPROM.commit();
        if(Zabavy.isDebug()) Serial.println(F("DATOS GUARDADOS EN MEMORIA INTERNA"));
        if ((EEPROM.read(3) == 'Z') && (EEPROM.read(4) == 'A') && (EEPROM.read(5) == 'B') && (EEPROM.read(6) == 'A') && (EEPROM.read(7) == 'V') && (EEPROM.read(8) == 'Y')) {
            if(Zabavy.isDebug()) Serial.println(F("CARGANDO CONFIGURACION."));
            Zabavy.addPage(F("<section><div><br><br><p>"));
            Zabavy.addPage(WiFi.localIP().toString());
            Zabavy.addPage(F("<br><br><br><a href='/'>Volver a Inicio</a></p></div></section>"));
            if(Zabavy.isDebug()) Serial.print(F("DIRECCION IP DEL MODULO: "));
            if(Zabavy.isDebug()) Serial.println(WiFi.localIP());
        } else {
            if(Zabavy.isDebug()) Serial.println(F("ERROR GUARDANDO INFORMACION EN LA MEMORIA INTERNA."));
            Zabavy.addPage(FPSTR(HTTP_UNSAVED));
        }
    } else {
        if(Zabavy.isDebug()) Serial.println(F("ERROR CONECTANDO A LA RED WIFI."));
        Zabavy.addPage(FPSTR(HTTP_UNSAVED));
    }
    Zabavy.addPage(FPSTR(HTTP_FOOTER) + FPSTR(HTTP_END));
    Zabavy.sendWeb(true, Zabavy.getPage());
    if(Zabavy.isDebug()) Serial.println(F("PAGINA DE GUARDAR CONFIGURACION ENVIADA."));
}

void pageInfo(void) {
    if(Zabavy.isDebug()) Serial.println(F("CARGANDO PAGINA DE INFORMACION."));
    String page = FPSTR(HTTP_HEAD);
    page += FPSTR(HTTP_SCRIPT);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADEND);
    page += FPSTR(HTTP_HEADER);
    page += F("<section><br><br><table>");
    String item = FPSTR(HTTP_INFO);
    item.replace("{v}", F("ID DEL CHIP"));
    item.replace("{r}", String(ESP.getChipId()));
    page += item;
    item = FPSTR(HTTP_INFO);
    item.replace("{v}", F("FLASH DEL CHIP"));
    item.replace("{r}", String(ESP.getFlashChipSize()) + " BYTES");
    page += item;
    item = FPSTR(HTTP_INFO);
    item.replace("{v}", F("TAMAÑO DE LA MEMORIA FLASH"));
    item.replace("{r}", String(ESP.getFlashChipRealSize()) + " BYTES");
    page += item;
    item = FPSTR(HTTP_INFO);
    item.replace("{v}", F("VERSIÓN DEL CÓDIGO"));
    item.replace("{r}", String(EEPROM.read(1)));
    page += item;
    item = FPSTR(HTTP_INFO);
    item.replace("{v}", F("NOMBRE DEL ACCESS POINT"));
    item.replace("{r}", AP_SSID);
    page += item;
    item = FPSTR(HTTP_INFO);
    item.replace("{v}", F("DIRECCIÓN IP DEL SERVIDOR"));
    item.replace("{r}", WiFi.softAPIP().toString());
    page += item;
    item = FPSTR(HTTP_INFO);
    item.replace("{v}", F("DIRECCIÓN MAC DEL SERVIDOR"));
    item.replace("{r}", WiFi.softAPmacAddress());
    page += item;
    item = FPSTR(HTTP_INFO);
    item.replace("{v}", F("ESTADO DE LA CONEXIÓN WIFI"));
    item.replace("{r}", WiFi.status() == WL_CONNECTED ? F("CONECTADO") : F("DESCONECTADO"));
    page += item;
    item = FPSTR(HTTP_INFO);
    item.replace("{v}", F("NOMBRE DE LA RED"));
    item.replace("{r}", WiFi.status() == WL_CONNECTED ? zabavyObj.getSSID() : F("SIN CONEXIÓN"));
    page += item;
    item = FPSTR(HTTP_INFO);
    item.replace("{v}", F("CONTRASEÑA DE LA RED"));
    item.replace("{r}", WiFi.status() == WL_CONNECTED ? F("*******") : F("SIN CONTRASEÑA"));
    page += item;
    item = FPSTR(HTTP_INFO);
    item.replace("{v}", F("DIRECCIÓN IP DE LA ESTACIÓN"));
    item.replace("{r}", WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : F("SIN DIRECCIÓN IP"));
    page += item;
    item = FPSTR(HTTP_INFO);
    item.replace("{v}", F("DIRECCIÓN MAC DE LA ESTACIÓN"));
    item.replace("{r}", WiFi.macAddress());
    page += item;
    page += F("</table><br><br><p><a href='/'>Volver a Inicio</a></p></section>");
    page += FPSTR(HTTP_FOOTER);
    page += FPSTR(HTTP_END);
    zabavyObj.sendWeb(true, page);
    if(zabavyObj.isDebug()) Serial.println(F("PAGINA DE INFORMACION ENVIADA."));
}

void pageState(void) {
    if(zabavyObj.isDebug()) Serial.println(F("CARGANDO PAGINA DE CAMBIO DE ESTADO."));
    zabavyObj.setST(!zabavyObj.getST());
    String page = FPSTR(HTTP_HEAD);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADEND);
    page += FPSTR(HTTP_HEADER);
    page += F("<section><div><br><br><p>BOMBILLO ");
    page += zabavyObj.getST() ? F("ENCENDIDO") : F("APAGADO");
    page += F("<br><br><br><a href='/'>Volver a Inicio</a></p></div></section>");
    page += FPSTR(HTTP_FOOTER);
    page += FPSTR(HTTP_END);
    zabavyObj.sendWeb(true, page);
    if(zabavyObj.isDebug()) Serial.println(F("PAGINA ENVIADA."));
}

void pageSave(void) {
    if(zabavyObj.isDebug()) Serial.println(F("CARGANDO PAGINA DE GUARDADO DE ESTADO."));
    String page = FPSTR(HTTP_HEAD);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADEND);
    page += FPSTR(HTTP_HEADER);
    EEPROM.write(677, zabavyObj.getR());
    EEPROM.write(679, zabavyObj.getG());
    EEPROM.write(681, zabavyObj.getB());
    EEPROM.write(675, (zabavyObj.getST() ? 0x01 : 0x00));
    EEPROM.write(683, zabavyObj.getANIM());
    EEPROM.commit();
    page += F("<section><div><br><br><p>ESTADOS GUARDADOS.<br><br><br><a href='/'>Volver a Inicio</a></p></div></section>");
    page += FPSTR(HTTP_FOOTER);
    page += FPSTR(HTTP_END);
    zabavyObj.sendWeb(true, page);
    if(zabavyObj.isDebug()) Serial.println(F("PAGINA ENVIADA."));
}

void pageColor(void){
    if(zabavyObj.isDebug()) Serial.println(F("CARGANDO PAGINA DE CAMBIO DE COLOR."));
    String color = zabavyObj.getArg("C");
    int16_t number = (int16_t) strtol(&color[1], NULL, 16);
    zabavyObj.setR(number >> 16);
    zabavyObj.setG(number >> 8 & 0xFF);
    zabavyObj.setB(number & 0xFF);
    zabavyObj.setST(false);
    String page = FPSTR(HTTP_HEAD);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADEND);
    page += FPSTR(HTTP_HEADER);
    page += F("<section><div><br><br><p>COLOR CAMBIADO.<br><br><br><a href='/'>Volver a Inicio</a></p></div></section>");
    page += FPSTR(HTTP_FOOTER);
    page += FPSTR(HTTP_END);
    zabavyObj.sendWeb(true, page);
    if(zabavyObj.isDebug()) Serial.println(F("PAGINA ENVIADA."));
}

void pageAnim(void){
    if(zabavyObj.isDebug()) Serial.println(F("CARGANDO PAGINA DE CAMBIO DE ANIMACION."));
    zabavyObj.setANIM(String(zabavyObj.getArg("A")).toInt());
    zabavyObj.setST(true);
    String page = FPSTR(HTTP_HEAD);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADEND);
    page += FPSTR(HTTP_HEADER);
    page += F("<section><div><br><br><p>ANIMACIÓN CAMBIADA.<br><br><br><a href='/'>Volver a Inicio</a></p></div></section>");
    page += FPSTR(HTTP_FOOTER);
    page += FPSTR(HTTP_END);
    zabavyObj.sendWeb(true, page);
    if(zabavyObj.isDebug()) Serial.println(F("PAGINA ENVIADA."));
}

void pageZabavy(void){
    if(zabavyObj.isDebug()) Serial.println(F("CARGANDO PAGINA ZABAVY"));
    zabavyObj.sendWeb(true, zabavyObj.getCODE());
    if(zabavyObj.isDebug()) Serial.println(F("PAGINA ENVIADA."));
}

void Zabavy::wifiInfo(void){
    Serial.print(F("ID DEL CHIP: "));
    Serial.println(String(ESP.getChipId()));
    Serial.print(F("FLASH DEL CHIP: "));
    Serial.print(String(ESP.getFlashChipSize()));
    Serial.println(F(" BYTES"));
    Serial.print(F("TAMANO DE LA MEMORIA FLASH: "));
    Serial.print(String(ESP.getFlashChipRealSize()));
    Serial.println(F(" BYTES"));
    Serial.print(F("VERSION DEL CODIGO: "));
    Serial.println(String(EEPROM.read(1)));
    Serial.print(F("NOMBRE DEL PUNTO DE ACCESO: "));
    Serial.println(AP_SSID);
    Serial.print(F("DIRECCION IP: "));
    Serial.println(WiFi.softAPIP().toString());
    Serial.print(F("DIRECCION MAC: "));
    Serial.println(WiFi.macAddress());
    Serial.print(F("ESTADO DE LA CONEXION WIFI: "));
    WiFi.status() == WL_CONNECTED ? Serial.println(F("CONECTADO")) : Serial.println(F("DESCONECTADO"));
    Serial.print(F("NOMBRE DE LA RED: "));
    WiFi.status() == WL_CONNECTED ? Serial.println(WiFi.SSID()) : Serial.println(F("DESCONECTADO"));
    Serial.print(F("CONTRASENA DE LA RED: "));
    WiFi.status() == WL_CONNECTED ? Serial.println(F("*******")) : Serial.println(F("DESCONECTADO"));
    Serial.print(F("DIRECCION IP: "));
    WiFi.status() == WL_CONNECTED ? Serial.println(WiFi.localIP().toString()) : Serial.println(F("DESCONECTADO"));
    Serial.print(F("FUERZA DE LA SENAL (RSSI): "));
    WiFi.status() == WL_CONNECTED ? Serial.print(WiFi.RSSI()) : Serial.print(F("DESCONECTADO"));
    WiFi.status() == WL_CONNECTED ? Serial.println(F(" dBm")) : Serial.println();
}

boolean Zabavy::isDebug(void){
    return zabavy.debug;
}

void Zabavy::sendWeb(boolean state, String page){
    webServer.send((state ? 200 : 404), "text/html", page);
}

String Zabavy::getArg(String arg){
    return webServer.arg(arg).c_str();
}

void Zabavy::setSSID(String data){
    zabavy.str_SSID = data;
}

void Zabavy::setPASS(String data){
    zabavy.str_PASS = data;
}

void Zabavy::setCODE(String data){
    zabavy.str_CODE = data;
}

void Zabavy::setNAME(String data){
    zabavy.str_NAME = data;
}

void Zabavy::setDESC(String data){
    zabavy.str_DESC = data;
}

void Zabavy::setST(boolean st){
    zabavy.on = st;
}

void Zabavy::setR(uint8_t color){
    zabavy.r = color;
}

void Zabavy::setG(uint8_t color){
    zabavy.g = color;
}

void Zabavy::setB(uint8_t color){
    zabavy.b = color;
}

void Zabavy::setANIM(uint8_t anim){
    zabavy.anim = anim;
}

void Zabavy::info(void){
    Serial.println(F("DATOS OBTENIDOS"));
    Serial.print(F("SSID: "));
    Serial.println(zabavy.str_SSID);
    Serial.print(F("PASS: "));
    Serial.println(zabavy.str_PASS);
    Serial.print(F("CODE: "));
    Serial.println(zabavy.str_CODE);
    Serial.print(F("NAME: "));
    Serial.println(zabavy.str_NAME);
    Serial.print(F("DESC: "));
    Serial.println(zabavy.str_DESC);
    Serial.print(F("STAT: "));
    Serial.println(zabavy.on ? F("ON") : F("OFF"));
    Serial.print(F("RED: "));
    Serial.println(zabavy.r, DEC);
    Serial.print(F("GREEN: "));
    Serial.println(zabavy.g, DEC);
    Serial.print(F("BLUE: "));
    Serial.println(zabavy.b, DEC);
}

String Zabavy::getSSID(void){
    return zabavy.str_SSID;
}

String Zabavy::getPASS(void){
    return zabavy.str_PASS;
}

String Zabavy::getCODE(void){
    return zabavy.str_CODE;
}

String Zabavy::getNAME(void){
    return zabavy.str_NAME;
}

String Zabavy::getDESC(void){
    return zabavy.str_DESC;
}

boolean Zabavy::getST(void){
    return zabavy.on;
}

uint8_t Zabavy::getR(void){
    return zabavy.r;
}

uint8_t Zabavy::getG(void){
    return zabavy.g;
}

uint8_t Zabavy::getB(void){
    return zabavy.b;
}

uint8_t Zabavy::getANIM(void){
    return zabavy.anim;
}

String Zabavy::getPage(void){
    return config.str_PAGE;
}

void Zabavy::setPage(String s){
    config.str_PAGE = s;
}

void Zabavy::addPage(String s){
    config.str_PAGE += s;
}