#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <Ticker.h>

#define AP_SSID "Zabavy"
#define NTP 48
#define localPort 2390
#define NEO_D 2
#define NEO_C 14

const char HTTP_HEAD[] PROGMEM = "<!DOCTYPE html><html lang='es'><head><meta charset='UTF-8'/><meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/><meta name='description' content='Zabavy'/><meta name='keywords' content='Zabavy'/><meta name='author' content='Juanez'/><title>Zabavy</title><link type='image/x-icon' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABoAAAAaCAYAAACpSkzOAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH4QYeBhsfDvkNVgAAAB1pVFh0Q29tbWVudAAAAAAAQ3JlYXRlZCB3aXRoIEdJTVBkLmUHAAAFNUlEQVRIx5XWa2wV9BkG8N+59PRyAEsJtQXaAjKCMKCEWBXlFioXkaIjIIwQtyzZ3KImRLZkHxb06wLJFhkmmri48WGUuSyMSRk4YCwIgSlILSClFAqT6zm9n17OZR/akq6rkP2//d+87/PkvT15Ax7wXiGnk/B0ut4mOZzPVsJ15PR/E3tIDecXGM5YSWkukbt8u4mFeXxVypHJ1L9HL/yQrAamXGNRB49DlFMr+MuvaX4o0dOUXeSXAcxidz2zepmFcJR9L1AN+1jXwSr0ZnEJOV2UR/not+xcPKQC4aFEOX3sgQx6OXONP79FQTXLEzz/V2ZAisLRVK9j/1vELpG1kh+1sGwb+9DwoLZ4lcL5rHmZxwZnvJVgJSuLOVHEySW8sJXg4NhnmVvAJxXMH4obHGo4zazLvNjWl3pmwF7Po9eZO5qjhRy8yex6Hh0cm0dLgGSaEUNxw8Nl1UNhhLytROrIzeGRz3g5j7Kp/CGb3ousO8fGTVR30TKdxCmyAgRCw0zofxFlCCzum6qRjbzURDzMqHZmdRN+hLM3KIUIV2OUn2F2HrUNtCAf0Vx6MgQCgypyn+injKzgmVaW9lLSydRC/jiavBRlxfxuI2eipOFLgqeYFue7RTTcIXGXJ3sp+ZrVFeS+zvF3aIUQVDH9AG+mmRvlPFJp/r2SPZ+yJMn55ezfTO8ec4N7RINHxVKX+HoM0Q4mLKbmC2bm0DqKc61UnuWppVw/z93QQsrr2ILWpfxqC4c+pqCDeUkCSUZ+j10/p5ux82hdTfNGgtOI1r2j++JpyuNMamJ+Lgd38H6Cs/XMuc6ychqD9WwIElvP9ve4vJhkEafTFDTynSCnN/dteoZcZEpIVxAqJD+zjkSGz66yIs3oIk4vJvkuDevYjthl1gcTlI2nZju3B/pVTn2YC20EKzjXZy3N585LtD9FZDdjdnK1GRZS30wkwoVy6gdwtnO7mJoEZUF95fkfKYqQ7KX4eL+O0RsiEyHQSXY2yZkU50FNn0yVROkeipMhmCYQKqCigxmvcfYYbf0yND3J00GSLRT/gmOHtDdT+i8CnxOMoYXojR1iuTW8lksqn3ica43c+gdlT1B0hRUh2kJzuHuHys+ZV05sG3cPUomuIg7co+oKt29xgXgHHTdov0x7E7HUDZ5vp6qEnRm6ehhziNbjbLvKcyWcKGFf6Cq3lnGhidlx1h1mbpLyfM5V8kktE5tZ9ANOnOxbyvvvDUpP8rMcajdQ3cS4GM9lyL7HtBTpx3n/Y+rv9+Z1Rn3KM7epivPiCM6HqSsgu5EV+eyazN4wnZAm9xJVLWwqY3+c7hTfamfGBHZMYW8bHYe5hkxgGAla0MCbYzjYyohuJiaY1M2UMF1RrkInZUmyI1zO5Uo2jfnE4iwp5TdHOPqNWhcg8ySRDG0z+dMk7gyI6j/Z1Mmaqfw9Q+A8G/LZ9Sy7BkT1Fvk1PJEg8lD1ThFOEkngbXr6xFzLRj44ytgvqeof/8ML+OBDbg7Eru9blezUMLjBYQztKaJxCgbbp3CvmNoE4zsZN47aicQG+7RSkGRUkPaHEo3lBlLXWFBLZDkTVjPxI9ZeZ8MY9hdwoImNv+fVzYzPEPqK7HoWIdCP8eDj5DDh7/OTDlY9xu7LLA2SNZkjceLL2NtD4G+sbWFNiGQOdRmyupiT9w3HSWgo0YekX+HiLWKj6bzJjDSZ6bz7Bse2kDhN11q+yOJUFz09FGUIjmTfKqp/3L8CD73rYC2h0QQvMj5N4BiNg2+I//eA/A+aFeump6aWDQAAAABJRU5ErkJggg==' rel='icon'/>";
const char HTTP_STYLE[] PROGMEM = "<style>*{font-family:'Quattrocento Sans',sans-serif;padding:0px;margin:0px;} body{text-align:center;background:#E6E6E6;} a{text-decoration:none;} a:hover{cursor:pointer;} header{position:fixed;width:100%;float:left;background:rgb(169,3,41);background:-moz-linear-gradient(top,rgba(169,3,41,1)0%,rgba(143,2,34,1)44%,rgba(109,0,25,1)100%);background:-webkit-linear-gradient(top,rgba(169,3,41,1)0%,rgba(143,2,34,1)44%,rgba(109,0,25,1)100%);background:linear-gradient(to bottom,rgba(169,3,41,1)0%,rgba(143,2,34,1)44%,rgba(109,0,25,1)100%);filter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#a90329',endColorstr='#6d0019',GradientType=0);box-shadow:0px 3px 5px;text-align:center;padding-top:15px;padding-bottom:15px;} header h1{text-decoration:none;color:#FFFFFF;font-size:20px;font-weight:bold;border-radius:3px;-moz-border-radius:3px;-webkit-border-radius:3px;} section{padding:60px;} div{color:#000000;padding: 20px;} button{background:rgb(169,3,41);background:-moz-linear-gradient(top,rgba(169,3,41,1)0%,rgba(143,2,34,1)44%,rgba(109,0,25,1)100%);background:-webkit-linear-gradient(top,rgba(169,3,41,1)0%,rgba(143,2,34,1)44%,rgba(109,0,25,1)100%);background:linear-gradient(to bottom,rgba(169,3,41,1)0%,rgba(143,2,34,1)44%,rgba(109,0,25,1)100%);filter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#a90329',endColorstr='#6d0019',GradientType=0);box-shadow:0px 0px 20px rgba(0,0,0,0.5);padding:10px;border-radius:20px;-moz-border-radius:20px;-webkit-border-radius:20px;margin:10px;transition:all 0.5s linear;color:#FFFFFF;float:center;width:100px;height:60px;} footer{width:100%;height:30px;float:left;margin:auto;margin-bottom:0px;bottom:0px;left:0px;background:rgb(169,3,41);background:-moz-linear-gradient(top,rgba(169,3,41,1)0%,rgba(143,2,34,1)44%,rgba(109,0,25,1)100%);background:-webkit-linear-gradient(top,rgba(169,3,41,1)0%,rgba(143,2,34,1)44%,rgba(109,0,25,1)100%);background:linear-gradient(to bottom,rgba(169,3,41,1)0%,rgba(143,2,34,1)44%,rgba(109,0,25,1)100%);filter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#a90329',endColorstr='#6d0019',GradientType=0);text-align:center;position:fixed;} footer p{text-decoration:none;color:#FFFFFF;font-size:15px;border-radius:3px;-moz-border-radius:3px;-webkit-border-radius:3px;padding:7px;} table{margin:auto;text-align:left;} .l{background:url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAZjQAAGY0BJ9e2RgAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAALLSURBVGiB7di9i1VHGAbw36yaIhvZbEK+CDHBfEhShSAhRVLZhiQIgUACCfkDUiSQkDRpQjobwT/AQgs7C+1ERVZUxGJFFdUVC1dRxLuFun6Oxe6KXM/svTPn4xb6wBT3PTPP+zxzz7wzZ0KMUZsIIUzia3yDD/E2XsUNXMIMdmFnjPFqdoIYYysNL2AL7iIO0R5gO1Zn5WlJ/Ds4PKTw/nYS60ZmABtwrVD8UpvDt50bwEbcryl+qT3Er4NyhqYWcQhhBU5g3TLdjuIsbuBlvI/PERL9r2BtjPF2krHB2f9Jejan8XFi3LuYWmbsH62/QliB0wkB2/DigPGrsDkx/irG2zaQmv0TGMvg2Zvg+attA7sTiX/I5PkqwXOsbQNnKpKeypn9J7j2VXD1Uv3Hkqt7SCxWn/cqHu2PMT4soJyqiE2EEF6p6lzbANZgFfpxvpDvQiK+tirYhIEPEvGZQr7UuNYMvJUpZBBS49ZUBZswkOJI757Lo5eIrxw6mIlz2FoRnyvku5vgO17VubGz0KhQ/A+EEF7DF/gU440pSmMe0zgUY5wFULDRjOF33NLMsTm33cO/WFm0E2MLRiH8qUNitgFswKiFP9k25pbRnzP7t41fcg2sb0VGOdZnldEQQg8T7enJRxM78Ujx3EABLmEPLjbClllGe8pL3m199zz4viZn9iZWJ9lvCc4f6xjoqgrdt3BpO1/BGTCH1QW8na2B61XiIcYYMVv1bBh0ZeCNEELlp2cI4XV8VErcZRX6Z/F16cff0nejg9HhIo74v4/vz5p8set94JMBv7PxfCceNbo20J+vfv6OF3HP4l0/VuFyTb7YxL1QDiZwMISwA9/hzbqEz9wHTeVxYIS4k2tguhUZ5ZjONXCgFRnlOJBbhcZxDrUqR0NtFpMlN3OfYQajFv9l0dXioomX8B+mcLMj0fM4gk2YXNLyCANG+kd/3EjsAAAAAElFTkSuQmCC') no-repeat left center;background-size:1em;} .q{float:right;width:64px;text-align:right;} input{border-radius:10px;-moz-border-radius:10px;-webkit-border-radius:10px;padding:5px;margin:5px auto;box-shadow:0 3px 5px;width:350px;height:25px;color:#000000;text-align:center;} select{border-radius:10px;-moz-border-radius:10px;-webkit-border-radius:10px;padding:5px;margin:5px auto;box-shadow:0 3px 5px;width:200px;height:60px;color:#000000;text-align:center;} button:hover{cursor:pointer;}#color{background:rgb(169,3,41);background:-moz-linear-gradient(top,rgba(169,3,41,1)0%,rgba(143,2,34,1)44%,rgba(109,0,25,1)100%);background:-webkit-linear-gradient(top,rgba(169,3,41,1)0%,rgba(143,2,34,1)44%,rgba(109,0,25,1)100%);background:linear-gradient(to bottom,rgba(169,3,41,1)0%,rgba(143,2,34,1)44%,rgba(109,0,25,1)100%);filter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#a90329',endColorstr='#6d0019',GradientType=0);box-shadow:0px 0px 20px rgba(0,0,0,0.5);padding:10px;border-radius:20px;-moz-border-radius:20px;-webkit-border-radius:20px;margin:10px;transition:all 0.5s linear;color:#FFFFFF;float:center;width:80px;height:40px;} #color:hover{cursor:pointer;}</style>";
const char HTTP_SCRIPT[] PROGMEM = "<script>function c(l){document.getElementById('S').value=l.innerText||l.textContent;document.getElementById('P').focus();}</script>";
const char HTTP_HEADEND[] PROGMEM = "</head><body>";
const char HTTP_HEADER[] PROGMEM = "<header><h1>Zabavy</h1></header>";
const char HTTP_ROOT[] PROGMEM = "<section><form action='/wifi' metod='get'><button>Configurar WiFi</button></form><br><form action='/color' metod='get'><table><thead><th><input type='color' id='color' name='C'/></th><th><button type='submit'>Enviar color</button></th></thead></table></form><br><form action='/anim' metod='get'><table><thead><th><select name='A'><option value=0>Ninguna animación</option><option value=1>Movimiento</option><option value=2>Destello</option><option value=3>Arcoiris</option><option value=4>Arcoiris individual</option><option value=5>Parpadeo multicolor</option><option value=6>Fuego</option></select></th><th><button type='submit'>Enviar animación</button></th></thead></table></form><br><form action='/state' metod='get'><button>Cambiar estado</button></form><br><form action='/save' metod='get'><button>Guardar estado</button></form><br><form action='/info' metod='get'><button>Información</button></form></section>";
const char HTTP_FOOTER[] PROGMEM = "<footer><p>Zabavy INC. Todos los derechos reservados.</p></footer>";
const char HTTP_NONET[] PROGMEM = "<section><div><p>No hay redes WiFi visibles en este momento, recargue esta página para volver a buscar redes WiFi.</p></div></section>";
const char HTTP_UNSAVED[] PROGMEM = "<section><div><p>Las configuraciones del dispositivo no pudieron ser guardadas.<br>Si el problema persiste comuniquese con nosotros.</p></div></section>";
const char HTTP_ITEM[] PROGMEM = "<tr><td><a href='#P' onclick='c(this)'>{v}</a></td><td><span class='q {i}'>{r}%</span></td></tr>";
const char HTTP_FORM[] PROGMEM = "<div><form method='get' action='wifisave'><input type='text' id='S' name='S' length=128 placeholder='NOMBRE DE RED WIFI' required/><br><input type='password' id='P' name='P' length=128 placeholder='CONTRASEÑA' required/><br><input type='text' id='C' name='C' length=20 placeholder='CÓDIGO DEL DISPOSITIVO' required/><br><input type='text' id='N' name='N' length=128 placeholder='NOMBRE'/><br><input type='text' id='D' name='D' length=256 placeholder='DESCRIPCIÓN'/><br><button type='submit'>Guardar</button></form></div>";
const char HTTP_INFO[] PROGMEM = "<tr><td><p>{v}</p></td><td><p>{r}</p></td></tr>";
const char HTTP_NOFOUND[] PROGMEM = "<section><div><br><br><p>UPS... Al parecer la página que buscas no se encuentra disponible.</p></div></section>";
const char HTTP_END[] PROGMEM = "</body></html>";

struct Zabavy {
  uint8_t version = 1;
  String str_SSID = "";
  String str_PASS = "";
  String str_CODE = "";
  String str_NAME = "";
  String str_DESC = "";
  boolean on = false;
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  boolean debug = true;
  unsigned int anim = 0;
} zabavy;

struct Tiempo {
  uint8_t h = 1;
  uint8_t m = 0;
  uint8_t s = 0;
  uint8_t z = 5;
  uint16_t y = 600;
  boolean t = false;
  boolean w = true;
} tiempo;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NEO_C, NEO_D, NEO_GRB + NEO_KHZ800);
unsigned int sync = 0;
ESP8266WebServer webServer(80);
IPAddress timeServer(129, 6, 15, 28);
byte packetBuffer[NTP];
WiFiUDP serverUDP;
Ticker sec;

void wifiInfo(void) {
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

void showTime(void) {
  Serial.print(tiempo.h);
  Serial.print(F(":"));
  if (tiempo.m < 10) Serial.print('0');
  Serial.print(tiempo.m);
  Serial.print(F(":"));
  if (tiempo.s < 10) Serial.print('0');
  Serial.print(tiempo.s);
  Serial.print(F(" "));
  if (tiempo.t) Serial.print(F("P"));
  else Serial.print(F("A"));
  Serial.println(F("M."));
}

void sendNTP(IPAddress& address) {
  memset(packetBuffer, 0, NTP);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  serverUDP.beginPacket(address, 123);
  serverUDP.write(packetBuffer, NTP);
  serverUDP.endPacket();
  if (zabavy.debug) Serial.println(F("PAQUETE NTP ENVIADO."));
}

void tick(void) {
  tiempo.s++;
  if (tiempo.s >= 60) {
    tiempo.s = 0;
    tiempo.m++;
    if (tiempo.m >= 60) {
      tiempo.m = 0;
      tiempo.h++;
      if (tiempo.h == 12) tiempo.t = !tiempo.t;
      if ((tiempo.h >= 13) && (tiempo.t)) tiempo.h = 1;
      if ((tiempo.h == 0) && (tiempo.t)) tiempo.h = 12;
    }
  }
  if (zabavy.debug) showTime();
  if (sync-- == 0) {
    if (WiFi.status() == WL_CONNECTED) {
      sendNTP(timeServer);
      if (zabavy.debug) Serial.println(F("SOLICITANDO HORA."));
      sync = tiempo.y;
    } else {
      if (zabavy.debug) Serial.println(F("SIN CONEXION"));
    }
  }
  if (serverUDP.parsePacket()) {
    if (zabavy.debug) Serial.print(F("HORA RECIBIDA. "));
    serverUDP.read(packetBuffer, NTP);
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800;
    unsigned long epoch = secsSince1900 - seventyYears;
    tiempo.h = ((epoch % 86400) / 3600);
    tiempo.m = ((epoch % 3600) / 60);
    tiempo.s = epoch % 60;
    if (tiempo.h < 12) tiempo.t = false;
    if (zabavy.debug) showTime();
    for (unsigned int i = 0; i < tiempo.z; i++) {
      if (tiempo.w) tiempo.h--;
      else tiempo.h++;
    }
    if (tiempo.h > 12) tiempo.t = true;
    else tiempo.t = false;
    if (tiempo.h > 12) tiempo.h -= 12;
    if (tiempo.h == 0) tiempo.h = 12;
  }
  if ((WiFi.status() != WL_CONNECTED) && (zabavy.str_SSID.length() > 0) && (zabavy.str_PASS.length() > 0)) {
    WiFi.begin(zabavy.str_SSID.c_str(), zabavy.str_PASS.c_str());
    if (zabavy.debug) {
      Serial.print(F("INTENTANDO CONECTAR A LA RED WIFI "));
      Serial.print(zabavy.str_SSID.c_str());
      Serial.println(F("."));
    }
  }
}

void formatMemory(void) {
  if (zabavy.debug) Serial.println(F("FORMATEANDO MEMORIA INTERNA."));
  for (int i = 0; i < 1024; i++) {
    EEPROM.write(i, 0x00);
  }
  EEPROM.write(1, 0x01);
  EEPROM.commit();
  if (zabavy.debug) Serial.println();
}

int getRSSIasQuality(int RSSI) {
  int quality = 0;
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
  if (zabavy.debug) Serial.println(F("PAGINA NO ENCONTRADA."));
  String page = FPSTR(HTTP_HEAD);
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADEND);
  page += FPSTR(HTTP_HEADER);
  page += FPSTR(HTTP_NOFOUND);
  page += FPSTR(HTTP_FOOTER);
  page += FPSTR(HTTP_END);
  webServer.send(404, "text/html", page);
  if (zabavy.debug) Serial.println(F("PAGINA NO ENCONTRADA ENVIADA."));
}

void pageRoot(void) {
  if (zabavy.debug) Serial.println(F("CARGANDO PAGINA INICIAL."));
  String page = FPSTR(HTTP_HEAD);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADEND);
  page += FPSTR(HTTP_HEADER);
  page += FPSTR(HTTP_ROOT);
  page += FPSTR(HTTP_FOOTER);
  page += FPSTR(HTTP_END);
  webServer.send(200, "text/html", page);
  if (zabavy.debug) Serial.println(F("PAGINA INICIAL ENVIADA."));
}

void pageWifi(void) {
  if (zabavy.debug) Serial.println(F("CARGANDO PAGINA DE CONFIGURACION DEL WIFI."));
  String page = FPSTR(HTTP_HEAD);
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADEND);
  page += FPSTR(HTTP_HEADER);
  if (zabavy.debug) Serial.println(F("BUSCANDO REDES WIFI."));
  int n = WiFi.scanNetworks();
  if (n == 0) {
    page += FPSTR(HTTP_NONET);
  } else {
    int indices[n];
    for (int i = 0; i < n; i++) {
      indices[i] = i;
    }
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
          std::swap(indices[i], indices[j]);
        }
      }
    }
    String cssid;
    for (int i = 0; i < n; i++) {
      if (indices[i] == -1) continue;
      cssid = WiFi.SSID(indices[i]);
      for (int j = i + 1; j < n; j++) {
        if (cssid == WiFi.SSID(indices[j])) {
          indices[j] = -1;
        }
      }
    }
    page += F("<section><table>");
    if (zabavy.debug) Serial.println(F("ORDENANDO REDES WIFI."));
    for (int i = 0; i < n; i++) {
      if (indices[i] == -1) continue;
      int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));
      if (quality > -1) {
        String item = FPSTR(HTTP_ITEM);
        String rssiQ;
        rssiQ += quality;
        item.replace("{v}", WiFi.SSID(indices[i]));
        item.replace("{r}", rssiQ);
        if (WiFi.encryptionType(indices[i]) != ENC_TYPE_NONE) item.replace("{i}", "l");
        else item.replace("{i}", "");
        page += item;
      }
    }
    page += F("</table>");
  }
  page += FPSTR(HTTP_FORM);
  page += F("</section>");
  page += FPSTR(HTTP_FOOTER);
  page += FPSTR(HTTP_END);
  webServer.send(200, "text/html", page);
  if (zabavy.debug) Serial.println(F("PAGINA DE CONFIGURACION WIFI ENVIADA."));
}

void saveWifi(void) {
  if (zabavy.debug) Serial.println(F("CARGANDO PAGINA DE GUARDADO DE WIFI."));
  zabavy.str_SSID = webServer.arg("S").c_str();
  zabavy.str_PASS = webServer.arg("P").c_str();
  zabavy.str_CODE = webServer.arg("C").c_str();
  zabavy.str_NAME = webServer.arg("N").c_str();
  zabavy.str_DESC = webServer.arg("D").c_str();
  zabavy.on = false;
  zabavy.r = 0;
  zabavy.g = 0;
  zabavy.b = 0;
  if (zabavy.debug) {
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
  String page = FPSTR(HTTP_HEAD);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADEND);
  page += FPSTR(HTTP_HEADER);
  delay(5000);
  if (WiFi.status() == WL_CONNECTED) {
    if (zabavy.debug) Serial.println(F("CONECTADO"));
    formatMemory();
    for (unsigned int i = 0; i < 128; i++) {
      if (zabavy.str_SSID.charAt(i) == '\0') break;
      else EEPROM.write(i + 10, zabavy.str_SSID.charAt(i));
    }
    for (unsigned int i = 0; i < 128; i++) {
      if (zabavy.str_PASS.charAt(i) == '\0') break;
      else EEPROM.write(i + 139, zabavy.str_PASS.charAt(i));
    }
    for (unsigned int i = 0; i < 20; i++) {
      if (zabavy.str_CODE.charAt(i) == '\0') break;
      else EEPROM.write(i + 268, zabavy.str_CODE.charAt(i));
    }
    for (unsigned int i = 0; i < 10; i++) {
      if (zabavy.str_NAME.charAt(i) == '\0') break;
      else EEPROM.write(i + 289, zabavy.str_NAME.charAt(i));
    }
    for (unsigned int i = 0; i < 10; i++) {
      if (zabavy.str_DESC.charAt(i) == '\0') break;
      else EEPROM.write(i + 418, zabavy.str_DESC.charAt(i));
    }
    EEPROM.write(675, (zabavy.on ? 0x01 : 0x00));
    EEPROM.write(677, zabavy.r);
    EEPROM.write(679, zabavy.g);
    EEPROM.write(681, zabavy.b);
    EEPROM.write(3, 'Z');
    EEPROM.write(4, 'A');
    EEPROM.write(5, 'B');
    EEPROM.write(6, 'A');
    EEPROM.write(7, 'V');
    EEPROM.write(8, 'Y');
    EEPROM.commit();
    if (zabavy.debug) Serial.println(F("DATOS GUARDADOS EN MEMORIA INTERNA"));
    if ((EEPROM.read(3) == 'Z') && (EEPROM.read(4) == 'A') && (EEPROM.read(5) == 'B') && (EEPROM.read(6) == 'A') && (EEPROM.read(7) == 'V') && (EEPROM.read(8) == 'Y')) {
      if (zabavy.debug) Serial.println(F("CARGANDO CONFIGURACION."));
      page += F("<section><div><br><br><p>");
      page += WiFi.localIP().toString();
      page += F("<br><br><br><a href='/'>Volver a Inicio</a></p></div></section>");
      if (zabavy.debug) Serial.print(F("DIRECCION IP DEL MODULO: "));
      if (zabavy.debug) Serial.println(WiFi.localIP());
    } else {
      if (zabavy.debug) Serial.println(F("ERROR GUARDANDO INFORMACION EN LA MEMORIA INTERNA."));
      page += FPSTR(HTTP_UNSAVED);
    }
  } else {
    if (zabavy.debug) Serial.println(F("ERROR CONECTANDO A LA RED WIFI."));
    page += FPSTR(HTTP_UNSAVED);
  }
  page += FPSTR(HTTP_FOOTER);
  page += FPSTR(HTTP_END);
  webServer.send(200, "text/html", page);
  if (zabavy.debug) Serial.println(F("PAGINA DE GUARDAR CONFIGURACION ENVIADA."));
}

void pageInfo(void) {
  if (zabavy.debug) Serial.println(F("CARGANDO PAGINA DE INFORMACION."));
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
  item.replace("{r}", WiFi.status() == WL_CONNECTED ? zabavy.str_SSID : F("SIN CONEXIÓN"));
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
  webServer.send(200, "text/html", page);
  if (zabavy.debug) Serial.println(F("PAGINA DE INFORMACION ENVIADA."));
}

void pageState(void) {
  if (zabavy.debug) Serial.println(F("CARGANDO PAGINA DE CAMBIO DE ESTADO."));
  zabavy.on = !zabavy.on;
  String page = FPSTR(HTTP_HEAD);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADEND);
  page += FPSTR(HTTP_HEADER);
  page += F("<section><div><br><br><p>BOMBILLO ");
  page += zabavy.on ? F("ENCENDIDO") : F("APAGADO");
  page += F("<br><br><br><a href='/'>Volver a Inicio</a></p></div></section>");
  page += FPSTR(HTTP_FOOTER);
  page += FPSTR(HTTP_END);
  EEPROM.commit();
  webServer.send(200, "text/html", page);
  if (zabavy.debug) Serial.println(F("PAGINA ENVIADA."));
}

void pageSave(void) {
  if (zabavy.debug) Serial.println(F("CARGANDO PAGINA DE GUARDADO DE ESTADO."));
  String page = FPSTR(HTTP_HEAD);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADEND);
  page += FPSTR(HTTP_HEADER);
  EEPROM.write(677, zabavy.r);
  EEPROM.write(679, zabavy.g);
  EEPROM.write(681, zabavy.b);
  EEPROM.write(675, (zabavy.on ? 0x01 : 0x00));
  EEPROM.write(683, zabavy.anim);
  EEPROM.commit();
  page += F("<section><div><br><br><p>ESTADOS GUARDADOS.<br><br><br><a href='/'>Volver a Inicio</a></p></div></section>");
  page += FPSTR(HTTP_FOOTER);
  page += FPSTR(HTTP_END);
  webServer.send(200, "text/html", page);
  if (zabavy.debug) Serial.println(F("PAGINA ENVIADA."));
}

void pageColor(void) {
  if (zabavy.debug) Serial.println(F("CARGANDO PAGINA DE CAMBIO DE COLOR."));
  String color = webServer.arg("C").c_str();
  int number = (int) strtol(&color[1], NULL, 16);
  zabavy.r = number >> 16;
  zabavy.g = number >> 8 & 0xFF;
  zabavy.b = number & 0xFF;
  zabavy.on = true;
  String page = FPSTR(HTTP_HEAD);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADEND);
  page += FPSTR(HTTP_HEADER);
  page += F("<section><div><br><br><p>COLOR CAMBIADO.<br><br><br><a href='/'>Volver a Inicio</a></p></div></section>");
  page += FPSTR(HTTP_FOOTER);
  page += FPSTR(HTTP_END);
  webServer.send(200, "text/html", page);
  if (zabavy.debug) Serial.println(F("PAGINA ENVIADA."));
}

void pageAnim(void) {
  if (zabavy.debug) Serial.println(F("CARGANDO PAGINA DE CAMBIO DE ANIMACION."));
  zabavy.anim = String(webServer.arg("A").c_str()).toInt();
  zabavy.on = true;
  String page = FPSTR(HTTP_HEAD);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADEND);
  page += FPSTR(HTTP_HEADER);
  page += F("<section><div><br><br><p>ANIMACIÓN CAMBIADA.<br><br><br><a href='/'>Volver a Inicio</a></p></div></section>");
  page += FPSTR(HTTP_FOOTER);
  page += FPSTR(HTTP_END);
  webServer.send(200, "text/html", page);
  if (zabavy.debug) Serial.println(F("PAGINA ENVIADA."));
}

void pageZabavy(void) {
  if (zabavy.debug) Serial.println(F("CARGANDO PAGINA ZABAVY"));
  webServer.send(200, "text/html", zabavy.str_CODE);
  if (zabavy.debug) Serial.println(F("PAGINA ENVIADA."));
}

void getConfig(void) {
  if (zabavy.debug) Serial.println(F("BUSCANDO CONFIGURACION."));
  if ((EEPROM.read(0) == 0x00) && (EEPROM.read(2) == 0x00) && (EEPROM.read(9) == 0x00) && (EEPROM.read(138) == 0x00) && (EEPROM.read(267) == 0x00) && (EEPROM.read(288) == 0x00) && (EEPROM.read(417) == 0x00) && (EEPROM.read(674) == 0x00) && (EEPROM.read(676) == 0x00) && (EEPROM.read(678) == 0x00) && (EEPROM.read(680) == 0x00) && (EEPROM.read(682) == 0x00) && (EEPROM.read(684) == 0x00) && (EEPROM.read(1) == 0x01)) {
    if ((EEPROM.read(3) == 'Z') && (EEPROM.read(4) == 'A') && (EEPROM.read(5) == 'B') && (EEPROM.read(6) == 'A') && (EEPROM.read(7) == 'V') && (EEPROM.read(8) == 'Y')) {
      if (zabavy.debug) Serial.println(F("CONFIGURACION ENCONTRADA."));
      int i = 10;
      while (EEPROM.read(i) != 0x00) {
        zabavy.str_SSID += char(EEPROM.read(i));
        i++;
      }
      i = 139;
      while (EEPROM.read(i) != 0x00) {
        zabavy.str_PASS += char(EEPROM.read(i));
        i++;
      }
      i = 268;
      while (EEPROM.read(i) != 0x00) {
        zabavy.str_CODE += char(EEPROM.read(i));
        i++;
      }
      i = 289;
      while (EEPROM.read(i) != 0x00) {
        zabavy.str_NAME += char(EEPROM.read(i));
        i++;
      }
      i = 418;
      while (EEPROM.read(i) != 0x00) {
        zabavy.str_DESC += char(EEPROM.read(i));
        i++;
      }
      zabavy.on = (EEPROM.read(675) == 1);
      zabavy.r = EEPROM.read(677);
      zabavy.g = EEPROM.read(679);
      zabavy.b = EEPROM.read(681);
      zabavy.anim = EEPROM.read(683);
    } else {
      if (zabavy.debug) Serial.println(F("NO FUE ENCONTRADA NINGUNA CONFIGURACION."));
    }
  } else {
    if (zabavy.debug) Serial.println(F("ERROR EN LA MEMORIA."));
    formatMemory();
  }
  delay(1000);
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for (j = 0; j < 256 * 5; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) {
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, c);
      }
      pixels.show();
      delay(wait);
      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, 0);
      }
    }
  }
}

void theaterChaseRainbow(uint8_t wait) {
  for (unsigned int j = 0; j < 256; j++) {
    for (unsigned int q = 0; q < 3; q++) {
      for (unsigned int i = 0; i < pixels.numPixels(); i += 3) {
        pixels.setPixelColor(i + q, Wheel((i + j) % 255));
      }
      pixels.show();
      delay(wait);
      for (unsigned int i = 0; i < pixels.numPixels(); i += 3) {
        pixels.setPixelColor(i + q, 0);
      }
    }
  }
}

void fire(uint8_t wait) {

}

void startShow() {
  switch (zabavy.anim - 1) {
    case 0:
      colorWipe(pixels.Color(zabavy.r, zabavy.g, zabavy.b), 500);
      colorWipe(pixels.Color(0, 0, 0), 500);
      break;
    case 1:
      theaterChase(pixels.Color(zabavy.r, zabavy.g, zabavy.b), 50);
      break;
    case 2:
      rainbow(15);
      break;
    case 3:
      rainbowCycle(15);
      break;
    case 4:
      theaterChaseRainbow(50);
      break;
    case 5:
      fire(15);
      break;
  }
}
