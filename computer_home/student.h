//
// Created by 11048 on 2023/5/24.
//

#ifndef COMPUTER_HOME_STUDENT_H
#define COMPUTER_HOME_STUDENT_H
#include <iostream>
#include <vector>
#include "computerRoom.h"

using namespace std;
#include "identity.h"
class Student:public Identity
{
public:
    Student();
    Student(int id,string name,string pwd);
    //�˵�����
    virtual void operMenu();
    //����ԤԼ
    void applyOrder();
    //�鿴�ҵ�ԤԼ
    void showMyOrder();
    //�鿴����ԤԼ
    void showAllOrder();
    //ȡ��ԤԼ
    void cancelOrder();
    int m_Id;
    vector<ComputerRoom> vCom;
};
#endif //COMPUTER_HOME_STUDENT_H
