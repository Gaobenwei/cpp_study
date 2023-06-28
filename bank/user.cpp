//
// Created by 11048 on 2023/5/24.
//
#include "user.h"
#include <iostream>
#include <string>
#include <list>
#include <cstring>
#include<time.h>
using namespace std;

long user::bankcardID0=633304;
user::user()
{
    income=0;
    expend=0;
    remaining=0;
    userchecknum=0;
    flag=1;
}

//�����û������� ���� ID
void user::createuser()
{
    cout << "����������" << endl;
    cin >> this->name;
    cout << "����������" << endl;
    cin >> userpassword;
    cout << "����������ID" << endl;
    cin >> ID;
    bankcardID = bankcardID0++;
}

//��ʾ�˻���Ϣ����ϸ
void user::showuser() {
    cout << "������" << name << endl;
    cout << "���п��ţ�" << bankcardID << endl;
    cout << "����ID��" << ID << endl;
    cout << "��" << remaining << endl;
    //��ʾ���û����˻���ϸ
    list<string>::iterator iter=usercheck.begin();
    while (iter!=usercheck.end())
    {
        cout<<*iter<<endl;
        iter++;
    }
}
//�ж����п�ID�ж��Ƿ��ǵ�ǰ�û�
int user::findusertransfer(long &bi)
{
    if(bankcardID==bi)
        return 0;
    return -1;
}

//�жϸ������Ƿ��ǵ�ǰ�û�������
int user::finduser(long & bi, std::string & pw)
{
    if(bankcardID==bi && userpassword==pw)
        return 0;
    return -1;
}
//���
void user::deposit()
{
    cout << "����������" << endl;
    cin>>income;
    remaining+=income;
    cout<<"��ǰ���:"<<remaining<<endl;
    //����һ��������¼
    string newcheck;
    string string1="+";
    string string2= changeint(income);
    string string3= "---��";
    string string4= changeint(remaining);
    string string5 = gettime();
    string string6 = "---ATM���---";
    newcheck = string1 + string2 + string3 + string4 + string6 + string5;
    usercheck.push_back(newcheck);
    userchecknum++;
}
//ȡ��
void user::dispense()
{
    cout<<"ȡ����"<<endl;
    cin>>expend;
    if(expend>remaining)
    {
        cout<<"����"<<endl;
        return;
    }
    remaining-=expend;
    string newcheck;
    string string1="-";
    string string2= changeint(expend);
    string string3= "---��";
    string string4= changeint(remaining);
    string string5 = gettime();
    string string6 = "---ATMȡ��---";
    newcheck = string1 + string2 + string3 + string4 + string6 + string5;
    usercheck.push_back(newcheck);
    userchecknum++;
}
//tempname �Է������� ת��ȥ װ��
int user::transferfrom(std::string & tempname)
{
    cout << "������ת�˽��" << endl;
    cin>>income;
    if(remaining<income)
    {
        cout<<"�˻�����"<<endl;
        return -1;
    }
    else
    {
        remaining-=income;
        string newcheck;
        string string1 = "-";
        string string2 = changeint(income);
        string string3 = "---���:";
        string string4 = changeint(remaining);
        string string5 = gettime();
        string string6 = "---ת�˸�";
        string string7 = "---";

        newcheck = string1 + string2 + string3 + string4 + string6 + tempname + string7 + string5;
        usercheck.push_back(newcheck);
        userchecknum++;
        return income;
    }
}
//ת��
void user::transferto(int & in, std::string & tempname)
{
    income=in;
    remaining+=income;
    string newcheck;
    string string1 = "+";
    string string2 = changeint(income);
    string string3 = "---���:";
    string string4 = changeint(remaining);
    string string5 = gettime();
    string string6 = "---����";
    string string7 = "��ת��---";
    newcheck = string1 + string2 + string3 + string4 + string6 + tempname + string7 + string5;
    usercheck.push_back(newcheck);
    userchecknum++;

}
//�޸��û�����
int user::changepassword()
{
    string ps1,ps2,ps3;
    cout << "�����뵱ǰ����" << endl;
    cin>>ps1;
    while(ps1==userpassword)
    {
        cout << "�������µ�����" << endl;
        cin>>ps2;
        cout << "���ٴ������µ�����" << endl;
        cin>>ps3;
        if(ps2==ps3)
        {
            userpassword=ps2;
            return 0;
        }
        else
        {
            cout << "�����벻һ�£�����������" << endl;
            continue;
        }
    }
    cout<<"�������"<<endl;
    return -1;
}
//�����û���Ϣ���˻���ϸ ���ļ��� cout �ض����ļ�
void user::writeuser()
{
    cout << userchecknum << endl;
    cout << name << endl;//����
    cout << bankcardID << endl;//���п���
    cout << userpassword << endl;//�û�����
    cout << ID << endl;//����ID
    cout << remaining << endl;//���
    list<string>::iterator iter=usercheck.begin();
    while (iter!=usercheck.end())
    {
        cout<<*iter<<endl;
        iter++;
    }

}
//cin�ض����ļ�
void user::readuser()
{
    string tempcheck;

    cin >> userchecknum;
    cin >> name;//����
    cin >> bankcardID;//���п���
    cin >> userpassword;//�û�����
    cin >> ID;//����ID
    cin >> remaining;//���
    for(int i=0;i<userchecknum;++i)
    {
        cin>>tempcheck;
        usercheck.push_back(tempcheck);
    }
}

string user::gettime()
{
    string mytime;
    time_t timep1;
    char *timep2;
    time(&timep1);
    timep2 = ctime(&timep1);
    timep2 = strtok(timep2, "\n");

    string p1 = strtok(timep2, " ");
    string p2 = strtok(NULL, " ");
    string p3 = strtok(NULL, " ");
    string p4 = strtok(NULL, " ");
    string p5 = strtok(NULL, " ");
    string p = "/";
    mytime = p1 + p + p2 + p + p3 + p + p4 + p + p5;

    return mytime;

}
string user::getname()
{
    return name;
}
string user::changeint(int &num)
{
    char buf[15]={0};
    string retstring;
    sprintf(buf,"%d",num);
    retstring=buf;
    return retstring;
}
long user::getbankcardID(void)
{
    return bankcardID;
}

int user::getflag()
{
    return flag;
}
void user::setflag() //����
{
    flag=0;
}
void user::resetflag(void)//�ⶳ
{
    flag = 1;
}