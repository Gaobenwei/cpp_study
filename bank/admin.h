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
    admin();//初始化管理密码为“123456”
    void showmain(void);//开户、登录、退出界面
    void establish(void);//开户
    int cancel(void);//销户,正确返回0，失败返回-1
    int login(void);//登录
    void loginchoose(user&);//用户登录成功后选择功能界面
    int admintransfer(user&);//转账
    int writeinfile(void);//将用户信息写入文件
    int readinfile(void);//从文件读出用户信息
    int resetuserflag(void);//解冻

private:
    string adminpassword;
    vector<user> myuser;
    static int usernum;
};

#endif //BANK_ADMIN_H
