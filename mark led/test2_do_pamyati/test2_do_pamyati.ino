#include "FastLED.h"          // библиотека для работы с лентой
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define LED_COUNT 12          // число светодиодов в кольце/ленте
#define LED_DT D3             // пин, куда подключен DIN ленты


/* Установите здесь свои SSID и пароль */
const char* ssid = "Mark II";       // SSID
const char* password = "74123skom";  // пароль

/* Настройки IP адреса */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

bool LED1status = LOW;
bool LED2status = LOW;
int demo = 0;

int max_bright = 10;          // максимальная яркость (0 - 255)

const int butPinLeft = 12;  //№ пина для подключения Левого поворота
const int butPinRight = 13;  //№ пина для подключения Правого поворота
const int butPinPolic = 14;  //№ пина для подключения мигалки

// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------
int BOTTOM_INDEX = 0;        // светодиод начала отсчёта
int TOP_INDEX = int(LED_COUNT / 2);
int EVENODD = LED_COUNT % 2;
struct CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];     //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, MARCH, ETC)

int thisdelay = 20;          //-FX LOOPS DELAY VAR
int thisstep = 10;           //-FX LOOPS DELAY VAR
int thishue = 0;             //-FX LOOPS DELAY VAR
int thissat = 255;           //-FX LOOPS DELAY VAR

int thisindex = 0;
int thisRED = 0;
int thisGRN = 0;
int thisBLU = 0;

int idex = 0;                //-LED INDEX (0 to LED_COUNT-1
int ihue = 0;                //-HUE (0-255)
int ibright = 0;             //-BRIGHTNESS (0-255)
int isat = 0;                //-SATURATION (0-255)
int bouncedirection = 0;     //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;          //-INC VAR FOR SIN LOOPS
int lcount = 0;              //-ANOTHER COUNTING VAR
// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------

void setup() {

Serial.begin(115200);
EEPROM.begin(100);



  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.on("/action_page", handleForm);
  server.on("/action_bri", handleBri);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Сохраненная яркость");
  EEPROM.get(0, max_bright);
  Serial.println(max_bright);

pinMode(butPinLeft, INPUT_PULLUP); 
pinMode(butPinRight, INPUT_PULLUP); 
pinMode(butPinPolic, INPUT_PULLUP);

digitalWrite(butPinLeft, HIGH); 
digitalWrite(butPinRight, HIGH);  
digitalWrite(butPinPolic, HIGH);



  LEDS.setBrightness(max_bright);  // ограничить максимальную яркость

  LEDS.addLeds<WS2812, LED_DT, GRB>(leds, LED_COUNT);  // настрйоки для нашей ленты (ленты на WS2811, WS2812, WS2812B)
  one_color_all(0, 0, 0);          // погасить все светодиоды
  LEDS.show();                     // отослать команду
  
}

void one_color_all(int cred, int cgrn, int cblu) {       //-SET ALL LEDS TO ONE COLOR
  for (int i = 0 ; i < LED_COUNT; i++ ) {
    leds[i].setRGB( cred, cgrn, cblu);
  }
}

void Pravo(int cred, int cgrn, int cblu, int SpeedDelay) {
  for (int i = TOP_INDEX; i > 0; i-- ) {
    leds[i].setRGB( cred, cgrn, cblu);
    LEDS.show();
    delay(SpeedDelay);
 }
}

void Levo(int cred, int cgrn, int cblu, int SpeedDelay) {       //-SET ALL LEDS TO ONE COLOR
  
    for (int i = TOP_INDEX; i > 0; i-- ) {
    int ih = horizontal_index(i);
    leds[ih].setRGB( cred, cgrn, cblu);
    LEDS.show();
    delay(SpeedDelay);
  }
}

void Avariyka(int cred, int cgrn, int cblu, int SpeedDelay) {       //-SET ALL LEDS TO ONE COLOR
  
    for (int i = TOP_INDEX; i >= 0; i-- ) {
    leds[i].setRGB( cred, cgrn, cblu);
    int ih = horizontal_index(i);
    leds[ih].setRGB( cred, cgrn, cblu);
    LEDS.show();
    delay(SpeedDelay);
 }
}


void loop() { 
  
    
        // ЛЕВЫЙ ПОВОРОТ
    if (digitalRead(butPinLeft) == HIGH && digitalRead(butPinRight) == LOW) {
      Levo(255, 255, 0, 5);
           
     }

     
         // Правый ПОВОРОТ
    if (digitalRead(butPinLeft) == LOW && digitalRead(butPinRight) == HIGH) {
      Pravo(255, 255, 0, 5);   
     }


// АВАРИЙКА
    if (digitalRead(butPinLeft) == LOW && digitalRead(butPinRight) == LOW) {
    Avariyka(255, 255, 0, 5);
     }


 
   // Ментовские войны
    if (digitalRead(butPinPolic) == LOW) {
     ems_lightsSTROBE();
     }


 server.handleClient();
 
 switch (demo) {
    case 1: flame(); break;
    case 2: color_bounceFADE(); break;
    case 3: ems_lightsSTROBE(); break;
    case 4: Avariyka(30, 30, 0, 5); break;
    case 5: rainbow_fade(); break;
    case 6: pulse_one_color_all(); break; 
    case 7: demo_modeA(); break; 
    case 8: demo_modeB(); break;
    case 9: kitt(); break;
  }
 
  if(LED1status == HIGH) {
    color_bounceFADE();
     }
  
  if(LED2status == HIGH){
   flame();

    }

     

else if (LED1status == LOW && LED2status == LOW && demo == 0) {
       delay(100);
       one_color_all(10, 10, 10);
     LEDS.show();

     }
     
}

void handle_OnConnect() 
{ 
  Serial.print("GPIO7 Status: ");
  if(LED1status)
    Serial.print("ON");
  else
    Serial.print("OFF");

  Serial.print(" | GPIO6 Status: ");
  if(LED2status)
    Serial.print("ON");
  else
    Serial.print("OFF");

  Serial.println("");
  server.send(200, "text/html", SendHTML(LED1status,LED2status)); 
}

void handleForm() { 
String dam = server.arg("demon"); 

String den = dam; 
Serial.print("First Name:");
 Serial.println(den);

 if (den == "0") {
    demo = 0;
     }
 if (den == "1") {
    demo = 1;
     }
 if (den == "2") {
    demo = 2;
     }
 if (den == "3") {
    demo = 3;
     }
 if (den == "4") {
    demo = 4;
     }
 if (den == "5") {
    demo = 5;
     }
 if (den == "6") {
    demo = 6;
     }
 if (den == "7") {
    demo = 7;
     }
 if (den == "8") {
    demo = 8;
     }
 if (den == "9") {
    demo = 9;
     }

 Serial.print("STAT:");
 Serial.println(demo);
 
 server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Updated– Press Back Button");
}

void handleBri() { 
int bri = atol(server.arg("brg").c_str());

Serial.print("SEC Bri:");
 Serial.println(bri);

EEPROM.put(0, bri);
EEPROM.commit();
EEPROM.get(0, max_bright);

 Serial.print("EEPROM:");
 Serial.println(max_bright);
 
 LEDS.setBrightness(max_bright);
 
 server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Updated– Press Back Button");
}

void handle_led1on() 
{
  LED1status = HIGH;
  Serial.println("GPIO7 Status: ON");
  server.send(200, "text/html", SendHTML(true,LED2status)); 
}

void handle_led1off() 
{
  LED1status = LOW;
  Serial.println("GPIO7 Status: OFF");
  server.send(200, "text/html", SendHTML(false,LED2status)); 
}

void handle_led2on() 
{
  LED2status = HIGH;
  Serial.println("GPIO6 Status: ON");
  server.send(200, "text/html", SendHTML(LED1status,true)); 
}

void handle_led2off() 
{
  LED2status = LOW;
  Serial.println("GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,false)); 
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat, uint8_t led2stat)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<meta charset=\"utf-8\">\n";
  ptr +="<title>Toyota Mark II</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Toyota Mark II</h1>\n";
  ptr +="<h3>Управление фарами</h3>\n";
  ptr +="<br>\n";
  ptr +="<br>\n";

  
  ptr +="<form action=\"/action_bri\">\n";
  ptr +="<p>Настройка яркости</p>\n";
  ptr +="<input type=\"range\" name=\"brg\" min=\"0\" max=\"255\" step=\"1\" value=\""; ptr += EEPROM.read(0); ptr +="\">\n";
  ptr += EEPROM.read(0);
  ptr +="<p><input type=\"submit\" value=\"Сохранить\"></p>\n";
  ptr +="</form>\n";
  ptr +="<br>\n";


  ptr +="<form action=\"/action_page\">\n";
  ptr +="<p>Выбери эффект</p><select name=\"demon\">\n";
  ptr +="<option value=\"0\" selected>Выключенно</option>\n";
  ptr +="<option value=\"1\" >Радуга переливающаяся</option>\n";
  ptr +="<option value=\"2\" >Бегущие прыгающие</option>\n";
  ptr +="<option value=\"3\" >Менты</option>\n";
  ptr +="<option value=\"9\" >Прыгающий красный</option>\n";
  ptr +="<option value=\"4\" >Аварийка</option>\n";
  ptr +="<option value=\"5\" >Радуга</option>\n";
  ptr +="<option value=\"6\" >Пульс</option>\n";
  ptr +="<option value=\"7\" >Демо А</option>\n";
  ptr +="<option value=\"8\" >Демо Б</option>\n";
  ptr +="</select>\n";
  ptr +="<p><input type=\"submit\" value=\"Запустить\"></p>\n";
  ptr +="</form>\n";
  
  if(led1stat)
    ptr +="<p>Эффект включен</p><a class=\"button button-off\" href=\"/led1off\">выключить </a>\n";
  else
    ptr +="<p>Эффект выключен</p><a class=\"button button-on\" href=\"/led1on\">включить </a>\n";

  if(led2stat)
    ptr +="<p>Мигалка включенна</p><a class=\"button button-off\" href=\"/led2off\">выключить  </a>\n";
  else
    ptr +="<p>Мигалка выключенна</p><a class=\"button button-on\" href=\"/led2on\">включить </a>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
