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
    //菜单界面
    virtual void operMenu();
    //申请预约
    void applyOrder();
    //查看我的预约
    void showMyOrder();
    //查看所有预约
    void showAllOrder();
    //取消预约
    void cancelOrder();
    int m_Id;
    vector<ComputerRoom> vCom;
};
#endif //COMPUTER_HOME_STUDENT_H
