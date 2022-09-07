#include <stdio.h>
#include <math.h>
#include <stdlib.h>

extern double Winkel;                                         //调用Socket.cpp中创建的Winkel变量 表示由速度arctan出来的移动方向角
extern double Winkelz;
extern double data_XYZ[3];
double lastWinkel = 360;
double lastData;
char LED;                                            //储存亮灯的LED编号，用来灭灯
unsigned char LED_status;                                     //led的状态
signed char LEDz = 0;
int lumi;
int lumj;
int LEDi;

char LED_array[16] = {0,7,6,5,4,3,2,1,
                       8,15,14,13,12,11,10,9};             //LED_array 在UR5 机器人坐标系统中的角度clockwise分配为:
            //Unten: 180,135,90,45,0,-45,-90,-135,Oben:...

void LED_Init() //LED初始化
{
    initLEDsPCM(1, 16, 0);                                     //从左至右的形参代表（1条LED灯带，这条LED灯带内有1个LED）
    setMaxBrightness(255);                                    //亮度调节 0-255
}

//void Ledfarbekontrolieren(int lednummer)                    //LED闪烁
//{

  // if (LED_status == 1) {                                  //若LED闪烁标志位为1，则需要LED亮
    //    setG(lednummer, 0, 255);
      //  showLEDs();                                         //在运行完上个函数后，要使用此函数使能led
    //}
    //else {
      //  clearLEDs();                                        //若LED闪烁标志位为0，则需要LED灭 
    //}
//}

    
void LEDchoosing()                                           //选择需要亮的LED并使能
{

    if (abs(Winkelz) >= 80)                                 //执行全亮
    {
        if (abs(lastWinkel - Winkelz) > 0.000000001)
        {
            clearLEDs();
            if (Winkelz > 0)
            {
                for (LEDi = 0; LEDi < 8; LEDi++) setR(LEDi, 0, 255);
            }
            else
            {
                for (LEDi = 8; LEDi < 16; LEDi++) setR(LEDi, 0, 255);
            }
            showLEDs();
        }
        lastWinkel = Winkelz;
    }

    else
    {
        if (Winkelz < -10)
        {
        if ((Winkel > 135 && Winkel <= 180) || Winkel == -180) LED = 8;
        else if (Winkel > 90 && Winkel <= 135) LED = 9;
        else if (Winkel > 45 && Winkel <= 90) LED = 10;
        else if (Winkel > 0 && Winkel <= 45) LED = 11;
        else if (Winkel > -45 && Winkel <= 0) LED = 12;
        else if (Winkel > -90 && Winkel <= -45) LED = 13;
        else if (Winkel > -135 && Winkel <= -90) LED = 14;
        else if (Winkel > -180 && Winkel <= -135) LED = 15;
        }
        
        else
        {
            if ((Winkel > 135 && Winkel <= 180) || (Winkel == -180)) LED = 0;
            else if (Winkel > 90 && Winkel <= 135) LED = 1;
            else if (Winkel > 45 && Winkel <= 90) LED = 2;
            else if (Winkel > 0 && Winkel <= 45) LED = 3;
            else if (Winkel > -45 && Winkel <= 0) LED = 4;
            else if (Winkel > -90 && Winkel <= -45) LED = 5;
            else if (Winkel > -135 && Winkel <= -90) LED = 6;
            else if (Winkel > -180 && Winkel <= -135) LED = 7;
            //printf("LED:%d\n", LED);

            if (Winkelz <= 10 && Winkelz >= -10)
            {
                LEDz = 1;
            }

        }

        
        //Ledfarbekontrolieren(0);                            //LED闪烁


        if (abs(lastWinkel - Winkel) > 0.000000001 || abs(lastData - data_XYZ[0]) > 0.000000001)
        {
            clearLEDs();

            switch (LED)
            {
            case 0:
            {   lumi = (int)(-data_XYZ[0] - data_XYZ[1]);
            lumj = (int)(data_XYZ[1] * sqrt(2));
            setR(LED_array[0], 0, lumi);
            setR(LED_array[1], 0, lumj);
            if (LEDz == 1)
            {
                setR(LED_array[8], 0, lumi);
                setR(LED_array[9], 0, lumj);
            }
            //setG(灯的序号,第几排灯,颜色的数值) //绿色
            //setB(灯的序号,第几排灯,颜色的数值) //蓝色
            //setRGB(灯的序号,第几排灯,RGB) //调色 0-255
            //printf("LED0,lumi:%d,lumj:%d\n", lumi, lumj);
            break;
            }
            case 1:
            {   lumi = (int)(-data_XYZ[0] * sqrt(2));
            lumj = (int)(data_XYZ[0] + data_XYZ[1]);
            setR(LED_array[1], 0, lumi);
            setR(LED_array[2], 0, lumj);
            if (LEDz == 1)
            {
                setR(LED_array[9], 0, lumi);
                setR(LED_array[10], 0, lumj);
            }
            //printf("LED1,lumi:%d,lumj:%d\n", lumi, lumj);
            break;
            }
            case 2:
            {   lumi = (int)(data_XYZ[1] - data_XYZ[0]);
            lumj = (int)(data_XYZ[0] * sqrt(2));
            setR(LED_array[2], 0, lumi);
            setR(LED_array[3], 0, lumj);
            if (LEDz == 1)
            {
                setR(LED_array[10], 0, lumi);
                setR(LED_array[11], 0, lumj);
            }
            //printf("LED2,lumi:%d,lumj:%d\n", lumi, lumj);
            break;
            }
            case 3:
            {   lumi = (int)(data_XYZ[1] * sqrt(2));
            lumj = (int)(data_XYZ[0] - data_XYZ[1]);
            setR(LED_array[3], 0, lumi);
            setR(LED_array[4], 0, lumj);
            if (LEDz == 1)
            {
                setR(LED_array[11], 0, lumi);
                setR(LED_array[12], 0, lumj);
            }
            //printf("LED3,lumi:%d,lumj:%d\n", lumi, lumj);
            break;
            }
            case 4:
            {   lumi = (int)(data_XYZ[0] + data_XYZ[1]);
            lumj = (int)(-data_XYZ[1] * sqrt(2));
            setR(LED_array[4], 0, lumi);
            setR(LED_array[5], 0, lumj);
            if (LEDz == 1)
            {
                setR(LED_array[12], 0, lumi);
                setR(LED_array[13], 0, lumj);
            }
            //printf("LED4,lumi:%d,lumj:%d\n", lumi, lumj);
            break;
            }
            case 5:
            {   lumi = (int)(data_XYZ[0] * sqrt(2));
            lumj = (int)(-data_XYZ[0] - data_XYZ[1]);
            setR(LED_array[5], 0, lumi);
            setR(LED_array[6], 0, lumj);
            if (LEDz == 1)
            {
                setR(LED_array[13], 0, lumi);
                setR(LED_array[14], 0, lumj);
            }
            //printf("LED5,lumi:%d,lumj:%d\n", lumi, lumj);
            break;
            }
            case 6:
            {   lumi = (int)(data_XYZ[0] - data_XYZ[1]);
            lumj = (int)(-data_XYZ[0] * sqrt(2));
            setR(LED_array[6], 0, lumi);
            setR(LED_array[7], 0, lumj);
            if (LEDz == 1)
            {
                setR(LED_array[14], 0, lumi);
                setR(LED_array[15], 0, lumj);
            }
            //printf("LED6,lumi:%d,lumj:%d\n", lumi, lumj);
            break;
            }
            case 7:
            {   lumi = (int)(-data_XYZ[1] * sqrt(2));
            lumj = (int)(data_XYZ[1] - data_XYZ[0]);
            setR(LED_array[7], 0, lumi);
            setR(LED_array[0], 0, lumj);
            if (LEDz == 1)
            {
                setR(LED_array[15], 0, lumi);
                setR(LED_array[8], 0, lumj);
            }
            //printf("LED7,lumi:%d,lumj:%d\n", lumi, lumj);
            break;
            }
            case 8:
            {   lumi = (int)(-data_XYZ[0] - data_XYZ[1]);
            lumj = (int)(data_XYZ[1] * sqrt(2));
            setR(LED_array[8], 0, lumi);
            setR(LED_array[9], 0, lumj);

            break;
            }
            case 9:
            {   lumi = (int)(-data_XYZ[0] * sqrt(2));
            lumj = (int)(data_XYZ[0] + data_XYZ[1]);
            setR(LED_array[9], 0, lumi);
            setR(LED_array[10], 0, lumj);

            break;
            }
            case 10:
            {   lumi = (int)(data_XYZ[1] - data_XYZ[0]);
            lumj = (int)(data_XYZ[0] * sqrt(2));
            setR(LED_array[10], 0, lumi);
            setR(LED_array[11], 0, lumj);

            break;
            }
            case 11:
            {   lumi = (int)(data_XYZ[1] * sqrt(2));
            lumj = (int)(data_XYZ[0] - data_XYZ[1]);
            setR(LED_array[11], 0, lumi);
            setR(LED_array[12], 0, lumj);

            break;
            }
            case 12:
            {   lumi = (int)(data_XYZ[0] + data_XYZ[1]);
            lumj = (int)(-data_XYZ[1] * sqrt(2));
            setR(LED_array[12], 0, lumi);
            setR(LED_array[13], 0, lumj);

            break;
            }
            case 13:
            {   lumi = (int)(data_XYZ[0] * sqrt(2));
            lumj = (int)(-data_XYZ[0] - data_XYZ[1]);
            setR(LED_array[13], 0, lumi);
            setR(LED_array[14], 0, lumj);

            break;
            }
            case 14:
            {   lumi = (int)(data_XYZ[0] - data_XYZ[1]);
            lumj = (int)(-data_XYZ[0] * sqrt(2));
            setR(LED_array[14], 0, lumi);
            setR(LED_array[15], 0, lumj);

            break;
            }
            case 15:
            {   lumi = (int)(-data_XYZ[1] * sqrt(2));
            lumj = (int)(data_XYZ[1] - data_XYZ[0]);
            setR(LED_array[15], 0, lumi);
            setR(LED_array[8], 0, lumj);

            break;
            }
            }
            showLEDs();
        }

        lastWinkel = Winkel;
        lastData = data_XYZ[0];
        LEDz = 0;

    }
}
