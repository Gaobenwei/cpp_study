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
//��ʦ�Ĳ˵�
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
            //�鿴����ԤԼ
            tea->showAllOrder();
        }
        else if (select == 2)
        {
            //���ԤԼ
            tea->validOrder();
        }
        else
        {
            delete teacher;
            cout << "ע���ɹ�" << endl;
            system("pause");
            system("cls");
            return;
        }
    }
}

//��¼����
void LoginIn(string fileName,int type)
{
    Identity *person= nullptr;
    ifstream ifs;
    ifs.open(fileName,ios::in);
    //�ļ�������
    if(!ifs.is_open())
    {
        cout<<"�ļ�������"<<endl;
        ifs.close();
        return;
    }

    int id=0;
    string name;
    string pwd;

    if(type==1)  //ѧ����¼
    {
        cout<<"����ѧ��"<<endl;
        cin>>id;
    }
    else if(type==2)
    {
        cout<<"�����ʦ����"<<endl;
        cin>>id;
    }
    cout<<"�����û�����"<<endl;
    cin>>name;
    cout << "���������룺 " << endl;
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
                cout << "ѧ����֤��¼�ɹ�!" << endl;
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
                cout << "��ʦ��֤��¼�ɹ�!" << endl;
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
                cout << "��֤��¼�ɹ�!" << endl;
                //��¼�ɹ��󣬰�������������Ա����
                system("pause");
                system("cls");
                //��������Ա����
                person = new Manager(name,pwd);
                managerMenu(person);
                return;
            }
        }
    }
    cout<<"��֤��¼ʧ��"<<endl;
    system("pause");
    system("cls");
    return;
}
//����Ա�˵�
void managerMenu(Identity* &manager)
{
    while (true)
    {
        //����Ա�˵�
        manager->operMenu();
        Manager *man=static_cast<Manager*>(manager);
        int select=0;
        cin>>select;
        if(select==1) //����˺�
        {
            cout<<"����˺�"<<endl;
            man->addPerson();
        }
        else if(2==select)
        {
            cout << "�鿴�˺�" << endl;
            man->showPerson();
        }
        else if (select == 3) //�鿴����
        {
            cout << "�鿴����" << endl;
            man->showComputer();
        }
        else if (select == 4) //���ԤԼ
        {
            cout << "���ԤԼ" << endl;
            man->cleanFile();
        }
        else
        {
            delete manager;
            cout << "ע���ɹ�" << endl;
            system("pause");
            system("cls");
            return;
        }
    }
}
//ѧ���˵�
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
                cout << "ע���ɹ�" << endl;
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
        cout << "======================  ��ӭ�������ǲ��ͻ���ԤԼϵͳ  =====================" << endl;
        cout << endl << "�������������" << endl;
        cout << "\t\t -------------------------------\n";
        cout << "\t\t|                               |\n";
        cout << "\t\t|          1.ѧ������           |\n";
        cout << "\t\t|                               |\n";
        cout << "\t\t|          2.��    ʦ           |\n";
        cout << "\t\t|                               |\n";
        cout << "\t\t|          3.�� �� Ա           |\n";
        cout << "\t\t|                               |\n";
        cout << "\t\t|          0.��    ��           |\n";
        cout << "\t\t|                               |\n";
        cout << "\t\t -------------------------------\n";
        cout << "��������ѡ��: ";
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
                cout<<"��ӭ�´�ʹ��"<<endl;
                system("pause");
                return 0;
                break;
            default:
                cout<<"���������⣬��������"<<endl;
                system("pause");
                system("cls");
                break;
        }
    }
    system("pasue");
    return 0;
}
