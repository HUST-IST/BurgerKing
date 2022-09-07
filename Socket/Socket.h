#ifndef _Socket_h_
#define _Socket_h_
#include <pthread.h>
#include <sys/socket.h>
extern "C" {
#include "ws2812b_rpi.h"
#include "LEDws2812b.h"
}

extern int iSocketFD;//socket���
extern int new_fd;//�������Ӻ�ľ��
extern int iRecvLen;//���ճɹ���ķ���ֵ
extern double data_XYZ[3];//����xyz�����ٶȵ�����
extern double Winkel;//���ٶ�arctan�������ƶ������
extern bool Warner;
extern char send_buf[3];
extern double Angle_RP_XY;
void* tcp_connect_send(void* args);
void* tcp_connect_recv(void* args);


#endif
