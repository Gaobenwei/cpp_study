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
    //����ԤԼ��¼
    void updateOrder();

    //��¼������  key --- ��¼������  value --- �����¼�ļ�ֵ����Ϣ
    map<int, map<string, string>> m_orderData;

    //ԤԼ��¼����
    int m_Size;
};


#endif //COMPUTER_HOME_ORDERFILE_H
