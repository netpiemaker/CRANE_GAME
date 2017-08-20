#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "qrcode.h"

#define APPID   "YOUR_APPID"
#define KEY     "YOUR_KEY"
#define SECRET  "YOUR_SECRET"
#define ALIAS   "espcranegame"

#define KEY_APP     "YOUR_KEY_APP"
#define SECRET_APP  "YOUR_SECRET_APP"

#define LEDPIN     2

#define TFT_DC    15
#define TFT_CS    12
#define TFT_RST   5
#define TFT_CLK   13
#define TFT_MOSI  14

#define APDEVICENAME "PIECRANE"
#define TCRANEGAME "/gearname/espcranegame"
#define TOPIC_CRANEGAME "/" APPID TCRANEGAME

Ticker ticker;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);
QRcode qrcode;

String SESSION = "";
String readString = "";
const char delimiter = ',';   // ใช้สำหรับเช็ครูปแบบข้อความที่คั่นด้วยเครื่องหมาย comma
int numBytes = 10;
String letters[40] = {"a", "b", "c", "d", "e", "f","g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
bool STARTED = false;
bool READY = true;
unsigned long timestart = 0;
unsigned long timeend = 0;
int timeouts = 30000;
float counter = -1.0;

int ix=0, iy=0;
bool modeColor = true;
unsigned long dot = 0;
int pot = 500;

// Configure WiFi
bool resetMode = false;
int timeoutDisconnection = 90000;
unsigned long timer = 0;
unsigned long timeoutConnection = 0;

WiFiManager wifiManager;
WiFiClient client;
MicroGear microgear(client);

void tick()
{
  #ifdef LEDPIN
    int stateled = digitalRead(LEDPIN);
    digitalWrite(LEDPIN, !stateled);
  #endif
}

String generatorString(){
  String randString = "";
  for(int i = 0; i<numBytes; i++){
    randString = randString + letters[random(0, 40)];
  }
  Serial.print("Session id : ");
  Serial.println(randString);
  return randString;
}

void setText(int x, int y, int s, String text){
  qrcode.screenwhite();
  tft.setCursor(x, y); 
  tft.setTextColor(ST7735_BLACK);  
  tft.setTextSize(s);
  tft.println(text);
}

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
//  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  String m = (char*)msg;                 // ตัวแปรเก็บข้อความชนิด string

  if(String(topic).equals(TOPIC_CRANEGAME) && m.endsWith(SESSION+":START") && !STARTED && SESSION!=""){
    STARTED = true;
    timestart = millis();
    timeend = millis()+timeouts;
    counter = (timeend - timestart)/1000;
    int dec = counter - (int)counter;
    if(dec>=50) counter+=1;
    String mt = "Timeout in "+String(int(counter))+" sec.";
    setText(2, 75, 1, mt);
//    Serial.print("Timeout in ");
//    Serial.println(int(counter));
  }else if(String(topic).equals(TOPIC_CRANEGAME) && m.startsWith(SESSION+":SVO") && STARTED && SESSION!=""){
    int start_str = SESSION.length()+4;
    String command = "";
    for(int i=start_str; i<msglen; i++){
      if(msg[i]==delimiter){
        Serial.println(command);
        command="";
      }else{
        command+=(char)msg[i];
      }
    }
  }else if(String(topic).equals(TOPIC_CRANEGAME) && m.endsWith(SESSION+":PICK") && STARTED && SESSION!=""){
    Serial.println("PICK");
    Serial.println("PICK");
    STARTED = false;
    SESSION = "";
    setText(2, 75, 1, "Wait to new game");
  }
}

void resetCrane(){
  Serial.println("RESET");
  READY = true;
  STARTED = false;
  SESSION = "";
  counter = -1.0;
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  setText(2, 75, 1, "Connected to NETPIE");
//  setText(2, 75, 1, "Waiting to generate QR Code");

  /* Set the alias of this microgear ALIAS */
  microgear.setAlias(ALIAS);  
  
  #ifdef LEDPIN
    ticker.detach();
    digitalWrite(LEDPIN, LOW);
  #endif
  resetCrane();
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("*PM: Entered config mode");
  Serial.print("*PM: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  #ifdef LEDWIFI
    ticker.attach(0.25, tick);
  #endif
}

void initWiFi(){
  setText(2, 75, 1, "Configuration WiFi");
  
  wifiManager.setTimeout(180);
  if(resetMode) wifiManager.resetSettings();
  wifiManager.setAPCallback(configModeCallback);
  resetMode = false;
  
  if(!wifiManager.autoConnect(APDEVICENAME)) {
    Serial.println("failed to connect and hit timeout");
    delay(5000);
  }
}

void setup() {    
  
  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);

  Serial.begin(9600);
  Serial.println("Starting...");
  timer = millis();
  
  pinMode(LEDPIN, OUTPUT);

  tft.initR(INITR_BLACKTAB);
  qrcode.init();
  
  initWiFi();
  
  /* Initial with KEY, SECRET and also set the ALIAS here */
  microgear.init(KEY,SECRET,ALIAS);

  /* connect to NETPIE to a specific APPID */
  if(WiFi.status()==WL_CONNECTED){
    ticker.attach(0.05, tick);
    setText(2, 75, 1, "Connecting to NETPIE");
    microgear.connect(APPID);
  }
}

void loop() {

  if(resetMode) initWiFi();
  
  if(WiFi.status()!=WL_CONNECTED){
      setText(2, 75, 1, "Disconnect WiFi");
      Serial.println("disconnect WiFi.");
      initWiFi();
  }else{
    if (microgear.connected()) {
      microgear.loop();
      
      if(READY && !STARTED){
        setText(2, 75, 1, "Create QR Code");
        SESSION = generatorString();
        if(SESSION!=""){
          qrcode.create("https://rawgit.com/netpiemaker/CRANE_GAME/master/HTML5/cranegame.html#"+(String)APPID+":"+(String)KEY_APP+":"+(String)SECRET_APP+":"+SESSION);
          READY=false;
        }
      }
      
      if(STARTED && !READY){
        float ct = (timeend - millis())/1000;
        int dec = ct - (int)ct;
        if(int(ct)!=int(counter) && dec<=0){
          counter = ct;
          if(dec>=50) counter+=1;
          String mt = "Timeout in "+String(int(counter))+" sec.";
          setText(2, 75, 1, mt);
          Serial.print("Timeout in ");
          Serial.println(int(counter));
        }

        if(int(counter)==0){
           STARTED = false;
           SESSION = "";
           counter = -1.0;
           setText(2, 75, 1, "TIMEOUT!!!");
           Serial.println("RESET");
        }
      }
    }
    else {
      Serial.println("connection lost, reconnect...");
      if(WiFi.status()==WL_CONNECTED){
        setText(2, 75, 1, "Disconnect NETPIE, reconnect...");
        ticker.attach(0.05, tick);
        microgear.connect(APPID);
      }
      if(timeoutConnection==0){
        timeoutConnection = millis();
      }
      if(millis()-timeoutConnection>=timeoutDisconnection){
        if(WiFi.status()==WL_CONNECTED){
          WiFi.disconnect();
        }
        timeoutConnection=0;
        initWiFi();
      }
    }
  }

  readString = "";
  while(Serial.available()) {
    delay(2);  //delay to allow byte to arrive in input buffer
    char c = Serial.read();
    readString += c;
    if(readString == "NEWGAME") {
      Serial.println("NEW");
      resetCrane();
      readString = "";
    }
  }
}
