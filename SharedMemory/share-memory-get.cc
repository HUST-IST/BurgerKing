/**
 * @file	share-memory-get.cc
 * @brief   进程间通信-共享内存get测试
 *              共享内存创建之后，一直存在于内核中，读完之后，内容还存在，直到被删除或系统关闭
 * @author	yanjingang
 * @note    编译：g++ mars/ipc/share-memory-get.cc -std=c++11 -Wall -o build/ipc-share-memory-get
 */
#include "share-memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <iostream>
using namespace std;

int main() {
    mars::ipc::ShareMemory shm; //(4096);

    while (true) {
        sleep(1);
        cout << "get share memory content: " << strlen(shm.mem) << " - " << shm.mem << endl;
    }

    return 0;
}