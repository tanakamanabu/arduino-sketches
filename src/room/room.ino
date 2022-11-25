#include <M5StickC.h>
#include "M5_ENV.h"
#include "mhz19c.h"

enum Status {
  ST_INITIALIZE,
  ST_CALIBRATION,
  ST_SENSOR,
};

SHT3X sht30;
QMP6988 qmp6988;
Mhz19c mhz19c;
int sleepTime;

float pressure = 0;
float tmp = 0;
float hum = 0;
uint16_t co2 = 0;

Status status = ST_INITIALIZE;

void setup() {
  M5.begin(); 
  pinMode(GPIO_NUM_10, OUTPUT); // LEDのON/OFF制御
  led( false );  
  M5.Lcd.setRotation(3);  
  Wire.begin();
  qmp6988.init();
  mhz19c.init( &Serial1, 36, 0 );
  mhz19c.setSelfCalibration( false );
  status = ST_SENSOR;
}

void led( boolean on ){
  digitalWrite( GPIO_NUM_10, on ? LOW : HIGH );
}

void loop() {
  static const int frameTime = 100;
  M5.update();
  if( M5.BtnB.isPressed() ){
    status = ST_CALIBRATION;
  }
  
  delay(frameTime);
  if( sleepTime > 0 ){
    sleepTime -= frameTime;
    return;
  }

  switch(status){
    case ST_CALIBRATION:
      loopCalibration();
      break;
    case ST_SENSOR:
      loopSensor();
      break;
  }
}

void loopCalibration() {
  M5.lcd.fillRect(0, 0, 120, 60, BLACK);
  M5.lcd.setCursor(0, 0);
  M5.lcd.printf("begin calibration.");
 
  led(true);

  //キャリブレーションには20分必要なので21分放置する
  for( int i = 0; i < 21 * 60; ++i ){
    led( i % 2 == 0 );
    delay(1000); 
  }
  mhz19c.calibration();
  led(false);
  delay(2000);

  M5.lcd.fillRect(0, 0, 100, 60, BLACK);
  M5.lcd.setCursor(0, 0);
  M5.lcd.printf("end calibration.");
  delay(1000);
  status = ST_SENSOR;
}

void readSensor() {
    pressure = qmp6988.calcPressure() / 100;
    if (sht30.get() == 0) {
        tmp = sht30.cTemp;
        hum = sht30.humidity;
    } else {
        tmp = 0;
        hum = 0;
    }
    co2 = mhz19c.read();
}

void loopSensor(){
  readSensor();
  int16_t bg,fg;
  if( co2 > 1200 ){
    fg = BLACK;
    bg = RED;
  } else {
    fg = WHITE;
    bg = BLACK;
  }
  M5.lcd.fillScreen(bg);
  M5.lcd.setCursor(0, 0);
  M5.lcd.setTextFont(2);
  M5.lcd.setTextColor(fg,bg);
  M5.lcd.printf("Tem: %2.1f  \r\nHum: %2.0f%%  \r\nPre: %2.2fhPa\r\nCo2: %dppm\r\n",
                  tmp, hum, pressure, co2);

  //10秒ごとにチェックする
  sleepTime = 1000 * 10;
}
