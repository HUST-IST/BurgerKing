#ifndef _LEDws2812b_h_
#define _LEDws2812b_h_
#include <ws2812b_rpi.h>

void LED_Init(); //LED初始化
//void Ledfarbekontrolieren(int lednummer);//LED闪烁
void LEDchoosing();//选择需要闪烁的LED并使能

#endif
