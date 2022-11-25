#pragma once
#include <M5StickC.h>
#include <stdint.h>

class Mhz19c {
  private:
    enum CONST {
      COMMAND_SIZE = 8,
      RESPONSE_SIZE = 9,
    };
    HardwareSerial* serial;
    uint8_t cmdRead[COMMAND_SIZE]               = {0xFF, 0x01, 0x86, 0x00, 0, 0, 0, 0}; 
    uint8_t cmdRunCalibration[COMMAND_SIZE]     = {0xFF, 0x01, 0x87, 0x00, 0, 0, 0, 0}; 
    uint8_t cmdSelfCalibrationOn[COMMAND_SIZE]  = {0xFF, 0x01, 0x79, 0xA0, 0, 0, 0, 0}; 
    uint8_t cmdSelfCalibrationOff[COMMAND_SIZE] = {0xFF, 0x01, 0x79, 0x00, 0, 0, 0, 0}; 
    uint8_t calcCheckSum(uint8_t cmd[]);
    boolean writeCommand(uint8_t cmd[], uint8_t *response);
    
  public:
    void init(HardwareSerial* serial, int8_t rx, int8_t tx);
    
    uint16_t read();

    void calibration();
    void setSelfCalibration(boolean on);
};
