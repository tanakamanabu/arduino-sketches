#include "mhz19c.h"

boolean Mhz19c::writeCommand(uint8_t cmd[], uint8_t *response = NULL) {
  //バッファにゴミが残ってることがあるので、送信前にクリアする
  while( serial->available() ){
    uint8_t dummy = serial->read();
  }
  //コマンド＋チェックサムを送信する
  serial->write( cmd, COMMAND_SIZE );
  serial->write( calcCheckSum( cmd ) );
  serial->flush();

  //レスポンス不要なのでここで終了
  if( response == NULL ){
    return true;
  }

  //レスポンスが受信できるようになるまで待つ
  int i = 0;
  while ( serial->available() < RESPONSE_SIZE ) {
    if( ++i > 100 ){
      return true; //レスポンスは無かった
    }
    delay(10);
  }
  serial->readBytes( response, RESPONSE_SIZE);
  return calcCheckSum( response ) == response[ RESPONSE_SIZE - 1 ];
}

uint8_t Mhz19c::calcCheckSum(uint8_t cmd[]) {
  uint8_t sum = 0;
  for( int i = 1; i < COMMAND_SIZE; ++i ){
    sum = sum + cmd[i];
  }
  return 0xff - sum + 1;
}

void Mhz19c::init(HardwareSerial* serial, int8_t rx, int8_t tx) {
  this->serial = serial;
  serial->begin(9600, SERIAL_8N1, rx, tx);
  delay(500);
}

uint16_t Mhz19c::read(){
  uint8_t response[RESPONSE_SIZE] = {0};
  if( writeCommand( cmdRead, response ) && response[0] == 0xff && response[1] == 0x86 ) {
    return (response[2] << 8 ) + response[3];
  }

  return 0;
}


void Mhz19c::calibration(){
  writeCommand( cmdRunCalibration );
}

void Mhz19c::setSelfCalibration(boolean on){
  if( on ){
    writeCommand( cmdSelfCalibrationOn );
  } else {
    writeCommand( cmdSelfCalibrationOff );
  }
}
