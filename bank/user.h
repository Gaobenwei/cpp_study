//
// Created by 11048 on 2023/5/24.
//

#ifndef BANK_USER_H
#define BANK_USER_H

#include <string>
#include <list>
using namespace std;
//�û���
class user
{
public:
    user();
    virtual ~user()=default;
    void showuser();
    void createuser();
    int findusertransfer(long&); //�����û�����ת��,��ȷ����0��ʧ�ܷ���-1������
    int finduser(long&,string&);//�����û���ƥ���������ڵ�¼,��ȷ����0��ʧ�ܷ���-1
    void deposit(); //���
    void dispense(void);//ȡ��
    int transferfrom(string&);//ת��
    void transferto(int&, string&);//ת��
    int changepassword(void);//�޸�����
    void writeuser(void);//д���ļ���
    void readuser(void);//���ļ��ж�ȡ
    string gettime(void);//��ȡ��ǰʱ��
    string getname(void);//��ȡ��ǰ�˻�����
    int getflag(void);//��ȡ�����־
    void setflag(void);//����
    void resetflag(void);//�ⶳ
    long getbankcardID(void);//��ȡ��ǰ�˻���ת��ʱ�Ƚ�
    string changeint(int &);//������ת����string��
    static long bankcardID0;//�������г�ʼ����Ϊ��̬��Ա����

private:
    string name;
    long bankcardID; //���п���
    string userpassword; //�û�����
    string ID; //ID
    int income; //����
    int expend;//֧��
    int remaining; //���
    string opdate; //��������
    string note; //��ע
    list<string> usercheck; //�û������˵�
    int userchecknum; //�û��˵���Ŀ
    int flag;
};

#endif //BANK_USER_H
