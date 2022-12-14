#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <byteswap.h>

#include "ads1115.h"

float adsMaxGain[8] = {
  6.144,
  4.096,
  2.048,
  1.024,
  0.512,
  0.256,
  0.256,
  0.256
};

float getADS1115MaxGain(int gain) {
    if (gain < 0) {
        return -1;
    }
    if (gain > 7) {
        return -1;
    }
    return adsMaxGain[gain];
}
int getSingeShotSingleEndedConfig(int pin, int gain) {
    int high = 1 << 7 | 1 << 6 | 1;
    high |= pin << 4;
    high |= gain << 1;

    int low = 0x83;

    return high << 8 | low;
}

float readVoltage(int handle, int pin) {
    uint16_t  rslt = 0;
    uint16_t config = getSingeShotSingleEndedConfig(pin, 0);
    wiringPiI2CWriteReg16(handle, 0x01, __bswap_16(config));
    //delay(1);

    rslt = __bswap_16(wiringPiI2CReadReg16(handle, 0x01));
    while ((rslt & 0x8000) == 0) {  // wait for data ready
        //delay(1);
        rslt = __bswap_16(wiringPiI2CReadReg16(handle, 0x01));
    }

    rslt = __bswap_16(wiringPiI2CReadReg16(handle, 0x00));

    if (rslt > 32767)rslt = 32767;

    return adsMaxGain[0] * rslt / 32767.0;
}
