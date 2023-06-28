#include <iostream>
using namespace std;
#include "gloablFile.h"
#include "identity.h"
#include <fstream>
#include <string>
#include "student.h"
#include "teacher.h"
#include "manager.h"

void managerMenu(Identity* &manager);
void studentMenu(Identity* &student);
//教师的菜单
void TeacherMenu(Identity* &teacher)
{
    while(true)
    {
        teacher->operMenu();
        Teacher *tea=(Teacher*)teacher;
        int select=0;
        cin>>select;
        if(select==1)
        {
            //查看所有预约
            tea->showAllOrder();
        }
        else if (select == 2)
        {
            //审核预约
            tea->validOrder();
        }
        else
        {
            delete teacher;
            cout << "注销成功" << endl;
            system("pause");
            system("cls");
            return;
        }
    }
}

//登录功能
void LoginIn(string fileName,int type)
{
    Identity *person= nullptr;
    ifstream ifs;
    ifs.open(fileName,ios::in);
    //文件不存在
    if(!ifs.is_open())
    {
        cout<<"文件不存在"<<endl;
        ifs.close();
        return;
    }

    int id=0;
    string name;
    string pwd;

    if(type==1)  //学生登录
    {
        cout<<"输入学号"<<endl;
        cin>>id;
    }
    else if(type==2)
    {
        cout<<"输入教师号码"<<endl;
        cin>>id;
    }
    cout<<"输入用户名："<<endl;
    cin>>name;
    cout << "请输入密码： " << endl;
    cin >> pwd;
    if(1==type)
    {
        int fId;
        string fname;
        string fpwd;
        while (ifs>>fId&&ifs>>fname&&ifs>>fpwd)
        {
            if(id==fId&&name==fname&&pwd==fpwd)
            {
                cout << "学生验证登录成功!" << endl;
                system("pause");
                system("cls");
                person=new Student(id,name,pwd);
                studentMenu(person);
                return;
            }
        }
    }
    else if(2==type)
    {
        int Fid;
        string fname;
        string fpwd;
        while (ifs>>Fid && ifs>>fname && ifs>>fpwd)
        {
            if(id==Fid && name==fname && fpwd==pwd)
            {
                cout << "教师验证登录成功!" << endl;
                system("pause");
                system("cls");
                person = new Teacher(id, name, pwd);
                TeacherMenu(person);
                return;
            }
        }
    }
    else if(3==type)
    {
        string fname;
        string fpwd;
        while (ifs>>fname && ifs>>fpwd)
        {
            if(name==fname && pwd==fpwd)
            {
                cout << "验证登录成功!" << endl;
                //登录成功后，按任意键进入管理员界面
                system("pause");
                system("cls");
                //创建管理员对象
                person = new Manager(name,pwd);
                managerMenu(person);
                return;
            }
        }
    }
    cout<<"验证登录失败"<<endl;
    system("pause");
    system("cls");
    return;
}
//管理员菜单
void managerMenu(Identity* &manager)
{
    while (true)
    {
        //管理员菜单
        manager->operMenu();
        Manager *man=static_cast<Manager*>(manager);
        int select=0;
        cin>>select;
        if(select==1) //添加账号
        {
            cout<<"添加账号"<<endl;
            man->addPerson();
        }
        else if(2==select)
        {
            cout << "查看账号" << endl;
            man->showPerson();
        }
        else if (select == 3) //查看机房
        {
            cout << "查看机房" << endl;
            man->showComputer();
        }
        else if (select == 4) //清空预约
        {
            cout << "清空预约" << endl;
            man->cleanFile();
        }
        else
        {
            delete manager;
            cout << "注销成功" << endl;
            system("pause");
            system("cls");
            return;
        }
    }
}
//学生菜单
void studentMenu(Identity* &student)
{
    while (1)
    {
        student->operMenu();

        Student *stu=(Student*)student;
        int select=0;
        cin>>select;
        switch (select) {
            case 1:
                stu->applyOrder();
                break;
            case 2:
                stu->showMyOrder();
                break;
            case 3:
                stu->showAllOrder();
                break;
            case 4:
                stu->cancelOrder();
                break;
            default:
                delete student;
                cout << "注销成功" << endl;
                system("pause");
                system("cls");
                return;
                break;
        }
    }
}

int main() {
    int select=0;
    while(1)
    {
        cout << "======================  欢迎来到传智播客机房预约系统  =====================" << endl;
        cout << endl << "请输入您的身份" << endl;
        cout << "\t\t -------------------------------\n";
        cout << "\t\t|                               |\n";
        cout << "\t\t|          1.学生代表           |\n";
        cout << "\t\t|                               |\n";
        cout << "\t\t|          2.老    师           |\n";
        cout << "\t\t|                               |\n";
        cout << "\t\t|          3.管 理 员           |\n";
        cout << "\t\t|                               |\n";
        cout << "\t\t|          0.退    出           |\n";
        cout << "\t\t|                               |\n";
        cout << "\t\t -------------------------------\n";
        cout << "输入您的选择: ";
        cin>>select;
        switch (select) {
            case 1:
                LoginIn(STUDENT_FILE,1);
                break;
            case 2:
                LoginIn(TEACHER_FILE,2);
                break;
            case 3:
                LoginIn(ADMIN_FILE,3);
                break;
            case 0:
                cout<<"欢迎下次使用"<<endl;
                system("pause");
                return 0;
                break;
            default:
                cout<<"输入有问题，重新输入"<<endl;
                system("pause");
                system("cls");
                break;
        }
    }
    system("pasue");
    return 0;
}
