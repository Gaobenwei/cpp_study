//
// Created by 11048 on 2023/5/24.
//

#ifndef BANK_USER_H
#define BANK_USER_H

#include <string>
#include <list>
using namespace std;
//用户类
class user
{
public:
    user();
    virtual ~user()=default;
    void showuser();
    void createuser();
    int findusertransfer(long&); //查找用户用于转账,正确返回0，失败返回-1、销户
    int finduser(long&,string&);//查找用户并匹配密码用于登录,正确返回0，失败返回-1
    void deposit(); //存款
    void dispense(void);//取款
    int transferfrom(string&);//转账
    void transferto(int&, string&);//转账
    int changepassword(void);//修改密码
    void writeuser(void);//写到文件中
    void readuser(void);//从文件中读取
    string gettime(void);//获取当前时间
    string getname(void);//获取当前账户姓名
    int getflag(void);//获取冻结标志
    void setflag(void);//冻结
    void resetflag(void);//解冻
    long getbankcardID(void);//获取当前账户，转账时比较
    string changeint(int &);//将整形转换成string类
    static long bankcardID0;//定义银行初始卡号为静态成员变量

private:
    string name;
    long bankcardID; //银行卡号
    string userpassword; //用户密码
    string ID; //ID
    int income; //存入
    int expend;//支出
    int remaining; //余额
    string opdate; //操作日期
    string note; //备注
    list<string> usercheck; //用户操作账单
    int userchecknum; //用户账单数目
    int flag;
};

#endif //BANK_USER_H
