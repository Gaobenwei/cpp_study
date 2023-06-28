//
// Created by 11048 on 2023/5/24.
//

#include "admin.h"

int admin::usernum=0;

admin::admin() {
    adminpassword="123456";
}
//����ҵ��
void admin::establish() {
    string temppassword;
    cout << "�������������" << endl;
    cin >> temppassword;
    if(temppassword==adminpassword)
    {
        user newuser;
        newuser.createuser();
        cout<<"�����ɹ�"<<endl;
        usernum++;
        cout << "�������Ƿ���Ҫ���:1���� 2����" << endl;

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
        cout << "������������" << endl;
    }
}
//����
int admin::cancel()
{
    string temppassword;
    long tempbankcardID;
    int i=0;
    cout << "�������������" << endl;
    cin >> temppassword;

    if(temppassword==adminpassword)
    {
        cout << "������Ҫע���Ŀ���" << endl;
        cin >> tempbankcardID;
        vector<user>::iterator iter=myuser.begin();
        while (iter!=myuser.end())
        {
            if(iter->findusertransfer(tempbankcardID)==0)
            {
                //����ID�ҵ��û�
                myuser.erase(myuser.begin()+i);
                cout<<"ɾ���˻��ɹ�"<<endl;
                usernum--;
                return 0;
            }
            iter++;
            i++;
        }
        cout<<"���Ŵ���"<<endl;
        return -1;
    }
    else
    {
        cout << "������������" << endl;
        return -1;
    }
}
//��¼�Ĺ���
int admin::login()
{
    long tempbankcardID;
    string tempuserpassword;
    cout << "�����뿨��" << endl;
    cin >> tempbankcardID;

    vector<user>::iterator iter1=myuser.begin();
    while (iter1!=myuser.end())
    {
        if(iter1->findusertransfer(tempbankcardID)==0)
        {
            //�������û����ݿ��ҵ�����
            if(iter1->getflag()==1)
            {
                for(int i=3;i>0;--i)
                {
                    cout<<"�������룬����"<<i<<"�λ���"<<endl;
                    cin>>tempuserpassword;
                    vector<user>::iterator iter2=myuser.begin();
                    while (iter2!=myuser.end())
                    {
                        if(iter2->finduser(tempbankcardID,tempuserpassword)==0)
                        {
                            //��ʾ�û����˻�������ƥ��,�Ե�ǰ�û�����¼
                            loginchoose(*iter2);
                            return 0;
                        }
                        iter2++;
                    }
                    cout<<"�������"<<endl;
                }
                iter1->setflag();
                cout<<"�˻�����"<<endl;
                return -1;
            }
            else
            {
                cout<<"�˻��ѱ�����"<<endl;
                return -1;
            }
        }
        iter1++;
    }
    cout<<"���Ŵ���"<<endl;
    return -1;
}

void admin::showmain()
{
    int ah;
    label1:cout<<"��ѡ���ܣ�1������ 2����¼ 3������ 4���ⶳ 5���˳�" << endl;
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
    label2:	cout << "��ѡ���ܣ�1����� 2��ȡ�� 3��ת�� 4����ѯ 5���޸����� 6���˳�" << endl;
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
    cout << "������Է��˻�����" << endl;
    cin >> tempbankID;
    if(tempbankID==curbankID)
    {
        cout << "�޷�ת�˸��Լ�" << endl;
        return -1;
    }
    vector<user>::iterator iter=myuser.begin();
    while (iter!=myuser.end())
    {
        if(iter->findusertransfer(tempbankID)==0)
        {
            //�ҵ���ת�˶Է����û���Ϣ
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
    cout << "������������" << endl;
    return -1;
}

int admin::writeinfile(void) {
    streambuf *backup;
    ofstream fout;
    fout.open("./bank.txt");
    backup=cout.rdbuf();
    cout.rdbuf(fout.rdbuf());//�ض���

    cout<<usernum<<endl;
    cout << user::bankcardID0 << endl;
    cout.rdbuf(backup);//�ض������
    fout.close();


    fout.open("./bank.txt", ios::app);
    backup = cout.rdbuf();
    cout.rdbuf(fout.rdbuf());//�ض���

    vector<user>::iterator iter=myuser.begin();
    while (iter!=myuser.end())
    {
        iter->writeuser();
        iter++;
    }
    cout.rdbuf(backup);//�ض������
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

    cin.rdbuf(fin.rdbuf()); //�ض���

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
    cout << "�������������" << endl;
    cin >> temppassword;
    if(temppassword==adminpassword)
    {
        cout << "������Ҫ�ⶳ�Ŀ���" << endl;
        cin >> tempbankcardID;
        vector<user>::iterator iter=myuser.begin();
        while (iter!=myuser.end())
        {
            if(iter->findusertransfer(tempbankcardID)==0)
            {
                if (iter->getflag() == 1)
                {
                    cout << "�����˻�δ����" << endl;
                    return -1;
                }
                else
                {
                    iter->resetflag();
                    cout << "�ⶳ�ɹ�" << endl;
                    return 0;
                }
            }
            iter++;
        }
        cout << "������������" << endl;
        return -1;
    }
    else
    {
        cout << "������������" << endl;
        return -1;
    }
}

