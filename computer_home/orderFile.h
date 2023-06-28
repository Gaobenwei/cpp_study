//
// Created by 11048 on 2023/5/25.
//

#ifndef COMPUTER_HOME_ORDERFILE_H
#define COMPUTER_HOME_ORDERFILE_H
#include<iostream>
using namespace std;
#include <map>
#include "gloablFile.h"
class OrderFile
{
public:
    OrderFile();
    //更新预约记录
    void updateOrder();

    //记录的容器  key --- 记录的条数  value --- 具体记录的键值对信息
    map<int, map<string, string>> m_orderData;

    //预约记录条数
    int m_Size;
};


#endif //COMPUTER_HOME_ORDERFILE_H
