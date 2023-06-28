#include<iostream>
#include<string>

/*原型
声明一个用于克隆自身的接口*/
class Prototype
{
public:
    virtual ~Prototype(){}
    virtual Prototype* clone()=0;
    virtual std::string type()=0;
};

/*
*具体原型A和B
*实现克隆自身的操作
*/

class ConcreatePrototypeA:public Prototype
{
public:
    ~ConcreatePrototypeA(){}

    Prototype* clone()
    {
        return new ConcreatePrototypeA();
    }

    std::string type()
    {
        return "type A";
    }
};

class ConcretePrototypeB:public Prototype
{
public:
    ~ConcretePrototypeB() {}
  
  Prototype* clone()
  {
    return new ConcretePrototypeB();
  }
  std::string type()
  {
    return "type B";
  }
};

/*
*客户端
通过要求原型克隆自己来创建一个新对象
*/

class Client
{
public:
    static void init()
    {
        types[0]=new ConcreatePrototypeA();
        types[1]=new ConcretePrototypeB();
    }

    static void remove()
    {
        delete types[0];
        delete types[1];
    }

    static Prototype* make(const int index)
    {
        if(index>=n_types)
            return nullptr;
        else 
        {
            return types[index]->clone();
        }
    }

    private:
    static Prototype* types[2];
    static int n_types;
};

int Client::n_types=2;
Prototype* Client::types[2];

int main()
{
    Client::init();
    Prototype* prototype1=Client::make(0);
    std::cout<<"Prototype: "<<prototype1->type()<<std::endl;

    delete prototype1;

    Prototype *prototype2 = Client::make( 1 );
    std::cout << "Prototype: " << prototype2->type() << std::endl;
    delete prototype2;

    Client::remove();


    return 0;
}