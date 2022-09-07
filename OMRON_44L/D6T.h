#ifndef _D6T_H_
#define _D6T_H_

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>
#include <time.h>
#include <linux/i2c.h> //add

/* defines */
#define N_PIXEL (4 * 4)
#define N_READ ((N_PIXEL + 1) * 2 + 1)
#define D6T_ADDR 0x0A  // for I2C 7bit address
#define D6T_CMD 0x4C  // for D6T-44L-06/06H, D6T-8L-09/09H, for D6T-1A-01/02
extern  uint8_t rbuf1[N_READ], rbuf2[N_READ];
extern  double ptat1, ptat2;
extern  double pix_data1[N_PIXEL], pix_data2[N_PIXEL];

#define N_ROW 4
#define RASPBERRY_PI_I2C    "/dev/i2c-1"
#define I2CDEV              RASPBERRY_PI_I2C

/* I2C functions */
/** <!-- i2c_read_reg8 {{{1 --> I2C read function for bytes transfer.
 */
uint32_t i2c_read_reg8(uint8_t devAddr, uint8_t regAddr,uint8_t* data, int length);

/** <!-- i2c_write_reg8 {{{1 --> I2C read function for bytes transfer.
 */
uint32_t i2c_write_reg8(uint8_t devAddr,uint8_t* data, int length);

uint8_t calc_crc(uint8_t data);

/** <!-- D6T_checkPEC {{{ 1--> D6T PEC(Packet Error Check) calculation.
 * calculate the data sequence,
 * from an I2C Read client address (8bit) to thermal data end.
 */
bool D6T_checkPEC(uint8_t buf[], int n);

int16_t conv8us_s16_le(uint8_t* buf, int n);

#endif