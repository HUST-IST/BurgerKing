#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "rpi_tca9548a.h"
#include <ads1115rpi.h>
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <VL53L1X_api.h>
#include "vl53l1_platform.h"
#include "smbus.h"
#include "VL53L1X_calibration.h"
#include "D6T.h"
#include "rplidar.h"
#include "SharedMemory.h"
#include <cstring>
#include <algorithm>
#include "Socket.h"                         //因为Socket要调用WS2812B，所以主函数不用调用WS2812B.h
#include "MyTimer.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

#define THERMO_SWITCH 0x01
#define SELF_CHECKING_ON 0x02
#define CLOSED_MONITORING_MODEL 0x03
#define OPEN_MONITORING_MODEL 0x04
#define h 55
#define l_max 16
#define H (62.5 * h)/(62.5 - l_max)
int data[512], angle[512], matrix_index, ignore_angle_start_index, ignore_angle_end_index,ignore_mode,ignore_angle, filter_index;
int check_flag = 0;
int D_RP = 15, D_EN, MAX_THERMOL;
double D,SEND_BUFF_OUT=1.0f, OUT[2] = { 1.0 };
bool ctrl_c_pressed, stop, stop_main;                         //记录Ctrc+C状态的变量
int S_IN_RP[4] = { 100,80,50,30 };
double UR_OUT_RP[4] = { 0.8, 0.6, 0.4, 0.2 };
int S_IN_EN[4] = { 450, 350, 250, 150 };
double UR_OUT_EN[4] = { 0.8, 0.6, 0.4, 0.2 };

void print_error(const char* msg) {
    printf("[E] %s\n", msg);
    ctrl_c_pressed = true;
}

void reverse(int *array, int left, int right)//用于旋转RPLidar的数据
{
    int temp;
    for (; left < right; left++, right--) 
    {
        temp = array[right];
        array[right] = array[left];
        array[left] = temp;
    }
}

int min,ave,dif,filter_matrix[6];
void* RPLidar(void* args)                                                          //TCP Socket接收数据的独立线程
{
    #define Matrixverschiebtzahl 101 //RPLidar的数据在网页中显示时旋转的度数
    using namespace rp::standalone::rplidar;
    const char* opt_com_path = "/dev/ttyUSB0";
    _u32         baudrateArray[2] = { 115200, 256000 };
    _u32         opt_com_baudrate = 0;
    u_result     op_result;
    int RPLIDAR_Zahl;
    bool useArgcBaudrate = false;
    RPlidarDriver* drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
    if (!drv) {
        fprintf(stderr, "insufficent memory, exit\n");
        exit(-1);
    }
    rplidar_response_device_info_t devinfo;
    bool connectSuccess = false;
    // make connection...
    if (useArgcBaudrate)
    {
        if (!drv)drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
        if (IS_OK(drv->connect(opt_com_path, opt_com_baudrate)))
        {
            op_result = drv->getDeviceInfo(devinfo);

            if (IS_OK(op_result))connectSuccess = true;
            else
            {
                delete drv;
                drv = NULL;
            }
        }
    }
    else
    {
        size_t baudRateArraySize = (sizeof(baudrateArray)) / (sizeof(baudrateArray[0]));

        for (size_t i = 0; i < baudRateArraySize; ++i)
        {
            if (!drv) drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);

            if (IS_OK(drv->connect(opt_com_path, baudrateArray[i])))
            {
                op_result = drv->getDeviceInfo(devinfo);

                if (IS_OK(op_result))
                {
                    connectSuccess = true;
                    break;
                }
                else
                {
                    delete drv;
                    drv = NULL;
                }
            }
        }
    }
    if (!connectSuccess) {

        fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n", opt_com_path);
        exit(-1);
    }

    drv->startMotor();
    drv->startScan(0, 1);
    
    while(1) {
        /**********************RPLidar*********************/
        rplidar_response_measurement_node_hq_t nodes[8192];
        size_t   count = _countof(nodes);

        op_result = drv->grabScanDataHq(nodes, count);
        if (IS_OK(op_result))
        {
            drv->ascendScanData(nodes, count);
            data[38] = count;
            for (int pos = 0; pos < (int)count; pos++)
            {
                if (nodes[pos].quality == 0)data[pos + 39] = 400;
                else data[pos + 39] = (int)(nodes[pos].dist_mm_q2 / 4.0f) / 10;
                angle[pos] = nodes[pos].angle_z_q14 * 90.f / (1 << 14);
            }

            int temp_matrix[count];
            for (int i = 0; i < count; i++) temp_matrix[i] = data[39 + i];
            reverse(temp_matrix, 0, count - Matrixverschiebtzahl - 1);
            reverse(temp_matrix, count - Matrixverschiebtzahl, count - 1);
            reverse(temp_matrix, 0, count - 1);
            for (int i = 0; i < count; i++) data[39+i] = temp_matrix[i]; 

            if ((int)Angle_RP_XY == 0)
            {
                for (int i = 0; i < count; i++)
                {
                    if (angle[i] > 127)
                    {
                        ignore_angle_start_index = i;
                        break;
                    }
                }
                for (int i = 0; i < count; i++)
                {
                    if (angle[i] > 217)
                    {
                        ignore_angle_end_index = i;
                        break;
                    }
                }
                for (int i = ignore_angle_start_index; i < ignore_angle_end_index; i++)data[39 + i] = 400;
            }
            else if (Angle_RP_XY < 720)
            {
                for (int i = 0; i < count; i++)
                {
                    if (angle[i] > Angle_RP_XY)
                    {
                        ignore_angle_start_index = i;
                        break;
                    }
                }
                for (int i = 0; i < count; i++)
                {
                    if (angle[i] > Angle_RP_XY + 90)
                    {
                        ignore_angle_end_index = i;
                        break;
                    }
                }
                for (int i = ignore_angle_start_index; i < ignore_angle_end_index; i++)data[39 + i] = 400;
            }
            Angle_RP_XY = 0;

            for (int i = 0; i < data[38]; i++)if (data[i + 39] < 18)data[i + 39] = 400;

            min = *std::min_element(&data[39], &data[39+ count]);

            for (int i = 0; i < count; i++)
            {
                if (data[i + 39] == min)
                {
                    filter_index = i;
                    break;
                }
            }

            if (filter_index >= 42)
            {
                filter_matrix[0] = data[filter_index + 36];
                filter_matrix[1] = data[filter_index + 37];
                filter_matrix[2] = data[filter_index + 38];

            }
            else if (filter_index == 41)
            {
                filter_matrix[0] = data[39 + count];
                filter_matrix[1] = data[39];
                filter_matrix[2] = data[40];
            }
            else if (filter_index == 40)
            {
                filter_matrix[0] = data[39 + count - 1];
                filter_matrix[1] = data[39 + count];
                filter_matrix[2] = data[39];
            }
            else if (filter_index == 39)
            {
                filter_matrix[0] = data[39 + count - 2];
                filter_matrix[1] = data[39 + count - 1];
                filter_matrix[2] = data[39 + count];
            }
            filter_matrix[3] = data[filter_index + 40];
            filter_matrix[4] = data[filter_index + 41];
            filter_matrix[5] = data[filter_index + 42];

            dif = data[filter_index + 39] - ((filter_matrix[0] + filter_matrix[1] + filter_matrix[2] + filter_matrix[3] + filter_matrix[4] + filter_matrix[5]) / 6);
            if ((dif < -5) || (dif > 5))data[filter_index + 39] = 400;

            D_RP=*std::min_element(&data[39], &data[39 + count]);

            if (D_RP < S_IN_RP[3])OUT[1] = UR_OUT_RP[3];
            else if (D_RP < S_IN_RP[2])OUT[1] = UR_OUT_RP[2];
            else if (D_RP < S_IN_RP[1])OUT[1] = UR_OUT_RP[1];
            else if (D_RP < S_IN_RP[0])OUT[1] = UR_OUT_RP[0];
            else OUT[1] = 1.0f;
        }
        /**********************RPLidar*********************/
        if (ctrl_c_pressed == true)break;
    }
    drv->stop();
    drv->stopMotor();
    RPlidarDriver::DisposeDriver(drv);
    drv = NULL;
    stop = 1;
    return NULL;
}



#define sound_Pin 27
#define EZ0_select_Pin 28
#define Relais1 24
#define Relais2 25
uint16_t Abstand_TOF_1, Abstand_TOF_2;
void* Sensoren(void* args)                                       //蜂鸣器间隔响的独立计时线程
{
    uint8_t ret, dataReady, rangeStatus1, rangeStatus2;
    uint16_t dev = 0x29;  // Address on i2c bus
    uint32_t ret1, ret2;
    int i, i2cfd, MIC_old, MIC_different;
    float volts[4];
    int16_t itemp;

    mars::ipc::ShareMemory shm; //(4096);
    rpi_tca9548a tca;
    i2cfd = tca.init(0x70);
    tca.set_channel(0);                                          //TCA9548A bus#0
    int handle = wiringPiI2CSetup(0x48);                         //ADS1115的地址为0x48
    

    int Initial_Difference_TOF = 0;
    int distance[4] = { 0 };

///************************TOF1***********************/
    tca.set_channel(6);                                          //TCA9548A bus#7
    ret = 0;
    if (VL53L1X_BootState(dev, &ret) != 0)print_error("Boot state");
    if (ret == 0)printf("Not booted\n");

    if (VL53L1X_SensorInit(dev) != 0)print_error("Sensor init");

    if (VL53L1X_SetInterMeasurementInMs(dev, 100) != 0)print_error("Set inter measurement");

    if (VL53L1X_SetDistanceMode(dev, 1) != 0)print_error("Set distance mode");

    if (VL53L1X_StartRanging(dev) != 0)print_error("Start ranging");
    /************************TOF1***********************/

    /************************TOF2***********************/
    tca.set_channel(3);                                          //TCA9548A bus#3
    ret = 0;
    if (VL53L1X_BootState(dev, &ret) != 0)print_error("Boot state");
    if (ret == 0) printf("Not booted\n");

    if (VL53L1X_SensorInit(dev) != 0) print_error("Sensor init");

    if (VL53L1X_SetInterMeasurementInMs(dev, 100) != 0)print_error("Set inter measurement");

    if (VL53L1X_SetDistanceMode(dev, 1) != 0)print_error("Set distance mode");

    if (VL53L1X_StartRanging(dev) != 0)print_error("Start ranging");
    /************************TOF2***********************/

    /************************MIC***********************/
    tca.set_channel(0);
    delay(100);
    MIC_old = (int)(readVoltage(handle, 3) * 3000);
    delay(100);
    /************************MIC***********************/

    printf("%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s\n", "VCC", "MIC", "EZ0_1", "EZ0_2", "TOF1", "TOF2", "RPLidar", "Winkel", "check", "THERMO", "Angle_XY", "SEND", "OUT[0]");
    while(1)
    {
        /************************EZ0***********************/
        tca.set_channel(0);                     //TCA9548A bus#0
        digitalWrite(EZ0_select_Pin, HIGH);
        delay(50);
        digitalWrite(EZ0_select_Pin, LOW);
        for (i = 0; i < 4; i++) volts[i] = readVoltage(handle, i);
        data[0] = (int)(volts[0] * 100);
        data[1] = (int)(volts[3] * 3000);
        data[2] = (int)((volts[1] / 0.0097) * 2.54);
        data[3] = (int)((volts[2] / 0.0097) * 2.54);
        
        MIC_different = MIC_old - data[1];
        MIC_old = data[1];
        data[1] = MIC_different;
        /************************EZ0***********************/

        /************************TOF***********************/
        tca.set_channel(6);                                          //TCA9548A bus#6
        if (VL53L1X_GetDistance(dev, &Abstand_TOF_1) != 0)print_error("Get distance");
        data[4] = (int)(Abstand_TOF_1 / 10.0f);

        tca.set_channel(3);                                          //TCA9548A bus#3
        if (VL53L1X_GetDistance(dev, &Abstand_TOF_2) != 0)print_error("Get distance");
        data[5] = (int)(Abstand_TOF_2 / 10.0f);
        /************************TOF***********************/

        /************************44L_1**********************/
        tca.set_channel(2);                                          //TCA9548A bus#2
        memset(rbuf1, 0, N_READ);
        ret1 = i2c_read_reg8(D6T_ADDR, D6T_CMD, rbuf1, N_READ);
        if (!D6T_checkPEC(rbuf1, N_READ - 1))
        {
            //Convert to temperature data (degC)
            ptat1 = (double)conv8us_s16_le(rbuf1, 0) / 10.0;
            for (i = 0; i < N_PIXEL; i++) {
                itemp = conv8us_s16_le(rbuf1, 2 + 2 * i);
                data[i + 6] = (int)(((double)itemp / 10.0) * 10);
            }
        }
        /************************44L_1**********************/

        /************************44L_2**********************/
        tca.set_channel(7);                                          //TCA9548A bus#7
        memset(rbuf2, 0, N_READ);
        ret2 = i2c_read_reg8(D6T_ADDR, D6T_CMD, rbuf2, N_READ);
        if (!D6T_checkPEC(rbuf2, N_READ - 1))
        {
            //Convert to temperature data (degC)
            ptat2 = (double)conv8us_s16_le(rbuf2, 0) / 10.0;
            for (i = 0; i < N_PIXEL; i++) {
                itemp = conv8us_s16_le(rbuf2, 2 + 2 * i);
                data[i + 22] = (int)(((double)itemp / 10.0) * 10);
            }
        }
        ///************************44L_2**********************/

        printf("%8.2f %8d %8d %8d %8i %8i %8d %8.1f %8d %8d %8.1f %8.1f %8.1f\r", (float)data[0] / 100, data[1], data[2], data[3], Abstand_TOF_1, Abstand_TOF_2, data[39], Winkel, check_flag, MAX_THERMOL, Angle_RP_XY, SEND_BUFF_OUT, OUT[0]); //ADS1115第1个通道测量电源电压 -> volts[0]   
        fflush(stdout);

        /*****************************************************************************************Raspberry Pi*******************************************************************************************************
        |          Sensoren und UR5 Robote              |                                                                 Webseite                                                                                  |
        |****************C++ Programm*******************|****************************PHP Programm******************************************************Javascript Programm******************************************|
        | VCC -> data[0]                                |=> $D['shm_data']['VCC']                                             |=> window.dashboard.shm_data.VCC                                                     |
        | MIC -> data[1]                                |   $D['shm_data']['MIC']                                             |   window.dashboard.shm_data.MIC                                                     |
        | EZ0_1 -> data[2]                              |   $D['shm_data']['EZ0_1']                                           |   window.dashboard.shm_data.EZ0_1                                                   |
        | EZ0_2 -> data[3]                              |   $D['shm_data']['EZ0_2']                                           |   window.dashboard.shm_data.EZ0_2                                                   |
        |-----------------------------------------------|---------------------------------------------------------------------|-------------------------------------------------------------------------------------|
        | TOF1 -> data[4]                               |=> $D['shm_data']['TOF1']                                            |=> window.dashboard.shm_data.TOF1                                                    |
        | TOF2 -> data[5]                               |=> $D['shm_data']['TOF2']                                            |=> window.dashboard.shm_data.TOF2                                                    |
        |-----------------------------------------------|---------------------------------------------------------------------|-------------------------------------------------------------------------------------|
        | THERMOL1 -> data[6] bis data[21]              |=> $D['shm_data']['THERMOL1'][0] bis                                 |=> window.dashboard.shm_data.THERMOL1[0] bis                                         |
        |                                               |   $D['shm_data']['THERMOL1'][15]                                    |   window.dashboard.shm_data.THERMOL1[15]                                            |
        | THERMOL2 -> data[22] bis data[37]             |=> $D['shm_data']['THERMOL2'][0] bis                                 |=> window.dashboard.shm_data.THERMOL2[0] bis                                         |
        |                                               |   $D['shm_data']['THERMOL2'][15]                                    |   window.dashboard.shm_data.THERMOL2[15]                                            |
        |-----------------------------------------------|---------------------------------------------------------------------|-------------------------------------------------------------------------------------|
        | RPLidar_Datenzahl -> data[38]                 |=> $D['shm_data']['RPLidar_Zahl']                                    |=> window.dashboard.shm_data.RPLidar_Zahl                                            |
        | RPLidar_Abstanddaten -> data[39] bis          |=> $D['shm_data']['RPLidar_Abstand'][0] bis                          |=> window.dashboard.shm_data.RPLidar_Abstand[0] bis                                  |
        |                         data[40+RPLIDAR_Zahl] |   $D['shm_data']['RPLidar_Abstand'][$D['shm_data']['RPLidar_Zahl']] |   window.dashboard.shm_data.RPLidar_Abstand[window.dashboard.shm_data.RPLidar_Zahl] |
        ************************************************************************************************************************************************************************************************************/
        if (Abstand_TOF_1 < Abstand_TOF_2)D_EN = Abstand_TOF_1;
        else D_EN = Abstand_TOF_2;
        //D_EN = *std::min_element(&Abstand_TOF_1, &Abstand_TOF_2);                               //取4个测距传感器的最小值
        MAX_THERMOL = *std::max_element(&data[6], &data[37]);                       //取2个THERMOL传感器的最大值

        if (MAX_THERMOL > 300)                                                      //若2个THERMOL传感器的最大值大于30℃，则进行距离判断，向UR5发送相应的调速信息
        {
            if (D_EN < S_IN_EN[3]) OUT[0] = UR_OUT_EN[3];
            else if (D_EN < S_IN_EN[2]) OUT[0] = UR_OUT_EN[2];
            else if (D_EN < S_IN_EN[1]) OUT[0] = UR_OUT_EN[1];
            else if (D_EN < S_IN_EN[0]) OUT[0] = UR_OUT_EN[0];
            else OUT[0] = 1.0f;
        }
        else OUT[0] = 1.0f;
        //若正常，一直发送UR5调速命令:100% 
        if (OUT[0] < OUT[1])SEND_BUFF_OUT = OUT[0];
        else SEND_BUFF_OUT = OUT[1];
        sprintf(send_buf, "%f", SEND_BUFF_OUT);

        for (int m = 0; m < 39 + data[38]; m++)snprintf(shm.mem + 4 * m, 4, "%d", data[m]); //向共享内存段存入数据
        if (stop == 1)break;                                                                //若RPLidar进程已完全关闭，则此进程即将关闭
    }
    printf("\nRPLidar thread has been shutdown\n");
    printf("Sensoren thread has been shutdown\n");
    stop_main = 1;                                                                          //向主函数发送关闭信号
    return NULL;                                                                            //程序运行完毕，返回值为0表示程序至关闭时无异常
}

signed int check_signal_TOF=0, check_signal_EZ0=0;
int  timer_zahl = 0, beep_zahl = 5, Blinkzahlen = 0;
bool sound_switch = 0,timer_flag = 0;
unsigned int temp = 0;         //计数用的变量
bool sound_status = 0;         //led的状态
void sigroutine(int signo) {
    switch (signo) {
    case SIGVTALRM:
        send(new_fd, send_buf, sizeof(send_buf), 0);
        memset(send_buf, NULL, sizeof(send_buf));                 //从内存清空send_buf的元素的值。
        
        check_signal_TOF = Abstand_TOF_1 - Abstand_TOF_2;
        check_signal_EZ0 = data[1] - data[2];
        if ((check_signal_TOF < -30 || check_signal_TOF > 30) || (check_signal_EZ0 < -3 || check_signal_EZ0>3))
        {
            if (check_flag >= 150) {
                check_flag = 0;
                digitalWrite(Relais2, LOW);
            }
            else check_flag = check_flag + 1;
        }
        else
        {
            check_flag = 0;
            digitalWrite(Relais2, HIGH);
        }
        break;
    case SIGINT:
        printf("\nCatch a signal -- SIGINT\n");
        ctrl_c_pressed = true;
        break;
    case SIGSEGV:
        printf("\nCatch a signal -- SIGSEGV\n");
        digitalWrite(Relais1, HIGH);
        ctrl_c_pressed = true;
        break;
    }
}

int *timer1_handle;
int main(int argc, const char* argv[]) 
{
    //signal(SIGSEGV, sigroutine);
    
    if (wiringPiSetup() == -1)ctrl_c_pressed = true; //wiringPi库
    pinMode(sound_Pin, OUTPUT);
    pinMode(Relais1, OUTPUT);
    pinMode(Relais2, OUTPUT);
    pinMode(EZ0_select_Pin, OUTPUT);
    digitalWrite(Relais1, HIGH);
    digitalWrite(Relais2, HIGH);
    printf("Web-Server restart in ...3s\r");
    fflush(stdout);
    delay(1000);
    printf("Web-Server restart in ...2s\r");
    fflush(stdout);
    delay(1000);
    printf("Web-Server restart in ...1s\r");
    fflush(stdout);
    delay(1000);
    printf("Web-Server restart in ...restarting....\r");
    fflush(stdout);
    system("sudo service nginx restart");
    delay(3000);
    system("sudo service php7.3-fpm restart");
    delay(1000);
    printf("Web-Server restart in ...restarting....accomplished\n");
    fflush(stdout);

    pthread_t stPid = 0;                                         //线程的id
    pthread_create(&stPid, NULL, tcp_connect_recv, NULL);        //创建TCP Socket的接收线程(此线程内部包含TCP Socket的发送子线程)
    pthread_detach(stPid);                                       //使TCP Socket的接收线程与主线程并发运行

    stPid = 1;                                                   //线程的id
    pthread_create(&stPid, NULL, RPLidar, NULL);                 //创建RPLidar线程
    pthread_detach(stPid);                                       //使RPLidar线程与主线程并发运行

    stPid = 2;                                                   //线程的id
    pthread_create(&stPid, NULL, Sensoren, NULL);                //创建传感器的线程
    pthread_detach(stPid);                                       //使传感器线程与主线程并发运行

    signal(SIGINT, sigroutine);
 
#ifdef SELF_CHECKING_ON
    timer1_handle = &timer_item;
    if (ST_CreateAppTimer("timer1", sigroutine, 100, 100, 4, timer1_handle) != 0)ctrl_c_pressed = true;
    else printf("Timer1 created\n");
#endif

    while(1)                                                     //主函数的循环函数
    {
        //if ((data[1] > 500 || data[1] < -500) && (sound_switch == 0) && (Blinkzahlen == 0) && (timer_flag == 0))sound_switch = 1;
        //digitalWrite(sound_Pin, sound_status);
        if (stop_main == 1)break;
    }

    while (!stop_main);                        //等待所有进程和线程结束
    printf("main() received the stop signal\n");

#ifdef SELF_CHECKING_ON
    while(ST_DeleteAppTimer(timer1_handle));   //停止并删除定时器1，直至成功
#endif
    printf("Timer1_100ms deleted successfully\n");
    endLEDs();                                 //退出程序时的LED设置
    printf("LEDs closed successfully\n");
    /*关掉线程的相关操作*/
    shutdown(0, SHUT_RDWR);
    shutdown(1, SHUT_RDWR);
    shutdown(2, SHUT_RDWR);
    shutdown(3, SHUT_RDWR);
    close(new_fd);
    close(0);
    close(1);
    close(2);
    close(3);
    /*关掉线程的相关操作*/

    printf("all Programs closed successfully\n");
    return EXIT_SUCCESS;                //程序运行完毕，返回值为0表示程序至关闭时无异常
}
