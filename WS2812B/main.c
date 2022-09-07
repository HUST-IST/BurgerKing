#include <ws2812b_api.h>
#include "wiringPi.h"
#include <signal.h>
#include <pthread.h>
#include "ws2812b_rpi.h"
#include "Socket.h"
#define LED_Delay_Time 100

unsigned int Blinkzahlen = 5; //Ĭ��Blinkzahlen��10��50ms*10 = 500ms��˸һ��
unsigned int t_LED, dt_LED;
int ctrl_c_pressed;
unsigned int temp = 0;              //�����õı���
unsigned char LED_switch = 1;       //led����˸����

extern double Winkel; //���ٶ�arctan�������ƶ������
unsigned char LED_status = 0;       //led��״̬

void ctrlc()
{
    ctrl_c_pressed = 1;
}

PI_THREAD(LED)                //��ʱ���жϺ��� ���Դ���LED��˸��־λ
{
    while (1) {
        dt_LED = millis() - t_LED;
        if (dt_LED >= LED_Delay_Time)
        {
            t_LED = millis();
            if (LED_switch == 1)      //��������˸
            {
                temp++;                 //�����õı���
                if (temp == Blinkzahlen)//�������ﵽ��Ҫ����˸���ʱ��
                {
                    LED_status = 1;       //LED��˸��־λ��1
                    temp = 0;             //������������
                }
                else {
                    LED_status = 0;    //LED��˸��־λ��0
                }
            }
        }
    }
}



int main() {

    LED_Init();//LED��ʼ��

    if (wiringPiSetup() == -1)return 1; //wiringPi��

    t_LED = millis();
    piThreadCreate(LED);
    printf("piThread LED created\n");
    signal(SIGINT, ctrlc);
    for (;;)
    {
        pthread_t stPid = 0;
        pthread_create(&stPid, NULL, tcp_connect_recv, NULL);
        pthread_join(stPid, NULL);

        //Ledauswahlen();
        //if (ctrl_c_pressed)break;
    }

on_finished:

    //�˳����
    endLEDs();
    return 0;
}
