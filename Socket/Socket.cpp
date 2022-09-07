#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string>
#include <math.h>
#include <pthread.h>
#include <signal.h>

extern "C"{                                                                                 //本程序是C++编译的，如需要调用C语言编译的子程序则从此处添加C的头文件
#include "ws2812b_rpi.h"
#include "LEDws2812b.h"
}

using namespace std;

#define PORT 30003                                                                          //端口号
#define BACKLOG 5                                                                           //最大监听数

int new_fd = 0;                                                                             //建立连接后的句柄
char send_buf[3] = { 1,NULL, NULL};              //"发送"寄存器
double data_XYZ[3]= { 0 };                                                                  //储存xyz轴向速度的数组
double Angle_RP_XY=0;
double Winkel;                                                                              //由速度arctan出来的移动方向角
double Winkelz;
unsigned char LED_status = 0;                                                               //led的状态
bool Warner = 0;

typedef struct st_tcp_args_t {
    int new_fd;
} tcp_args_t;

void* tcp_connect_send(void* args)                                                          //TCP Socket发送数据的独立线程
{
    tcp_args_t* tcp_arg = (tcp_args_t*)args;
    while (1) {
        scanf("%s", send_buf);                                                              //发送内容，参数分别是连接句柄，内容，大小，其他信息（设为0即可）
        write(tcp_arg->new_fd, send_buf, sizeof(send_buf));                                 
    }
    return NULL;
}

void* tcp_connect_recv(void* args)                                                          //TCP Socket接收数据的独立线程
{
    LED_Init();                                                  //LED初始化
    int iSocketFD = 0; // socket句柄
    iSocketFD = socket(AF_INET, SOCK_STREAM, 0);                                            //建立socket，SOCK_STREAM流式套接字，表示tcp
    if (iSocketFD < 0) 
    {
        printf("创建socket失败：%d\n", errno);
        return 0;
    }

    struct sockaddr_in stLocalAddr = { 0 };                                                 //本地地址信息结构图，下面有具体的属性赋值
    stLocalAddr.sin_family = AF_INET;                                                       //该属性表示接收本机或其他机器传输
    stLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);                                        //IP，INADDR_ANY表示本机IP
    stLocalAddr.sin_port = htons(PORT);                                                     //端口号

    int flag = 1;
    if (setsockopt(iSocketFD, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0) 
    {
        printf("设置socket选项失败：%d\n", errno);
        close(iSocketFD);
    }

    
    if (bind(iSocketFD, (struct sockaddr*)&stLocalAddr, sizeof(stLocalAddr)) < 0)           //绑定地址结构体和socket
    {
        printf("绑定socket失败：%d\n", errno);
        close(iSocketFD);
        return 0;
    }


    if (listen(iSocketFD, BACKLOG) < 0)                                                     //开启监听，第二个参数是最大监听数
    {
        printf("监听socket失败：%d\n", errno);
        shutdown(iSocketFD, SHUT_RDWR);
        close(iSocketFD);
        return 0;
    }

    printf("开启监听, 监听的tcp socket句柄为：%d\n", iSocketFD);


    struct sockaddr_in stRemoteAddr = { 0 };                                                //保存客户端地址信息
    socklen_t socklen = sizeof(stRemoteAddr);                                               //千万不能为0，否则获取不到stRemoteAddr

    new_fd = accept(iSocketFD, (struct sockaddr*)&stRemoteAddr, &socklen);                  //开启accept后会阻塞直到接收到消息，参数分别是socket句柄，接收到的地址信息以及大小

    if (new_fd < 0) 
    {
        if (errno == 22)
        {
            shutdown(iSocketFD, SHUT_RDWR);
            close(iSocketFD);
            printf("\nSocket thread has been shut down\n");
            return EXIT_SUCCESS;
        }
        
    }
    else 
    {
        printf("接收成功！连接句柄为：%d\n", new_fd);
        printf("确认客户端ip addr is %s\n", inet_ntoa(stRemoteAddr.sin_addr));
        printf("确认客户端port：%d \n", ntohs(stRemoteAddr.sin_port));
        //strncpy(send_buf, "-2", sizeof("-2"));
        //send(new_fd, send_buf, sizeof(send_buf), 0);
    }

    /*创建发送的独立子线程*/
    pthread_t child_tid = 3;
    tcp_args_t tcp_arg;
    tcp_arg.new_fd = new_fd;
    pthread_create(&child_tid, NULL, tcp_connect_send, (void*)&tcp_arg);
    pthread_detach(child_tid);
    /*创建发送的独立子线程*/

    int iRecvLen = 0;                                                                           // 接收成功后的返回值
    char recv_buf[256] = { 0 };                                                                 //"接收"寄存器
    while (1) 
    {

        iRecvLen = recv(new_fd, recv_buf, sizeof(recv_buf), 0);
        //iRecvLen = read(new_fd, recv_buf, sizeof(recv_buf));
        string recv = recv_buf;
        if (iRecvLen <= 0) {                                                                //对端关闭连接 返回0
            printf("接收失败或者对端关闭连接：%d\n", errno);
            shutdown(iSocketFD, SHUT_RDWR);
            close(new_fd);
            close(iSocketFD);
            while (1) {
                iSocketFD = socket(AF_INET, SOCK_STREAM, 0);                                //建立socket，SOCK_STREAM流式套接字，表示tcp
                if (iSocketFD < 0)
                {
                    printf("创建socket失败：%d\n", errno);
                    return 0;
                }

                struct sockaddr_in stLocalAddr = { 0 };                                     //本地地址信息结构图，下面有具体的属性赋值
                stLocalAddr.sin_family = AF_INET;                                           //该属性表示接收本机或其他机器传输
                stLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);                            //IP，INADDR_ANY表示本机IP
                stLocalAddr.sin_port = htons(PORT);                                         //端口号

                int flag = 1;
                if (setsockopt(iSocketFD, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0)
                {
                    printf("设置socket选项失败：%d\n", errno);
                    close(iSocketFD);
                }

                if (bind(iSocketFD,(struct sockaddr*)&stLocalAddr, sizeof(stLocalAddr)) < 0)//绑定地址结构体和socket
                {
                    printf("绑定socket失败：%d\n", errno);
                    close(iSocketFD);
                    return 0;
                }

                if (listen(iSocketFD, BACKLOG) < 0)                                         //开启监听，第二个参数是最大监听数
                {
                    printf("监听socket失败：%d\n", errno);
                    shutdown(iSocketFD, SHUT_RDWR);
                    close(iSocketFD);
                    return 0;
                }
                printf("开启监听, 监听的tcp socket句柄为：%d\n", iSocketFD);

                struct sockaddr_in stRemoteAddr = { 0 };                                    //保存客户端地址信息
                socklen_t socklen = sizeof(stRemoteAddr);                                   //千万不能为0，否则获取不到stRemoteAddr
                new_fd = accept(iSocketFD, (struct sockaddr*)&stRemoteAddr, &socklen);      //开启accept后会阻塞直到接收到消息，参数分别是socket句柄，接收到的地址信息以及大小

                if (new_fd < 0)
                {
                    if(errno==22)printf("\nSocket thread has shut down\n");
                    shutdown(iSocketFD, SHUT_RDWR);
                    close(iSocketFD);
                    return 0;
                }
                else
                {
                    printf("接收成功！连接句柄为：%d\n", new_fd);
                    printf("确认客户端ip addr is %s\n", inet_ntoa(stRemoteAddr.sin_addr));
                    printf("确认客户端port：%d \n", ntohs(stRemoteAddr.sin_port));
                    //strncpy(send_buf, "-2", sizeof("-2"));
                    //send(new_fd, send_buf, sizeof(send_buf), 0);
                    break;
                }
            }
            return NULL;
        }
        else
        {
            //size_t Komma1 = recv.find(",");                                                 //检索第1个逗号 得到第1个逗号在字符串中的序号
            //size_t Komma2 = recv.find(",", Komma1 + 1);                                     //检索第2个逗号 得到第2个逗号在字符串中的序号
            //data_XYZ[0] = atof(recv.substr(1, Komma1).c_str());                             //将x轴速度作为子字符串从接收的字符串中提取出来 
            //data_XYZ[1] = atof(recv.substr(Komma1 + 1, Komma2).c_str());                    //将y轴速度作为子字符串从接收的字符串中提取出来 
            //data_XYZ[2] = atof(recv.substr(Komma2+1, recv.length()).c_str());               //将z轴速度作为子字符串从接收的字符串中提取出来 
            //Winkel = atan2(data_XYZ[1], data_XYZ[0]) * 57.296;                              //由arctan（机器人y轴与x轴的速度比值）得到机器人运动方向
            //Winkelz = atan2(data_XYZ[2], sqrt(data_XYZ[0] * data_XYZ[0] + data_XYZ[1] * data_XYZ[1])) * 57.296;

            size_t Komma1 = recv.find(",");                                                 //检索第1个逗号 得到第1个逗号在字符串中的序号
            size_t Komma2 = recv.find(",", Komma1 + 1);                                     //检索第2个逗号 得到第2个逗号在字符串中的序号
            size_t Komma3 = recv.find(",", Komma2 + 1);                                     //检索第3个逗号 得到第3个逗号在字符串中的序号
            size_t Komma4 = recv.find(",", Komma3 + 1);                                     //检索第4个逗号 得到第4个逗号在字符串中的序号
            size_t Komma5 = recv.find(",", Komma4 + 1);                                     //检索第5个逗号 得到第5个逗号在字符串中的序号

            data_XYZ[0] = atof(recv.substr(1, Komma1).c_str());                             //将x轴速度作为子字符串从接收的字符串中提取出来
            data_XYZ[1] = atof(recv.substr(Komma1 + 1, Komma2).c_str());                    //将y轴速度作为子字符串从接收的字符串中提取出来
            data_XYZ[2] = atof(recv.substr(Komma2 + 1, Komma3).c_str());                    //将z轴速度作为子字符串从接收的字符串中提取出来
            Winkel = atof(recv.substr(Komma3 + 1, Komma4).c_str());                         //将Winkel作为子字符串从接收的字符串中提取出来
            Winkelz = atof(recv.substr(Komma4 + 1, Komma5).c_str());                        //将Winkelz作为子字符串从接收的字符串中提取出来
            Angle_RP_XY = atof(recv.substr(Komma5 + 1, recv.length()).c_str());             //将Angle_RP_XY作为子字符串从接收的字符串中提取出来

            if (Warner == 0)LEDchoosing();//选择需要闪烁的LED并使能
        }
    }
    shutdown(iSocketFD, SHUT_RDWR);
    close(new_fd);
    close(iSocketFD);
    return NULL;
}