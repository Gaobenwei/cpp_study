//
// Created by 11048 on 2023/5/24.
//

#include "admin.h"

int admin::usernum=0;

admin::admin() {
    adminpassword="123456";
}
//开户业务
void admin::establish() {
    string temppassword;
    cout << "请输入管理密码" << endl;
    cin >> temppassword;
    if(temppassword==adminpassword)
    {
        user newuser;
        newuser.createuser();
        cout<<"开户成功"<<endl;
        usernum++;
        cout << "您现在是否想要存款:1、是 2、否" << endl;

        int ah;
        cin>>ah;
        if(1==ah)
        {
            newuser.deposit();
        }
        myuser.push_back(newuser);
        newuser.showuser();
    }
    else
    {
        cout << "密码输入有误" << endl;
    }
}
//销户
int admin::cancel()
{
    string temppassword;
    long tempbankcardID;
    int i=0;
    cout << "请输入管理密码" << endl;
    cin >> temppassword;

    if(temppassword==adminpassword)
    {
        cout << "请输入要注销的卡号" << endl;
        cin >> tempbankcardID;
        vector<user>::iterator iter=myuser.begin();
        while (iter!=myuser.end())
        {
            if(iter->findusertransfer(tempbankcardID)==0)
            {
                //根据ID找到用户
                myuser.erase(myuser.begin()+i);
                cout<<"删除账户成功"<<endl;
                usernum--;
                return 0;
            }
            iter++;
            i++;
        }
        cout<<"卡号错误"<<endl;
        return -1;
    }
    else
    {
        cout << "密码输入有误" << endl;
        return -1;
    }
}
//登录的功能
int admin::login()
{
    long tempbankcardID;
    string tempuserpassword;
    cout << "请输入卡号" << endl;
    cin >> tempbankcardID;

    vector<user>::iterator iter1=myuser.begin();
    while (iter1!=myuser.end())
    {
        if(iter1->findusertransfer(tempbankcardID)==0)
        {
            //在银行用户数据库找到卡号
            if(iter1->getflag()==1)
            {
                for(int i=3;i>0;--i)
                {
                    cout<<"输入密码，还有"<<i<<"次机会"<<endl;
                    cin>>tempuserpassword;
                    vector<user>::iterator iter2=myuser.begin();
                    while (iter2!=myuser.end())
                    {
                        if(iter2->finduser(tempbankcardID,tempuserpassword)==0)
                        {
                            //表示用户的账户和密码匹配,以当前用户名登录
                            loginchoose(*iter2);
                            return 0;
                        }
                        iter2++;
                    }
                    cout<<"密码错误"<<endl;
                }
                iter1->setflag();
                cout<<"账户冻结"<<endl;
                return -1;
            }
            else
            {
                cout<<"账户已被冻结"<<endl;
                return -1;
            }
        }
        iter1++;
    }
    cout<<"卡号错误"<<endl;
    return -1;
}

void admin::showmain()
{
    int ah;
    label1:cout<<"请选择功能：1、开户 2、登录 3、销户 4、解冻 5、退出" << endl;
    cin>>ah;
    switch (ah) {
        case 1:
            establish();
            goto label1;
        case 2:
            login();
            goto label1;
        case 3:
            cancel();
            goto label1;
        case 4:
            resetuserflag();
            goto label1;
        default:
            break;
    }

}

void admin::loginchoose(user &curuser)
{
    int ah;
    label2:	cout << "请选择功能：1、存款 2、取款 3、转账 4、查询 5、修改密码 6、退出" << endl;
    cin>>ah;
    switch (ah)
    {
        case 1:
            curuser.deposit();
            goto label2;
        case 2:
            curuser.dispense();
            goto label2;
        case 3:
            admintransfer(curuser);
            goto label2;
        case 4:
            curuser.showuser();
            goto label2;
        case 5:
            curuser.changepassword();
            goto label2;
        case 6:
            break;
        default:
            break;
    }
}

int admin::admintransfer(user &curuser) {
    long tempbankID;
    long curbankID=curuser.getbankcardID();
    int tempmoney;
    string tempname1;
    string tempname2;
    cout << "请输入对方账户卡号" << endl;
    cin >> tempbankID;
    if(tempbankID==curbankID)
    {
        cout << "无法转账给自己" << endl;
        return -1;
    }
    vector<user>::iterator iter=myuser.begin();
    while (iter!=myuser.end())
    {
        if(iter->findusertransfer(tempbankID)==0)
        {
            //找到了转账对方的用户信息
            tempname1=iter->getname();
            tempname2=curuser.getname();
            tempmoney=curuser.transferfrom(tempname1);
            if(tempmoney>0)
            {
                iter->transferto(tempmoney,tempname2);
                return 0;
            }
            return -1;
        }
        iter++;
    }
    cout << "卡号输入有误" << endl;
    return -1;
}

int admin::writeinfile(void) {
    streambuf *backup;
    ofstream fout;
    fout.open("./bank.txt");
    backup=cout.rdbuf();
    cout.rdbuf(fout.rdbuf());//重定向

    cout<<usernum<<endl;
    cout << user::bankcardID0 << endl;
    cout.rdbuf(backup);//重定向回来
    fout.close();


    fout.open("./bank.txt", ios::app);
    backup = cout.rdbuf();
    cout.rdbuf(fout.rdbuf());//重定向

    vector<user>::iterator iter=myuser.begin();
    while (iter!=myuser.end())
    {
        iter->writeuser();
        iter++;
    }
    cout.rdbuf(backup);//重定向回来
    fout.close();
    cout << "write called" << endl;

    return 0;
}

int admin::readinfile(void) {
    int i=0;
    streambuf * backup;
    ofstream fin;
    fin.open("./bank.txt",ios::in);
    backup=cin.rdbuf();

    cin.rdbuf(fin.rdbuf()); //重定向

    cin>>usernum;
    cin>>user::bankcardID0;

    for(int i=0;i<usernum;++i)
    {
        user tempuser;
        tempuser.readuser();
        myuser.push_back(tempuser);
    }

    cin.rdbuf(backup);
    fin.close();
    cout << "read called" << endl;

    return 0;
}

int admin::resetuserflag(void) {
    string temppassword;
    long tempbankcardID;
    cout << "请输入管理密码" << endl;
    cin >> temppassword;
    if(temppassword==adminpassword)
    {
        cout << "请输入要解冻的卡号" << endl;
        cin >> tempbankcardID;
        vector<user>::iterator iter=myuser.begin();
        while (iter!=myuser.end())
        {
            if(iter->findusertransfer(tempbankcardID)==0)
            {
                if (iter->getflag() == 1)
                {
                    cout << "您的账户未冻结" << endl;
                    return -1;
                }
                else
                {
                    iter->resetflag();
                    cout << "解冻成功" << endl;
                    return 0;
                }
            }
            iter++;
        }
        cout << "卡号输入有误" << endl;
        return -1;
    }
    else
    {
        cout << "密码输入有误" << endl;
        return -1;
    }
}

