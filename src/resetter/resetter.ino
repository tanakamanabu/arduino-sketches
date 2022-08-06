#include <M5Atom.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

WiFiMulti wifiMulti;

#define STATUS_OFF 0
#define STATUS_CONNECT 1
#define STATUS_SCAN 2
#define STATUS_REBOOT 3
#define TIME_UNIT 50
#define CONNECT_WAIT 1000
#define SCAN_WAIT (10*1000)
#define SCAN_RETRY_COUNT 3
#define REBOOT_WAIT (10*60*1000)
#define RELAY_DELAY 500

#define PIN_RELAY 26

int status = STATUS_OFF;
int waitCount = 0;
int retryCount = 0;


void setup() {
    M5.begin(true,false,true);
    wifiMulti.addAP("","");
    wifiMulti.addAP("","");
    pinMode(PIN_RELAY,OUTPUT);
    digitalWrite(PIN_RELAY,LOW);
    setStatus(STATUS_OFF);
    //Serial.begin(115200);
    delay(50);
}

void loop() {
  checkButton();
  switch(status){
    case STATUS_CONNECT:
      connectMain();
      break;
    case STATUS_SCAN:
      scanMain();
      break;
    case STATUS_REBOOT:
      rebootMain();
      break;
  }
  delay(TIME_UNIT);
}

void checkButton(){
  M5.update();
  if( M5.Btn.wasPressed()){
    switch(status){
      case STATUS_OFF:
        setStatus(STATUS_CONNECT);
        break;
      default:
        setStatus(STATUS_OFF);
        break;
    }
  }  
}

void setStatus(int nextStatus){
    status = nextStatus;
    switch(status){
      case STATUS_CONNECT:
        M5.dis.drawpix(0, 0x00FF00);
        waitCount = 0;
        break;
      case STATUS_SCAN:
        M5.dis.drawpix(0, 0x000000);
        waitCount = 0;
        retryCount = 0;
        break;
      case STATUS_OFF:
        M5.dis.drawpix(0, 0xFF0000);
        break;
      case STATUS_REBOOT:
        M5.dis.drawpix(0, 0x0000FF);
        waitCount = REBOOT_WAIT;
        break;
    }
}

void connectMain() {
  waitCount -= TIME_UNIT;
  if( waitCount > 0 ){
    return;
  }
  waitCount = CONNECT_WAIT;
  int con = wifiMulti.run(); 
  if ( con == WL_CONNECTED){
    setStatus(STATUS_SCAN);
    return;
  }
}

bool scanGoogle(){
  HTTPClient http;
  http.begin("https://www.google.com/");
  int res =  http.GET();
  http.end();
  return res == HTTP_CODE_OK;
}

bool scanRokuga(){
  HTTPClient http;
  http.begin("192.168.1.30",8888);
  int res = http.GET();
  http.end();
  return res == HTTP_CODE_OK;
}

void scanMain(){
  waitCount -= TIME_UNIT;
  if( waitCount > 0 ){
    return;
  }
  
  if (wifiMulti.run() != WL_CONNECTED){
    setStatus(STATUS_CONNECT);
    return;
  }
  waitCount = SCAN_WAIT;
  
  //googleにつながらないということは、そもそもネットワーク死んでる可能性が高いのでチェックしないことにする。
  if( !scanGoogle() ){
    return;
  }

  //接続が確認できたらこの先のエラー処理は行わない
  if( scanRokuga() ){
    retryCount = 0;
    return;
  }

  //リトライ数が一定数になるまでエラー扱いにしない
  ++retryCount;
  if( retryCount < SCAN_RETRY_COUNT ){
    return;
  }

  //リレーを一瞬だけONにしてリセットスイッチを押したような感じにする
  M5.dis.drawpix(0, 0xffffff); 
  digitalWrite(PIN_RELAY,HIGH);
  delay(RELAY_DELAY);
  digitalWrite(PIN_RELAY,LOW);
    
  setStatus(STATUS_REBOOT);
}

void rebootMain(){
  waitCount -= TIME_UNIT;
  if( waitCount > 0 ){
    return;
  }
  setStatus(STATUS_CONNECT);
}
