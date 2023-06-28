//
// Created by 11048 on 2023/5/24.
//

#ifndef BANK_ADMIN_H
#define BANK_ADMIN_H

#include "user.h"
#include <iostream>
#include <string>
#include <list>
#include <cstring>
#include <vector>
#include <fstream>
using namespace std;


class admin
{
public:
    admin();//��ʼ����������Ϊ��123456��
    void showmain(void);//��������¼���˳�����
    void establish(void);//����
    int cancel(void);//����,��ȷ����0��ʧ�ܷ���-1
    int login(void);//��¼
    void loginchoose(user&);//�û���¼�ɹ���ѡ���ܽ���
    int admintransfer(user&);//ת��
    int writeinfile(void);//���û���Ϣд���ļ�
    int readinfile(void);//���ļ������û���Ϣ
    int resetuserflag(void);//�ⶳ

private:
    string adminpassword;
    vector<user> myuser;
    static int usernum;
};

#endif //BANK_ADMIN_H
