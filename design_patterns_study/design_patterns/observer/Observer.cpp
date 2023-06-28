#include<iostream>

/*
为对象定义一个更新接口，该接口应在主题发生更改时收到通知
*/

#include<vector>

class Subject;// 前向声明

class Observer // 观察者
{
private:
    /* data */
public:
    Observer(/* args */){}
    virtual ~Observer(){}
    virtual void update(Subject *sub)=0; // 更新接口
    virtual int get_state()=0; // 获取状态
};

//具体的观察者
//将感兴趣的状态存储到ConcreteObserver对象中，并在状态发生变化时向其观察者发送通知
class ConcreteObserverA:public Observer
{
private:
    int state; // 状态
public:
    ConcreteObserverA(const int state_):state(state_)
    {}
    virtual ~ConcreteObserverA(){}
    void update(Subject *sub)override ;// 更新接口

    int get_state()override
    {
        return state;
    }
};

//主题
//知道它的观察者，并提供一个接口来附加和分离观察者
class Subject
{
private:
    /* data */
    std::vector<Observer*> observers; // 观察者
public:
    Subject(){}
    virtual ~Subject(){}
    void attach(Observer *ob) // 附加观察者
    {
        observers.push_back(ob);
    }

    void detach(const int index) // 分离观察者
    {
        observers.erase(observers.begin()+index); 
    }

    void notify() // 通知观察者
    {
        for(auto ob:observers)
        {
            ob->update(this);
        }
    }

    virtual int get_state()=0; // 获取状态
    virtual void set_state(const int state)=0; // 设置状态
};

/*具体的主题
存储状态应该与主题保持一致*/
class ConcreteSubject:public Subject
{
private:
    int state; // 状态
public:
    //ConcreteSubject(const int state_):state(state_){}
    virtual ~ConcreteSubject(){}
    int get_state()override
    {
        return state;
    }

    void set_state(const int state)override
    {
        this->state=state;
    }


    
};

void ConcreteObserverA::update(Subject *sub) // 更新接口
{
    state=sub->get_state(); // 获取状态
    std::cout << "ConcreteObserverA::update" << std::endl;
    std::cout << "state:" << state << std::endl;
}

int main()
{
    ConcreteObserverA observer1(1);
    ConcreteObserverA observer2(2);

    std::cout << "Observer 1 state: " << observer1.get_state() << std::endl;
    std::cout << "Observer 2 state: " << observer2.get_state() << std::endl;
    
    Subject *subject=new ConcreteSubject();
    subject->attach(&observer1);
    subject->attach(&observer2);

    subject->set_state(10);
    subject->notify();

     std::cout << "Observer 1 state: " << observer1.get_state() << std::endl;
  std::cout << "Observer 2 state: " << observer2.get_state() << std::endl;
    
    delete subject;
    return 0;

}
