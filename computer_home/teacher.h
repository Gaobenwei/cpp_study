//
// Created by 11048 on 2023/5/24.
//

#ifndef COMPUTER_HOME_TEACHER_H
#define COMPUTER_HOME_TEACHER_H

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include "identity.h"

class Teacher:public Identity
{
public:
    Teacher();
    Teacher(int emId,string name,string pwd);
    virtual void operMenu();
    //�鿴����ԤԼ
    void showAllOrder();

    //���ԤԼ
    void validOrder();

    int m_EmpId; //��ʦ���
};

#endif //COMPUTER_HOME_TEACHER_H
