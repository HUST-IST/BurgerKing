/**
 * @file	share-memory.h
 * @brief   ���̼�ͨ��-�����ڴ�ͷ�ļ���װ
 *              �����ڴ洴��֮��һֱ�������ں��У�����֮�����ݻ����ڣ�ֱ����ɾ����ϵͳ�ر�
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
        // �����ڴ���
        class ShareMemory {
        private:
            // key
            int key_;
            // �����ڴ�ID
            int shmid_;
            // �����ڴ��С
            int size_;
            // ���������ڴ�
            int create_share_memory();
        public:
            ShareMemory();
            virtual ~ShareMemory();
            // ��������
            int key() { return key_; }
            int shmid() { return shmid_; }
            int size() { return size_; }
            // �����ڴ�ָ��
            char* mem;
        };


        // ���캯��
        ShareMemory::ShareMemory() {
            this->size_ = MEMSIZE;
            int ret = this->create_share_memory();
            if (ret != 0) {
                printf("create_share_memory fail!\n");
            }
        }

        // ���������ڴ�
        int ShareMemory::create_share_memory() {
            // ����key, ��ʵ�ַ���Ե��ϵ���̼�ͨ��
            // ����key, ��ʵ�ַ���Ե��ϵ���̼�ͨ��
            key_t key_ = ftok("/home/pi/projects/", '2');

            ////���������½��ļ�
            //fd = open("/home/pi/projects/SharedMemory/shmid", O_CREAT | O_EXCL);
            //if (-1 == fd)  //true��˵���ļ��򿪳���
            //{
            //    perror("File exists\n");
            //    int fd = open("/home/pi/projects/SharedMemory/shmid", O_RDONLY);
            //    fchmod(fd,S_IRWXU | S_IRWXG | S_IRWXO);
            //    read(fd, &key_, sizeof(key_t));
            //}
            //else
            //{
            //    int fd = open("/home/pi/projects/SharedMemory/shmid", O_RDWR | O_CREAT);
            //    if (-1 == fd)  //true��˵���ļ��򿪳���
            //    {
            //        perror("File open\n");
            //        exit(-1);
            //    }
            //    printf("File open success\n");
            //    fchmod(fd, S_IRWXU | S_IRWXG | S_IRWXO);
            //    // д���ļ�
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


            // ���������ڴ�
            shmid_ = shmget(key_, this->size(), IPC_CREAT | 0777);    // ��ftok������key����ҪIPC_CREAT���� 
            //shmid_ = shmget(IPC_PRIVATE, 128, 0777);   // ���ں������ɹ����ڴ�Ķ����൱�ڻ��棬ֻ��ʵ������Ե��ϵ���̼�ͨ��
            if (shmid_ == -1) {
                printf("shmget create share memory fail!\n");
                return -3;
            }
            printf("shmget create share memory success! shmid:%d, size:%d \n", shmid(), size());

            // ��������ڴ�������ַ��������Ĺ����ڴ�ҽ��ڸý��̵�ҳ���ϣ��ǽ������ڴ�������ڴ����Ӧ��
            mem = (char*)shmat(shmid_, NULL, 0);
            if (mem == nullptr) {
                printf("shmat share memory mapping fail!\n");
                return -4;
            }

            return 0;
        }


        // ��������
        ShareMemory::~ShareMemory()
        {
            // ���û��ռ���ڴ��ͷ�
            shmdt(mem);
            // ���ں˿ռ���ڴ��ͷ�
            shmctl(shmid_, IPC_RMID, NULL);
        }

    }
}
void* SharedMemory(void* args);
