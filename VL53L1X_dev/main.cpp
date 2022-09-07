/*
 *  RPLIDAR
 *  Ultra Simple Data Grabber Demo App
 *
 *  Copyright (c) 2009 - 2014 RoboPeak Team
 *  http://www.robopeak.com
 *  Copyright (c) 2014 - 2019 Shanghai Slamtec Co., Ltd.
 *  http://www.slamtec.com
 *
 */
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "signal.h"
#include <wiringPi.h>
#include <VL53L1X_api.h>
#include "vl53l1_platform.h"
#include "smbus.h"
#include "VL53L1X_calibration.h"

void print_error(const char* msg) {
    printf("[E] %s\n", msg);
    exit(1);
};


bool ctrl_c_pressed;
void ctrlc(int)
{
    ctrl_c_pressed = true;
}


int main(int argc, const char * argv[]) {

    VL53L1X_Version_t version;
    uint8_t ret, dataReady, rangeStatus;
    uint16_t distance;
    uint16_t dev = 0x29;  // Address on i2c bus

    if(-1==wiringPiSetup())
    {
        printf("wiringPi setup error\n");
        exit(-1);
    }

//VL53L1X 

    ret = 0;
    if (VL53L1X_BootState(dev, &ret) != 0)print_error("Boot state");
    if (ret == 0) {
        printf("Not booted\n");
        return 0;
    }

    if (VL53L1X_SensorInit(dev) != 0) {
        print_error("Sensor init");
    }

// Ms
    if (VL53L1X_SetInterMeasurementInMs(dev, 100) != 0)
        print_error("Set inter measurement");

// (1=short, 2=long(default)
    if (VL53L1X_SetDistanceMode(dev, 1) != 0)
        print_error("Set distance mode");

// mm
    if (VL53L1X_SetOffset(dev, 28) != 0)
        print_error("Set offset");

// enable the ranging
    if (VL53L1X_StartRanging(dev) != 0)
        print_error("Start ranging");

    signal(SIGINT, ctrlc);
    
    while (1) 
    {
	
        while (dataReady == 0)
        {
            if (VL53L1X_CheckForDataReady(dev, &dataReady) != 0)
                print_error("Check data ready");
        }
        dataReady = 0;

        if (VL53L1X_GetRangeStatus(dev, &rangeStatus) != 0)print_error("Get range status");

        printf("Range status: %i\n", rangeStatus);

        if (VL53L1X_GetDistance(dev, &distance) != 0)print_error("Get distance");

        printf("Distance (mm): %i\n", distance);

        if (VL53L1X_ClearInterrupt(dev) != 0)print_error("Clear interrupt");

        if (ctrl_c_pressed)break;
    }
    return 0;
}

