
#include<iostream>

/*实现者
定义实现类的接口*/

class Implementor
{
public:
    virtual ~Implementor(){}
    virtual void action()=0;
    //...    
};

/*具体实现者
*实现Implementor接口并定义具体实现*/

class ConcreteImplemA :public Implementor
{
private:
    /* data */
public:
    ~ConcreteImplemA(){}
    void action()override
    {
        std::cout << "Concrete Implementor A" << std::endl;

    }
};

class ConcreateImpB:public Implementor
{
    public:
    ~ConcreateImpB(){}
    void action()
    {
        std::cout << "Concrete Implementor B" << std::endl;

    }
};

/*抽象
定义抽象的接口*/
class Abstraction
{
public:
    virtual ~Abstraction(){}

    virtual void operation()=0;
};

/*扩展抽象定义的接口*/
class RefinedAbstraction :public Abstraction
{
    public:
    ~RefinedAbstraction(){}
    RefinedAbstraction(Implementor *impl):implementor(impl){}
    void operation()override
    {
        implementor->action();
    }
    private:
    Implementor * implementor; //一个指向另一种变化维度的指针
};

int main()
{
    Implementor *ia=new ConcreteImplemA;
    Implementor *ib=new ConcreateImpB;

    Abstraction *abstract1=new RefinedAbstraction(ia);
    abstract1->operation();

    Abstraction *abstract2=new RefinedAbstraction(ib);
    abstract2->operation();

    delete abstract1;
    delete abstract2;

    delete ia;
    delete ib;

    return 0;
}