/**
 * @file	share-memory-get.cc
 * @brief   ���̼�ͨ��-�����ڴ�get����
 *              �����ڴ洴��֮��һֱ�������ں��У�����֮�����ݻ����ڣ�ֱ����ɾ����ϵͳ�ر�
 * @author	yanjingang
 * @note    ���룺g++ mars/ipc/share-memory-get.cc -std=c++11 -Wall -o build/ipc-share-memory-get
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