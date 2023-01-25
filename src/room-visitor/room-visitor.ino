#include "Arduino.h"
#include <FastLED.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "env.h"

#define NUM_LEDS 1
#define DATA_PIN 27
#define RIP_PIN 33
#define RIP_GPIO_NUM GPIO_NUM_33
CRGB leds[NUM_LEDS];

void setup() {
  pinMode( RIP_PIN, INPUT );
  switch(esp_sleep_get_wakeup_cause()){
    case ESP_SLEEP_WAKEUP_EXT0:
      wakePin();
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      wakeTimer();
      break;
    default:
      first();
      break;
  }
}

// 初回起動
void first(){
  //適当にチカチカさせて自己主張する
  FastLED.addLeds<SK6812, DATA_PIN, RGB>(leds, NUM_LEDS); // GRB ordering is typical
  for( int i = 0; i < 30; ++i ){
    delay(250);
    leds[0] = 0xf0f000;
    FastLED.show();
    delay(250);
    leds[0] = 0x00f0f0;
    FastLED.show();
  }
  
  leds[0] = 0x000000;
  FastLED.show();
  
  //RIPセンサが反応したらDeepSleepから起きる
  esp_sleep_enable_ext0_wakeup(RIP_GPIO_NUM,HIGH);
  esp_deep_sleep_start();
}

//RIPセンサーが反応して起動した
void wakePin(){
  //Wifiでサーバーへ来客を告げる
  WiFi.begin( WIFI_SSID, WIFI_PASSWORD );
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }
  sendData();

  //一度来客があったら5分間は眠る
  esp_deep_sleep(5*60*1000*1000);
}

//タイマーから起動した。来客後の５分間スリープからの起床
void wakeTimer(){
  if( digitalRead(RIP_PIN) == 1 ){
    //既に誰かいるのでサーバーへ来客を告げる
    wakePin();
  } else {
    //周りにだれもいないのでRIPセンサが反応するまでDeepSleep
    esp_sleep_enable_ext0_wakeup(RIP_GPIO_NUM,HIGH);
    esp_deep_sleep_start();
  }
}

boolean sendData(){
  HTTPClient client;
  String url = HOST;
  url += "/api/add_person";
  client.begin( url );
  client.addHeader( "Content-Type", "application/json" );
  String queryString = "{";
  queryString += "\"place_id\": \"2\",";
  queryString += "\"pass\": \"";
  queryString += PASS;
  queryString += "\"}";
  
  int statusCode = client.POST(queryString);
  client.end();

  return statusCode == 200;
}

void loop() {
}
