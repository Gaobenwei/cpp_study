#include<iostream>

class AbstractClass
{
private:
    /* data */
public:
    AbstractClass(/* args */);
    virtual ~AbstractClass();
    void templateMethod()
    {
        primit1();
        primit2();
    }

    virtual void primit1()=0;
    virtual void primit2()=0;
};

AbstractClass::AbstractClass(/* args */)
{
}

AbstractClass::~AbstractClass()
{
}

class ConcreateClass:public AbstractClass
{
    private:
    public:
    ConcreateClass()=default;
    virtual ~ConcreateClass(){}

    virtual void primit1()override
    {
        std::cout << "Primitive operation 1" << std::endl;
    }

    void primit2()override
    {
        std::cout << "Primitive operation 2" << std::endl;
    }
};

int main()
{
    AbstractClass *tm=new ConcreateClass;
    tm->templateMethod();
    delete tm;
    return 0;
}
