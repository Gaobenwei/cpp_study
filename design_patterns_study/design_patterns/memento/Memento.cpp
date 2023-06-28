#include<iostream>
#include<vector>

/*备忘录
存储发起者对象的内部状态，防止发起者以外的对象访问*/
class Memento
{
    private:
    //只有发起者可以访问
        friend class Originator;
        Memento(const int s):state(s){}

        void setState(const int s)
        {
            state=s;
        }

        int getState()
        {
            return state;
        }

    private:
        int state;
};

/*创建包含其当前内部状态快照的纪念品，并使用该纪念品恢复其内部状态*/
class Originator
{
    private:
        int state;
    public:
        //仅用于打印目的
        void setState(const int s)
        {
            std::cout << "Set state to " << s << "." << std::endl;
            state = s;
        }

        int getState()
        {
            return state;
        }

        void setMemento(Memento* const m)
        {
            state=m->getState();
        }

        Memento* createMemento()
        {
            return new Memento(state);
        }
};

//管理快照的
class CareTaker
{
    public:
        CareTaker(Originator* const o):originator(o){}

        ~CareTaker()
        {
            for(unsigned int i=0;i<history.size();++i)
            {
                delete history.at(i);
            }
            history.clear();
        }

        void save()
        {
            std::cout<<"Save state."<<std::endl;
            history.push_back(originator->createMemento());
        }

        void undo()  //向上恢复快照
        {
            if(history.empty())
            {
                std::cout<<"Unable to undo state."<<std::endl;
                return ;
            }

            Memento* m=history.back();
            originator->setMemento(m);
            std::cout<<"Undo state."<<std::endl;

            history.pop_back();
            delete m;
        }
    private:
        Originator* originator;
        std::vector<Memento*> history;
};


int main()
{
    Originator* originator=new Originator();
    CareTaker* caretaker=new CareTaker(originator);

    originator->setState(1);  //状态1
    caretaker->save();  //保存状态1快照

    originator->setState(2);
    caretaker->save();

    originator->setState(3);
    caretaker->undo(); //恢复快照2

    std::cout << "Actual state is " << originator->getState() << "." << std::endl;

    delete originator;
    delete caretaker;


    return 0;
}