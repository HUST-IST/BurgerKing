#ifndef _Socket_h_
#define _Socket_h_
#include <pthread.h>
#include <sys/socket.h>
extern "C" {
#include "ws2812b_rpi.h"
#include "LEDws2812b.h"
}

extern int iSocketFD;//socket句柄
extern int new_fd;//建立连接后的句柄
extern int iRecvLen;//接收成功后的返回值
extern double data_XYZ[3];//储存xyz轴向速度的数组
extern double Winkel;//由速度arctan出来的移动方向角
extern bool Warner;
extern char send_buf[3];
extern double Angle_RP_XY;
void* tcp_connect_send(void* args);
void* tcp_connect_recv(void* args);


#endif
