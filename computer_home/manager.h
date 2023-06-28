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
    //默认构造
    Manager();

    //有参构造  管理员姓名，密码
    Manager(string name, string pwd);

    //选择菜单
    virtual void operMenu();

    //添加账号
    void addPerson();

    //查看账号
    void showPerson();

    //查看机房信息
    void showComputer();

    //清空预约记录
    void cleanFile();

    //初始化容器
    void initVector();
    vector<Student> vStu;
    vector<Teacher> vTea;
    vector<ComputerRoom> vCom;
    bool checkRepeat(int id,int type);
};

#endif //COMPUTER_HOME_MANAGER_H
