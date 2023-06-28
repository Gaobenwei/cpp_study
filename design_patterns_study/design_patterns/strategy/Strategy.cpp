#include<iostream>

class Strategy // AbstractClass作用：定义算法的骨架，具体的算法由子类实现
{
private:
    /* data */
public:
    Strategy(/* args */);
    virtual ~Strategy();
    virtual void algorithmInterface()=0;
};

Strategy::Strategy(/* args */)
{
}

Strategy::~Strategy()
{
}

class ConcreteStrategyA:public Strategy // 作用：实现具体的算法
{
private:
    /* data */
public:
    ConcreteStrategyA(/* args */){}
    virtual ~ConcreteStrategyA(){}
    void algorithmInterface()override
    {
        std::cout << "ConcreteStrategyA" << std::endl;
    }
};

class ConscreateStrategyB:public Strategy
{
private:
    /* data */
public:
    ConscreateStrategyB(/* args */){}
    virtual ~ConscreateStrategyB(){}
    void algorithmInterface()override
    {
        std::cout << "ConscreateStrategyB" << std::endl;
    }
};


class ConscreateStrategyC:public Strategy
{
private:
    /* data */
public:
    ConscreateStrategyC(/* args */){}
    virtual ~ConscreateStrategyC(){}
    void algorithmInterface()override
    {
        std::cout << "ConscreateStrategyC" << std::endl;
    }
};

class Context // 作用：维护一个Strategy对象的引用，可以定义一个接口来让Strategy访问它的数据
{            // 作用：负责具体的算法的调用  
private:
    /* data */
    Strategy *strategy;
public:
    Context(Strategy *const s):strategy(s){}
    ~Context()
    {
        delete strategy;
    }

    void contextUnterface()
    {
        strategy->algorithmInterface();
    }
};

int main()
{
    Context context(new ConcreteStrategyA());
    context.contextUnterface();
    return 0;
}