/**
 * @file	share-memory.h
 * @brief   进程间通信-共享内存头文件封装
 *              共享内存创建之后，一直存在于内核中，读完之后，内容还存在，直到被删除或系统关闭
 * @author	yanjingang
 */
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//extern float data[320];

#define MEMSIZE 2048
int fd;
namespace mars {
    namespace ipc {
        // 共享内存类
        class ShareMemory {
        private:
            // key
            int key_;
            // 共享内存ID
            int shmid_;
            // 共享内存大小
            int size_;
            // 创建共享内存
            int create_share_memory();
        public:
            ShareMemory();
            virtual ~ShareMemory();
            // 变量返回
            int key() { return key_; }
            int shmid() { return shmid_; }
            int size() { return size_; }
            // 共享内存指针
            char* mem;
        };


        // 构造函数
        ShareMemory::ShareMemory() {
            this->size_ = MEMSIZE;
            int ret = this->create_share_memory();
            if (ret != 0) {
                printf("create_share_memory fail!\n");
            }
        }

        // 创建共享内存
        int ShareMemory::create_share_memory() {
            // 创建key, 以实现非亲缘关系进程间通信
            // 创建key, 以实现非亲缘关系进程间通信
            key_t key_ = ftok("/home/pi/projects/", '2');

            ////不存在则新建文件
            //fd = open("/home/pi/projects/SharedMemory/shmid", O_CREAT | O_EXCL);
            //if (-1 == fd)  //true则说明文件打开出错
            //{
            //    perror("File exists\n");
            //    int fd = open("/home/pi/projects/SharedMemory/shmid", O_RDONLY);
            //    fchmod(fd,S_IRWXU | S_IRWXG | S_IRWXO);
            //    read(fd, &key_, sizeof(key_t));
            //}
            //else
            //{
            //    int fd = open("/home/pi/projects/SharedMemory/shmid", O_RDWR | O_CREAT);
            //    if (-1 == fd)  //true则说明文件打开出错
            //    {
            //        perror("File open\n");
            //        exit(-1);
            //    }
            //    printf("File open success\n");
            //    fchmod(fd, S_IRWXU | S_IRWXG | S_IRWXO);
            //    // 写入文件
            //    ssize_t res = write(fd, &key_, sizeof(key_t));
            //    if (res <= 0)
            //    {
            //        perror("File write\n");
            //        exit(-1);
            //    }
            //    else printf("File write success\n");
            //    close(fd);
            //    printf("File closed\n");
            //}


            // 创建共享内存
            shmid_ = shmget(key_, this->size(), IPC_CREAT | 0777);    // 以ftok创建的key，需要IPC_CREAT参数 
            //shmid_ = shmget(IPC_PRIVATE, 128, 0777);   // 在内核中生成共享内存的对象；相当于缓存，只能实现有亲缘关系进程间通信
            if (shmid_ == -1) {
                printf("shmget create share memory fail!\n");
                return -3;
            }
            printf("shmget create share memory success! shmid:%d, size:%d \n", shmid(), size());

            // 返回这块内存的虚拟地址（将申请的共享内存挂接在该进程的页表上，是将虚拟内存和物理内存相对应）
            mem = (char*)shmat(shmid_, NULL, 0);
            if (mem == nullptr) {
                printf("shmat share memory mapping fail!\n");
                return -4;
            }

            return 0;
        }


        // 析构函数
        ShareMemory::~ShareMemory()
        {
            // 将用户空间的内存释放
            shmdt(mem);
            // 将内核空间的内存释放
            shmctl(shmid_, IPC_RMID, NULL);
        }

    }
}
void* SharedMemory(void* args);
