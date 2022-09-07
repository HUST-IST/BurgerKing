extern double Winkel;                                         //调用Socket.cpp中创建的Winkel变量 表示由速度arctan出来的移动方向角
char lastled;                                                 //储存亮灯的LED编号，用来灭灯
unsigned char LED_status;                                     //led的状态

void LED_Init() //LED初始化
{
    initLEDsPCM(1, 16, 0);                                     //从左至右的形参代表（1条LED灯带，这条LED灯带内有1个LED）
    setMaxBrightness(255);                                    //亮度调节 0-255
}

//void Ledfarbekontrolieren(int lednummer)                    //LED闪烁
//{
//
//    if (LED_status == 1) {                                  //若LED闪烁标志位为1，则需要LED亮
//        setR(lednummer, 0, 255);
//        showLEDs();                                         //在运行完上个函数后，要使用此函数使能led
//    }
//    else {
//        clearLEDs();                                        //若LED闪烁标志位为0，则需要LED灭 
//    }
//}

void Ledauswahlen()                                           //选择需要闪烁的LED并使能
{
    if ((lastled != 0) && (Winkel > 135) && (Winkel < 180)){  //判断机器人移动方向 
        clearLEDs();                                          //将记录的LED灭灯
        //Ledfarbekontrolieren(0);                            //LED闪烁
        setR(0, 0, 255);
        setR(8, 0, 255);
        showLEDs();
        lastled = 0;                                          //记录此次闪烁的LED编号
    }
    else if ((lastled != 1) && (Winkel > 91) && (Winkel < 135)){
        clearLEDs();                                          //将记录的LED灭灯
        //Ledfarbekontrolieren(1);
        setR(1, 0, 255);
        setR(9, 0, 255);
        showLEDs();
        lastled = 1;
    }
    else if ((lastled != 2) && (Winkel > 45) && (Winkel < 89)){
        clearLEDs();                                          //将记录的LED灭灯
        //Ledfarbekontrolieren(2);
        setR(2, 0, 255);
        setR(10, 0, 255);
        showLEDs();
        lastled = 2;
    }
    else if ((lastled != 3) && (Winkel > 1) && (Winkel < 45)){
        clearLEDs();                                          //将记录的LED灭灯
        //Ledfarbekontrolieren(3);
        setR(3, 0, 255);
        setR(11, 0, 255);
        showLEDs();
        lastled = 3;
    }
    else if ((lastled != 4) && (Winkel > -45) && (Winkel < 0)){
        clearLEDs();                                          //将记录的LED灭灯
        //Ledfarbekontrolieren(4);
        setR(4, 0, 255);
        setR(12, 0, 255);
        showLEDs();
        lastled = 4;
    }
    else if ((lastled != 5) && (Winkel > -89) && (Winkel < -45)){
        clearLEDs();                                          //将记录的LED灭灯
        //Ledfarbekontrolieren(5);
        setR(5, 0, 255);
        setR(13, 0, 255);
        showLEDs();
        lastled = 5;
    }

    else if ((lastled != 6) && (Winkel > -135) && (Winkel < -91)){
        clearLEDs();                                          //将记录的LED灭灯
        //Ledfarbekontrolieren(6);
        setR(6, 0, 255);
        setR(14, 0, 255);
        showLEDs();
        lastled = 6;
    }

    else if ((lastled != 7) && (Winkel > -180) && (Winkel < -135)){
        clearLEDs();                                          //将记录的LED灭灯
        //Ledfarbekontrolieren(7);
        setR(7, 0, 255);
        setR(15, 0, 255);
        showLEDs();
        lastled = 7;
    }
}
