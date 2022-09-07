#include <ws2812b_api.h>
#include "wiringPi.h"
#include <signal.h>
#include <pthread.h>
#include "ws2812b_rpi.h"
#include "Socket.h"
#define LED_Delay_Time 100

unsigned int Blinkzahlen = 5; //默认Blinkzahlen是10，50ms*10 = 500ms闪烁一次
unsigned int t_LED, dt_LED;
int ctrl_c_pressed;
unsigned int temp = 0;              //计数用的变量
unsigned char LED_switch = 1;       //led的闪烁开关

extern double Winkel; //由速度arctan出来的移动方向角
unsigned char LED_status = 0;       //led的状态

void ctrlc()
{
    ctrl_c_pressed = 1;
}

PI_THREAD(LED)                //定时器中断函数 用以处理LED闪烁标志位
{
    while (1) {
        dt_LED = millis() - t_LED;
        if (dt_LED >= LED_Delay_Time)
        {
            t_LED = millis();
            if (LED_switch == 1)      //若允许闪烁
            {
                temp++;                 //计数用的变量
                if (temp == Blinkzahlen)//若计数达到需要的闪烁间隔时间
                {
                    LED_status = 1;       //LED闪烁标志位置1
                    temp = 0;             //计数变量清零
                }
                else {
                    LED_status = 0;    //LED闪烁标志位置0
                }
            }
        }
    }
}



int main() {

    LED_Init();//LED初始化

    if (wiringPiSetup() == -1)return 1; //wiringPi库

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

    //退出清空
    endLEDs();
    return 0;
}
