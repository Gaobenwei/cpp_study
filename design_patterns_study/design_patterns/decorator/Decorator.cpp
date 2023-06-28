#include<iostream>

/*
*组件
定义了一个接口，对象可以有责任
*是动态添加的*/

class Component 
{
    public:
        virtual ~Component() {}
        virtual void operation()=0;
};

/**定义了一个可以附加额外职责的对象*/
class ConcreteComponent:public Component  
{
    public:
        ~ConcreteComponent() {}

        void operation() override
        {
            std::cout<<"ConcreteComponent operation..."<<std::endl;
        }
};



/*装饰
维护一个对Component对象的引用，并定义一个符合Component接口的接口
*/

class Decorator:public Component
{
protected:
    Component* m_com;  //一方面继承自基类，一方面又包含一个基类的指针，这就是装饰的关键，装饰类中包含一个真实对象的引用，继承可以理解某种子类，而指针可以是添加了功能
public:
    ~Decorator() {}
    Decorator(Component* com):m_com(com) {}

    virtual void operation() override
    {
        m_com->operation();
    }

};

/*为组件添加职责(可以扩展组件的状态)*/
class ConcreteDecoratorA:public Decorator
{
public:
    ~ConcreteDecoratorA() {}
    ConcreteDecoratorA(Component* com):Decorator(com) {}

    void operation()
    {
        Decorator::operation();  //在重写函数中依旧完成原有功能
        std::cout<<"ConcreteDecoratorA operation..."<<std::endl; //类似于添加的职责
    }
};

class ConcreteDecoratorB : public Decorator
{
    public:
        ConcreteDecoratorB(Component *c):Decorator(c){}

        void operation()
        {
            Decorator::operation();
            std::cout << "Decorator B" << std::endl;
        }
};

int main()
{
    ConcreteComponent *cc=new ConcreteComponent();//创建某个对象
    ConcreteDecoratorB *db=new ConcreteDecoratorB(cc);  //针对类系统中的某个对象装饰，加功能B
    ConcreteDecoratorA *da=new ConcreteDecoratorA(cc);
//针对类系统中的某个对象装饰，加功能A

    Component *comment=da;
    comment->operation();
    comment=db;
    comment->operation();
    delete da;
    delete db;
    delete cc;
    return 0;
}