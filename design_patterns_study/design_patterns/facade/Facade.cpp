
//门面模式
#include<iostream>

/*子系统
实现更复杂的子系统功能，并且不了解facade*/

class SubsystemA
{
    public:
        void suboperation()
        {
            std::cout << "Subsystem A method" << std::endl;
    // ...
        }
};

class SubsystemB
{
    public:
        void suboperation()
        {
            std::cout << "Subsystem B method" << std::endl;
            // ...
        }
};

class SubsystemC
{
public:
  void suboperation()
  {
    std::cout << "Subsystem C method" << std::endl;
    // ...
  }
  // ...
};

/*外观
将客户端请求委托给适当的子系统对象和易于使用的统一接口*/

class Facade
{
    public:
        Facade():subA(),subB(),subC(){}

        void operation1()
        {
            subA->suboperation();
            subB->suboperation();
        }

        void operation2()
        {
            subC->suboperation();
        }
    private:
        SubsystemA *subA;
        SubsystemB *subB;
        SubsystemC *subC;
};

int main()
{

    Facade *facade=new Facade();  //接口
    facade->operation1();  //设计子系统A,B
    facade->operation2();  //C
    delete facade;
    return 0;
}