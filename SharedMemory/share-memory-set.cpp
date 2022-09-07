/**
 * @file	share-memory-set.cc
 * @brief   进程间通信-共享内存set测试
 *              共享内存创建之后，一直存在于内核中，读完之后，内容还存在，直到被删除或系统关闭
 * @author	yanjingang
 * @note    编译：g++ mars/ipc/share-memory-set.cc -std=c++11 -Wall -o build/ipc-share-memory-set
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>





//
//float data[400] = { -1 };
//void* SharedMemory(void* args)
//{
//    mars::ipc::ShareMemory shm; //(4096);
//    
//    for (;;) {
//        //shm.mem = (char*)data_test;
//        sprintf(shm.mem, "%f", data);
//        fflush(stdout);
//    }
//    return NULL;
//}