/**
 * @file	share-memory-set.cc
 * @brief   进程间通信-共享内存set测试
 *              共享内存创建之后，一直存在于内核中，读完之后，内容还存在，直到被删除或系统关闭
 * @author	yanjingang
 * @note    编译：g++ mars/ipc/share-memory-set.cc -std=c++11 -Wall -o build/ipc-share-memory-set
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
         cout << "input content: ";
        fgets(shm.mem, shm.size(), stdin);
        printf("input content: ");
        fflush(stdout);
        ssize_t n = read(0, shm.mem, 4096 - 1);  //从标准输入获取消息
        if (n > 0) {
            shm.mem[n - 1] = '\0';  //过滤掉从标准输入中获取的换行
        }
        cout << "get share memory content: " << shm.mem << endl;
    }

    return 0;
}