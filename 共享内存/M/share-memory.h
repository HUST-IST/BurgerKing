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
#include <unistd.h>
#include <iostream>

#define PATHNAME "."
#define PROCID 'b' //0x6666
#define MEMSIZE 4096*1

using namespace std;

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
            ShareMemory(const size_t m_size);
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
                cout << "create_share_memory fail!" << endl;
            }
        }
        // ���캯��
        ShareMemory::ShareMemory(const size_t m_size) {
            this->size_ = m_size;
            int ret = this->create_share_memory();
            if (ret != 0) {
                cout << "create_share_memory fail!" << endl;
            }
        }

        // ���������ڴ�
        int ShareMemory::create_share_memory() {
            // ����key, ��ʵ�ַ���Ե��ϵ���̼�ͨ��
            key_ = ftok(PATHNAME, PROCID);
            if (key_ == -1) {
                cout << "create key file..." << endl;
                FILE* fp;
                if ((fp = fopen(PATHNAME, "a")) == nullptr) {
                    cout << "keyfile created failed" << endl;
                    return -2;
                }
                fclose(fp);
                key_ = ftok(PATHNAME, PROCID);
                if (key_ == -1) {
                    cout << "key created failed" << endl;
                    return -1;
                }
            }
            cout << "mq key:" << key_ << endl;

            // ���������ڴ�
            shmid_ = shmget(key_, this->size(), IPC_CREAT | 0777);    // ��ftok������key����ҪIPC_CREAT���� 
            //shmid_ = shmget(IPC_PRIVATE, 128, 0777);   // ���ں������ɹ����ڴ�Ķ����൱�ڻ��棬ֻ��ʵ������Ե��ϵ���̼�ͨ��
            if (shmid_ == -1) {
                cout << "shmget create share memory fail!" << endl;
                return -3;
            }
            cout << "shmget create share memory success! shmid:" << shmid() << " size:" << size() << endl;

            // ��������ڴ�������ַ��������Ĺ����ڴ�ҽ��ڸý��̵�ҳ���ϣ��ǽ������ڴ�������ڴ����Ӧ��
            mem = (char*)shmat(shmid_, NULL, 0);
            if (mem == nullptr) {
                cout << "shmat share memory mapping fail!" << endl;
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
