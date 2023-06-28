//
// Created by 11048 on 2023/5/24.
//

#ifndef COMPUTER_HOME_IDENTITY_H
#define COMPUTER_HOME_IDENTITY_H

#include <iostream>
using namespace std;
class Identity
{
public:
    virtual void operMenu()=0;
    string m_Name;
    string m_Pwd;
private:
};

#endif //COMPUTER_HOME_IDENTITY_H
