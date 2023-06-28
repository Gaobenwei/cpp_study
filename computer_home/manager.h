//
// Created by 11048 on 2023/5/25.
//

#ifndef COMPUTER_HOME_MANAGER_H
#define COMPUTER_HOME_MANAGER_H
#include "computerRoom.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "student.h"
#include "teacher.h"

using namespace std;
#include "identity.h"
class Manager:public Identity
{
public:
    //Ĭ�Ϲ���
    Manager();

    //�вι���  ����Ա����������
    Manager(string name, string pwd);

    //ѡ��˵�
    virtual void operMenu();

    //����˺�
    void addPerson();

    //�鿴�˺�
    void showPerson();

    //�鿴������Ϣ
    void showComputer();

    //���ԤԼ��¼
    void cleanFile();

    //��ʼ������
    void initVector();
    vector<Student> vStu;
    vector<Teacher> vTea;
    vector<ComputerRoom> vCom;
    bool checkRepeat(int id,int type);
};

#endif //COMPUTER_HOME_MANAGER_H
