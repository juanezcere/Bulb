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
const char HTTP_ROOT[] PROGMEM = "<section><form action='/wifi' metod='get'><button>Configurar WiFi</button></form><br><form action='/color' metod='get'><table><thead><th><input type='color' id='color' name='C'/></th><th><button type='submit'>Enviar color</button></th></thead></table></form><br><form action='/anim' metod='get'><table><thead><th><select name='A'><option value=0>Ninguna animaci??n</option><option value=1>Movimiento</option><option value=2>Destello</option><option value=3>Arcoiris</option><option value=4>Arcoiris individual</option><option value=5>Parpadeo multicolor</option><option value=6>Fuego</option></select></th><th><button type='submit'>Enviar animaci??n</button></th></thead></table></form><br><form action='/state' metod='get'><button>Cambiar estado</button></form><br><form action='/save' metod='get'><button>Guardar estado</button></form><br><form action='/info' metod='get'><button>Informaci??n</button></form></section>";
const char HTTP_FOOTER[] PROGMEM = "<footer><p>Zabavy INC. Todos los derechos reservados.</p></footer>";
const char HTTP_NONET[] PROGMEM = "<section><div><p>No hay redes WiFi visibles en este momento, recargue esta p??gina para volver a buscar redes WiFi.</p></div></section>";
const char HTTP_UNSAVED[] PROGMEM = "<section><div><p>Las configuraciones del dispositivo no pudieron ser guardadas.<br>Si el problema persiste comuniquese con nosotros.</p></div></section>";
const char HTTP_ITEM[] PROGMEM = "<tr><td><a href='#P' onclick='c(this)'>{v}</a></td><td><span class='q {i}'>{r}%</span></td></tr>";
const char HTTP_FORM[] PROGMEM = "<div><form method='get' action='wifisave'><input type='text' id='S' name='S' length=128 placeholder='NOMBRE DE RED WIFI' required/><br><input type='password' id='P' name='P' length=128 placeholder='CONTRASE??A' required/><br><input type='text' id='C' name='C' length=20 placeholder='C??DIGO DEL DISPOSITIVO' required/><br><input type='text' id='N' name='N' length=128 placeholder='NOMBRE'/><br><input type='text' id='D' name='D' length=256 placeholder='DESCRIPCI??N'/><br><button type='submit'>Guardar</button></form></div>";
const char HTTP_INFO[] PROGMEM = "<tr><td><p>{v}</p></td><td><p>{r}</p></td></tr>";
const char HTTP_NOFOUND[] PROGMEM = "<section><div><br><br><p>UPS... Al parecer la p??gina que buscas no se encuentra disponible.</p></div></section>";
const char HTTP_END[] PROGMEM = "</body></html>";

const String MENSAJES[] = {
    "Hello, welcome to Zabavy.",
    "Initing Zabavy. Wait a moment, please.",
    "Asking for the time and weither.",
    "There is not connection to WiFi network.",
    "The time is here.",
    "Formatting internal storage. Wait a moment, please."
};

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
    struct Tiempo {
        uint8_t h = 1;
        uint8_t m = 0;
        uint8_t s = 0;
        uint8_t z = 5;
        uint16_t y = 600;
        boolean t = false;
        boolean w = true;
    } tiempo;
} zabavy;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NEO_C, NEO_D, NEO_GRB + NEO_KHZ800);
unsigned int sync = 0;
ESP8266WebServer webServer(80);
IPAddress timeServer(129, 6, 15, 28);
byte packetBuffer[NTP];
WiFiUDP serverUDP;
Ticker sec;

void printDebug(int msj){
    if(zabavy.debug) Serial.println(MENSAJES[msj]);
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
}

void tick(void){
    zabavy.tiempo.s++;
    sync--;
    if(zabavy.tiempo.s >= 60){
        zabavy.tiempo.s = 0;
        zabavy.tiempo.m++;
        if(zabavy.tiempo.m >= 60){
            zabavy.tiempo.m = 0;
            zabavy.tiempo.h++;
            if(zabavy.tiempo.h == 12) zabavy.tiempo.t = !zabavy.tiempo.t;
            if((zabavy.tiempo.h >= 13) && (zabavy.tiempo.t)) zabavy.tiempo.h = 1;
            if((zabavy.tiempo.h == 0) && (zabavy.tiempo.t)) zabavy.tiempo.h = 12;
        }
    }
    if(sync == 0){
        if(WiFi.status() == WL_CONNECTED) {
            sendNTP(timeServer);
            printDebug(2);
            sync = zabavy.tiempo.y;
        } else {
            printDebug(3);
        }
    }
    if(serverUDP.parsePacket()){
        printDebug(4);
        serverUDP.read(packetBuffer, NTP);
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        const unsigned long seventyYears = 2208988800;
        unsigned long epoch = secsSince1900 - seventyYears;
        zabavy.tiempo.h = ((epoch % 86400) / 3600);
        zabavy.tiempo.m = ((epoch % 3600) / 60);
        zabavy.tiempo.s = epoch % 60;
        if(zabavy.tiempo.h < 12) zabavy.tiempo.t = false;
        for (unsigned int i = 0; i < zabavy.tiempo.z; i++) {
            if (zabavy.tiempo.w) zabavy.tiempo.h--;
            else zabavy.tiempo.h++;
        }
        if (zabavy.tiempo.h > 12) zabavy.tiempo.t = true;
        else zabavy.tiempo.t = false;
        if (zabavy.tiempo.h > 12) zabavy.tiempo.h -= 12;
        if (zabavy.tiempo.h == 0) zabavy.tiempo.h = 12;
    }
}

void formatMemory(void) {
    printDebug(5);
    for (int i = 0; i < 1024; i++) {
        EEPROM.write(i, 0x00);
    }
    EEPROM.write(1, 0x01);
    EEPROM.commit();
  }