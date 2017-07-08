#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include "font8x8.h"

#define PIN_BUTTON   0
#define PIN_LED     D4
#define NUM_LEDS    64


String ip = "";
String scrolltext = " I Love You!..";
uint16_t scrollwait = 75;
uint32_t scrollcolor = 0x002000;
byte scrollindex = 0; // scroll character index
byte j = 0;
byte a = 0;
int lastPos1[NUM_LEDS];
int lastPos2[NUM_LEDS];

long wifiUpdatetime = 0;
long lastupdatetime = 0;
long demotimer = 0;
int ani_speed = 40;
bool demoMode = false;

// Neopixel object
Adafruit_NeoPixel leds = Adafruit_NeoPixel(NUM_LEDS, PIN_LED, NEO_GRB + NEO_KHZ800);

byte demo_index = 0;
uint32_t  color = 0;
String hexcolor = "";
int rgb_color[3] = {0, 0, 0};
byte brightness = 80;       // default brightness: 48  80 = 1A, 200 = 3,2A, 255 = 3,6A,
uint32_t pixels[NUM_LEDS];  // pixel buffer. this buffer allows you to set arbitrary
// brightness without destroying the original color values

ESP8266WebServer server(80);  // create web server at HTTP port 80

// Forward declare functions
String get_ap_name();
//void button_handler();
void on_status();
void on_change_color();
void on_change_ani();
void on_homepage();
void show_leds();
void hexToRGB();
void demo_wipe();
void demo_rainbow();
void demo_scrolltext();
void cross();
void test();
void pictures();
void MoveEyesLR();
void sparkle();
void clearPIXELS();
void tunnel();

void show_leds() {
  uint32_t r, g, b;
  uint32_t c;
  for (byte i = 0; i < NUM_LEDS; i++) {
    r = (pixels[i] >> 16) & 0xFF;
    g = (pixels[i] >> 8) & 0xFF;
    b = (pixels[i]) & 0xFF;
    r = r * brightness / 255;
    g = g * brightness / 255;
    b = b * brightness / 255;
    c = (r << 16) + (g << 8) + b;
    leds.setPixelColor(i, c);
  }
  leds.show();
}

//dreht jede zweite zeile um

uint8_t numToPos(int num) {
  int x = num % 8;
  int y = num / 8;
  return xyToPos(x, y);
}

uint8_t xyToPos(int x, int y) {
  if (y % 2 == 0) {
    return (y * 8 + x);
  } else {
    return (y * 8 + (7 - x));
  }
}

void setAll(uint16_t color) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    leds.setPixelColor(i, color);
  }
  show_leds();
}

void hexToRGB(String hexcolor) {

  String hexstring = "#" + hexcolor;

  // Get rid of '#' and convert it to integer
  long number = (long) strtol( &hexstring[1], NULL, 16);
    
  // Split them up into r, g, b values
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  color = leds.Color(r, g, b);
  Serial.println(r);
  Serial.println(g);
  Serial.println(b);
  Serial.println(color);
  Serial.println(leds.Color(255,0,255));
  Serial.println(leds.Color(r, g, b));
  Serial.println("");
}

//#####################################################################

void setup() {
  Serial.begin(115200);

  // Set pin mode
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LED,    OUTPUT);

  // Initialize LEDs
  leds.begin();
  leds.show();


  // Setup new Wifi Manager
  String ap_name = get_ap_name();
  WiFiManager wifiManager;
  wifiManager.setTimeout(180);
  if (!wifiManager.autoConnect(ap_name.c_str(), "pixelmatrix")) {
    // Set WiFi SSID
    WiFi.persistent(false);
    WiFi.softAP(ap_name.c_str(), "pixelmatrix");
    WiFi.mode(WIFI_AP);
    scrolltext = " I Love You!..";
  }
  WiFi.hostname("Pixelmatrix");

  // Set server callback functions
  server.on("/",   on_homepage);
  server.on("/js", on_status);
  server.on("/cc", on_change_color);
  server.on("/ani", on_change_ani);
  server.begin();

  // Set button handler
  //  attachInterrupt(PIN_BUTTON, button_handler, FALLING);
  ip = WiFi.localIP().toString();
  scrolltext = "!! " + ip + " !!";
}

// The variable below is modified by interrupt service routine
// so declare it as volatile
volatile boolean button_clicked = false;

void loop() {
  if (millis() - wifiUpdatetime > 75) {
    server.handleClient();
    wifiUpdatetime = millis();
  }

  if (demoMode == true) {
    if (millis() - demotimer > 30000) {
      demo_index++;
      if (demo_index > 5) {
        demo_index = 0;
      }
      demotimer = millis();
    }
  }

  if (millis() - lastupdatetime > ani_speed) {
    switch (demo_index) {
      case 0:
        if (scrolltext.length() > 0) {
          demo_scrolltext();
          if (j >= scrolltext.length() - 1) {
            j = 0;
          }
        }
        break;
      case 1:
        test();
        break;
      case 2:
        demo_rainbow();
        if (j > NUM_LEDS) {
          j = 0;
        }
        break;
      case 3:
        demo_wipe();
        break;
      case 4:
        cross();
        break;
      case 5:
        pictures();
        break;
      case 6:
        demoMode = true;
        break;
      case 7:
        demoMode = false;
        demo_index = 1;
        break;
      case 8:
        //sparkle(120, 30, 80);
        tunnel();
        break;
      default:
        demo_index = 0;
        break;
    }
    lastupdatetime = millis();
  }
  /*  if(button_clicked) {
      demo_index ++;  // upon button click, proceed to the next demo
      button_clicked = false;
    }
  */
}

/* ----------------
   Color Wipe Demo
   ---------------- */

void wipe(uint32_t c) {
  uint32_t r, g, b;
  r = (c >> 16) & 0xFF;
  g = (c >> 8) & 0xFF;
  b = (c) & 0xFF;
  r = r * brightness / 255;
  g = g * brightness / 255;
  b = b * brightness / 255;
  c = (r << 16) + (g << 8) + b;
  //for (byte i = 0; i < NUM_LEDS; i++) {
  leds.setPixelColor(j, c);
  leds.show();
  j++;
}

void demo_wipe() {
  static byte idx = 0;
  switch (idx) {
    case 0:
      wipe(0xFF0000);
      break;
    case 1:
      wipe(0x00FF00);
      break;
    case 2:
      wipe(0x0000FF);
      break;
    case 3:
      wipe(0x00F050);
      break;
    case 4:
      wipe(0xFF00FF);
      break;
    case 5:
      wipe(0xFFFF00);
      break;
    case 6:
      wipe(0x00FFFF);
      break;
    case 7:
      wipe(0xFFEE00);
      break;
    default:
      wipe(0x101010);
      break;
  }
  if (j >= NUM_LEDS) {
    idx = (idx + 1) % 8;
    j = 0;
  }
}

/* ----------------
     Rainbox Demo
   ---------------- */

uint32_t wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return leds.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return leds.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return leds.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void demo_rainbow() {
  static byte idx = 0;
  for (byte i = 0; i < NUM_LEDS; i++) {
    pixels[i] = wheel((i + idx) & 0xFF);
  }
  show_leds();
  idx++;
}

/* ----------------
   Scroll Text Demo
   ---------------- */
void drawChar(char c, int offset) {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint64_t mask = 1LL << (uint64_t)i;

    if ( (font[c]&mask) == 0) {
      leds.setPixelColor(numToPos(NUM_LEDS - 1 - i), leds.Color(0, 0, 0)); //bit is 0 at pos i
    } else {
      /*float _brightness = 1.0 - ( (TotalSteps - Index) * 1.0 / TotalSteps );
        uint8_t _r = (uint8_t)(Color1 >> 16);
        uint8_t _g = (uint8_t)(Color1 >>  8);
        uint8_t _b = (uint8_t)Color1; */
      leds.setPixelColor(numToPos(NUM_LEDS - 1 - i), scrollcolor); //bit is 1 at pos i
    }
  }
}


void demo_scrolltext() {
  j++;
  drawChar(scrolltext.charAt(j), 0);
  leds.show();
  delay(500);
}

// #############################################

// Cross Animation

void cross() {
  if (j >= 8) {
    j = 0;
    color = color + 37;
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels[i] = leds.Color(0, 0, 0);
  }
  for (int u = 0; u < 8; u++) {
    pixels[xyToPos(j, u)] = wheel(color);
  }
  for (int u = 0; u < 8; u++) {
    pixels[xyToPos(u, j)] = wheel(color);
  }

  j++;
  show_leds();
}



void test() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels[i] = leds.Color(0, 0, 0);
  }
  if (j >= 8) {
    j = 0;
    a++;
    color = color + 37;
  }
  if (a >= 8) {
    a = 0;
  }
  pixels[xyToPos(j, a)] = wheel(color);
  j++;

  show_leds();
}
//Move Eyes Function

void MoveEyesLR(int posL, int posR, int r, int g, int b) {
  int lastColor = pixels[posL];
  int posL2 = 0;
  int posR2 = 0;
  leds.setPixelColor(posL, leds.Color(r, g, b));
  leds.setPixelColor(posR, leds.Color(r, g, b));
  delay(100);
  leds.show();
  delay(900);
  posL2 = posL + 1;
  posR2 = posR + 1;
  leds.setPixelColor(posL2, leds.Color(r, g, b));
  leds.setPixelColor(posR2, leds.Color(r, g, b));
  leds.setPixelColor(posL, lastColor);
  leds.setPixelColor(posR, lastColor);
  leds.show();
  delay(1000);
}

// MINECRAFT Pictures

void pictures() {
  String mc_cow = "602000602000602000602000602000602000602000602000602000602000602000602000602000602000602000602000FFFFFFFFFFFF602000602000602000602000FFFFFFFFFFFFFFFFFFFFFFFF602000602000602000602000FFFFFFFFFFFF602000602000602000602000602000602000602000602000602000602000FFFFFFFFFFFFFFFFFFFFFFFF602000602000602000FFFFFF000000444444444444000000FFFFFF602000602000FFFFFF444444444444444444444444FFFFFF602000";
  String mc_creeper = "00AA0000AA0000AA0000AA0000AA0000AA0000AA0000AA0000AA0000AA0000AA0000AA0000AA0000AA0000AA0000AA0000AA0000000000000000AA0000AA0000000000000000AA0000AA0000000000000000AA0000AA0000000000000000AA0000AA0000AA0000AA0000000000000000AA0000AA0000AA0000AA0000AA0000000000000000000000000000AA0000AA0000AA0000AA0000000000000000000000000000AA0000AA0000AA0000AA0000000000AA0000AA0000000000AA0000AA00";
  String mc_sheep = "999999AAAAAABBBBBB999999CCCCCCAAAAAA888888DDDDDDBBBBBB999999CCCCCCAAAAAA888888DDDDDD999999AAAAAACCCCCCF7CEAAF7CEAAF7CEAAF7CEAAF7CEAAF7CEAADDDDDDAAAAAA000000FFFFFFF7CEAAF7CEAAFFFFFF000000EEEEEEEEEEEEF7CEAAF7CEAAF7CEAAF7CEAAF7CEAAF7CEAABBBBBBCCCCCCBBBBBBF7CEAA000000000000F7CEAA888888999999888888999999F7CEAAF7CEAAF7CEAAF7CEAACCCCCCBBBBBB999999AAAAAABBBBBB999999CCCCCCAAAAAA888888DDDDDD";
  String smilie = "000000000000FFDD00FFDD00FFDD00FFDD00000000000000000000FFDD00000000000000000000000000FFDD00000000FFDD00000000FFDD00000000FFDD00000000000000FFDD00FFDD00000000FFDD00000000FFDD00000000000000FFDD00FFDD00000000000000000000000000FFDD00000000FFDD00FFDD00000000FFDD00FFDD00FFDD00000000000000FFDD00000000FFDD00000000000000000000000000FFDD00000000000000000000FFDD00FFDD00FFDD00FFDD00000000000000";

  switch (j) {
    case 0:
      ShowPixel(mc_creeper);
      clearPIXELS();
      MoveEyesLR(25, 29, 255, 0, 0);
      delay(2000);
      break;
    case 1:
      ShowPixel(mc_cow);
      delay(2000);
      break;
    case 2:
      ShowPixel(mc_sheep);
      delay(2000);
      break;
    case 3:
      ShowPixel(smilie);
      delay(2000);
      break;

    default:
      j = -1;
      break;
  }
  j++;
}


void ShowPixel(String val) {
  byte c = 0;
  int z = 0;
  uint16_t i;
  for (i = 0; i < NUM_LEDS; i++) {
    byte r = i / 8;
    byte c = i % 8;
    if (r % 2 == 0) {
      z = r * 8 + c;
    }
    else
    {
      z = r * 8 + 7 - c;
    }
    pixels[z] = strtol(val.substring(i * 6, i * 6 + 6).c_str(), NULL, 16);
  }
  show_leds();
}


void clearPIXELS() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels[i] = 0;
  }
}
//###############################################


void tunnel() {

  bool inToOut = true;
  clearPIXELS();

  int bSize = sqrt(NUM_LEDS) - a;

  for (int e = 0; e < bSize; e++) {
    for (int i = 0; i < bSize; i++) {
      if (e == 0 || e == bSize - 1 || i == 0 || i == bSize - 1) {
        pixels[xyToPos((a / 2) + i, (a / 2) + e)] = color;
        //Draw Pixel
      }
    }
  }

  if (inToOut) {
    if (a <= 0) { // Adapt if uneven number of leds
      a = sqrt(NUM_LEDS);
    } else {
      a -= 2; // This too
    }

  }
  else
  {

    if (sqrt(NUM_LEDS) - a < 2) { // Adapt if uneven number of leds
      a = 0;
    } else {
      a += 2; // This too
    }
  }
  show_leds();
}

/*

   Sparkle Effekt

*/

/*
  void sparkle(int r, int g, int b){
  if(j>=256){
    a=1;
  }
  if(j<=0){
    a=0;
  }
  switch(a){
    case 0:
      if(j<256){
        setAll(r,g,b);
        leds.show();
        j++;
      }
      break;
    case 1:
      if(j>256){
        setAll(r,g,b);
        leds.show();
        j--;
      }
      break;
  }
  }
*/

/*

  void drawCol(char c, int i){
  if(i >= 0 && i < 8){
    for(int j = 0; j < 8; j++){
        if(c & 0x1){
          leds.setPixelColor(i*8+j, scrollcolor);
        } else {
          leds.setPixelColor(i*8+j, 0);
        }
        c >>= 1;
    }
  }
  }

  void drawChar(char c, int offset){
  char col;
  for(int i = 0; i < 8; i++){
    if(i - offset >= 0 && i - offset < 8){
      col = pgm_read_byte(IMAGES+(c*8)+i);
      drawCol(col,i - offset);
    }
  }
  }

  void demo_scrolltext() {

  if(j < 6){
    j = j++;
    for(byte k = 0; k < 2; k++){
      drawChar(scrolltext.charAt(scrollindex+k),j - k*6);
      drawCol(0,5-j+k*6);
    }
    delay(scrollwait);
    leds.show();
  }

  scrollindex = (scrollindex+1)%scrolltext.length();
  }
*/
/* ----------------
    WebServer Demo
   ---------------- */

#include "html.h"
void on_homepage() {
  String html = FPSTR(index_html);
  server.send(200, "text/html", html);
}

// this returns device variables in JSON, e.g.
// {"pixels":xxxx,"blink":1}
void on_status() {
  String html = "";
  html += "{\"brightness\":";
  html += brightness;
  html += "{\"demo_index\":";
  html += demo_index;
  html += "{\"demoMode\":";
  html += demoMode;
  html += "{\"ani_speed\":";
  html += ani_speed;
  html += "}";
  server.send(200, "text/html", html);
}

void on_change_color() {
  uint16_t i;
  Serial.println("Change Color Triggert");
  if (server.hasArg("pixels")) {
    int z = 0;
    String val = server.arg("pixels");
    for (i = 0; i < NUM_LEDS; i++) {
      // every pixel color is 6 bytes storing the hex value
      // pixels are specified in row-major order
      // here we need to flip it to column-major order to
      // match the physical connection of the leds
      // byte r=i/8, c=i%8;
      // pixels[c*8+r] = strtol(val.substring(i*6, i*6+6).c_str(), NULL, 16);
      byte r = i / 8, c = i % 8;
      if (r % 2 == 0) {
        z = r * 8 + c;
      }
      else
      {
        z = r * 8 + 7 - c;
      }
      pixels[z] = strtol(val.substring(i * 6, i * 6 + 6).c_str(), NULL, 16);
    }
    scrolltext = "";
  }
  if (server.hasArg("clear")) {
    for (i = 0; i < NUM_LEDS; i++) {
      pixels[i] = 0;
    }
    scrolltext = "";
  }
  if (server.hasArg("brightness")) {
    brightness = server.arg("brightness").toInt();
    Serial.println("brightness:");
    Serial.println(brightness);
  }
  if (server.hasArg("scrolltext")) {
    scrolltext = server.arg("scrolltext");
    scrolltext = " " + scrolltext;

    Serial.println("scrolltext:");
    Serial.println(scrolltext);

    scrollindex = 0;
    if (server.hasArg("wait")) {
      scrollwait = server.arg("wait").toInt();
      if (scrollwait < 8) scrollwait = 8;
    }
    if (server.hasArg("color")) {
      scrollcolor = strtol(server.arg("color").c_str(), NULL, 16);
    }
  }
  if (server.hasArg("speed")) {
    ani_speed = server.arg("speed").toInt();
    Serial.println("ani_speed:");
    Serial.println(ani_speed);
  }
  if (scrolltext.length() == 0) {
    show_leds();
    server.send(200, "text/html", "{\"result\":1}");
  }
}
void on_change_ani() {
  if (server.hasArg("ani")) {
    int val = 0;
    val = server.arg("ani").toInt();
    demo_index = val;
    Serial.println("demo:" + demo_index);
  }
  if (server.hasArg("color")) {
    String val;
    val = server.arg("color");
    hexcolor = val;
    Serial.println("HexColor übergeben von Webseite:" + hexcolor);
    hexToRGB(hexcolor);
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels[i] = 0;
  }
  j = 0;
  a = 0;
}


char dec2hex(byte dec) {
  if (dec < 10) return '0' + dec;
  else return 'A' + (dec - 10);
}

// AP name is ESP_ following by
// the last 6 bytes of MAC address
String get_ap_name() {
  static String ap_name = "";
  if (!ap_name.length()) {
    byte mac[6];
    WiFi.macAddress(mac);
    ap_name = "PixelMatrix_";
    for (byte i = 3; i < 6; i++) {
      ap_name += dec2hex((mac[i] >> 4) & 0x0F);
      ap_name += dec2hex(mac[i] & 0x0F);
    }
  }
  return ap_name;
}

/*
  void button_handler() {
  button_clicked = true;
  }
*/

