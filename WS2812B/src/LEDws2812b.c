#include <stdio.h>
#include <math.h>
#include <stdlib.h>

extern double Winkel;                                         //����Socket.cpp�д�����Winkel���� ��ʾ���ٶ�arctan�������ƶ������
extern double Winkelz;
extern double data_XYZ[3];
double lastWinkel = 360;
double lastData;
char LED;                                            //�������Ƶ�LED��ţ��������
unsigned char LED_status;                                     //led��״̬
signed char LEDz = 0;
int lumi;
int lumj;
int LEDi;

char LED_array[16] = {0,7,6,5,4,3,2,1,
                       8,15,14,13,12,11,10,9};             //LED_array ��UR5 ����������ϵͳ�еĽǶ�clockwise����Ϊ:
            //Unten: 180,135,90,45,0,-45,-90,-135,Oben:...

void LED_Init() //LED��ʼ��
{
    initLEDsPCM(1, 16, 0);                                     //�������ҵ��βδ���1��LED�ƴ�������LED�ƴ�����1��LED��
    setMaxBrightness(255);                                    //���ȵ��� 0-255
}

//void Ledfarbekontrolieren(int lednummer)                    //LED��˸
//{

  // if (LED_status == 1) {                                  //��LED��˸��־λΪ1������ҪLED��
    //    setG(lednummer, 0, 255);
      //  showLEDs();                                         //���������ϸ�������Ҫʹ�ô˺���ʹ��led
    //}
    //else {
      //  clearLEDs();                                        //��LED��˸��־λΪ0������ҪLED�� 
    //}
//}

    
void LEDchoosing()                                           //ѡ����Ҫ����LED��ʹ��
{

    if (abs(Winkelz) >= 80)                                 //ִ��ȫ��
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

        
        //Ledfarbekontrolieren(0);                            //LED��˸


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
            //setG(�Ƶ����,�ڼ��ŵ�,��ɫ����ֵ) //��ɫ
            //setB(�Ƶ����,�ڼ��ŵ�,��ɫ����ֵ) //��ɫ
            //setRGB(�Ƶ����,�ڼ��ŵ�,RGB) //��ɫ 0-255
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
