/*
Based on the TCA9548A datasheet.
Author: Bryan Casanelli - bryancasanelli@gmail.com
*/

#include "rpi_tca9548a.h"
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

rpi_tca9548a::rpi_tca9548a(){
    wiringPiSetup();
}

rpi_tca9548a::~rpi_tca9548a(){
  no_channel();
}

int rpi_tca9548a::init(int id){
  this->fd = wiringPiI2CSetup(id);
  return this->fd;            // Ok
}

void rpi_tca9548a::set_channel(uint8_t channel){
  unsigned char last_channel = 255;
  switch (channel)
  {
  case 0:
      if (last_channel != 0) {
          wiringPiI2CWriteReg8(this->fd, 0x04, 0x01);//TCA9548A bus#0
          last_channel = 0;
      }
      break;
  case 1:
      if (last_channel != 1) {
          wiringPiI2CWriteReg8(this->fd, 0x04, 0x02);//TCA9548A bus#1
          last_channel = 1;
      }
      break;
  case 2:
      if (last_channel != 2) {
          wiringPiI2CWriteReg8(this->fd, 0x04, 0x04);//TCA9548A bus#2
          last_channel = 2;
      }
      break;
  case 3:
      if (last_channel != 3) {
          wiringPiI2CWriteReg8(this->fd, 0x04, 0x08);//TCA9548A bus#3
          last_channel = 3;
      }
      break;
  case 4:
      if (last_channel != 4) {
          wiringPiI2CWriteReg8(this->fd, 0x04, 0x10);//TCA9548A bus#4
          last_channel = 4;
      }
      break;
  case 5:
      if (last_channel != 5) {
          wiringPiI2CWriteReg8(this->fd, 0x04, 0x20);//TCA9548A bus#5
          last_channel = 5;
      }
      break;
  case 6:
      if (last_channel != 6) {
          wiringPiI2CWriteReg8(this->fd, 0x04, 0x40);//TCA9548A bus#6
          last_channel = 6;
      }
      break;
  case 7:
      if (last_channel != 7) {
          wiringPiI2CWriteReg8(this->fd, 0x04, 0x80);//TCA9548A bus#7
          last_channel = 7;
      }
      break;
  default:
      no_channel();
  }
}

void rpi_tca9548a::no_channel(){
    wiringPiI2CWriteReg8(this->fd, 0x04, 0x00);
}
