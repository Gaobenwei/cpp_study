#include<iostream>

/*产品A
产品实现相同的接口，以便类可以引用接口而不是具体的产品*/

class ProductA
{
public:
    virtual ~ProductA(){}
    virtual const char* getName()=0;
};

/*定义由具体工厂创建的对象*/
class ConcreteProductAX:public ProductA
{
public:
    ~ConcreteProductAX(){}

    const char * getName()override
    {
        return "A-X";
    }
};

class ConcreteProductAY:public ProductA
{
public:
    ~ConcreteProductAY(){}

    const char * getName()override
    {
        return "A-Y";
    }
};

/*产品B
与产品A一样，产品B声明了具体产品的接口，每个接口都可以生产一整套产品*/

class ProductB
{
public:
  virtual ~ProductB() {}
  
  virtual const char* getName() = 0;
  // ...
};

/*与具体的产品类相同*/
class ConcreteProductBX : public ProductB
{
public:
  ~ConcreteProductBX() {}
  
  const char* getName()
  {
    return "B-X";
  }
  // ...
};

class ConcreteProductBY : public ProductB
{
public:
  ~ConcreteProductBY() {}
  
  const char* getName()
  {
    return "B-Y";
  }
  // ...
};

/*抽象工厂
为创建一系列产品提供一个抽象接口*/
class AbstractFactory
{
public:
    virtual~AbstractFactory(){}

    virtual ProductA* createProA()=0;
    virtual ProductB* createProB()=0;

};
/*每个具体工厂创建一个产品族，客户端使用这些工厂中的一个，因此它不必实例化产品对象*/
class ConcreateFactoryX: public AbstractFactory
{
public:
    ~ConcreateFactoryX(){}

    ProductA* createProA()override
    {
        return new ConcreteProductAX();
    }

    ProductB* createProB()override
    {
        return new ConcreteProductBX();
    }

};

class ConcreateFactoryY:public AbstractFactory
{
public:
    ~ConcreateFactoryY(){}

    ProductA* createProA()
    {
        return new ConcreteProductAY();
    }

    ProductB* createProB()override
    {
        return new ConcreteProductBY();
    }
};

//A\B两个产品族，都有X、Y两套接口，对应关系不能出错

int main()
{
    ConcreateFactoryX* factoryX=new ConcreateFactoryX();  //X接口产品的工厂
    ConcreateFactoryY* factoryY=new ConcreateFactoryY(); //Y接口产品的工厂
    
    ProductA *p1=factoryX->createProA();
    std::cout << "Product: " << p1->getName() << std::endl;

    ProductA *p2=factoryY->createProA();
    std::cout << "Product: " << p2->getName() << std::endl;

    ProductB *q1=factoryX->createProB();
    std::cout << "Product: " << q1->getName() << std::endl;

    ProductB *q2=factoryY->createProB();
    std::cout << "Product: " << q2->getName() << std::endl;

    delete q1;
    delete q2;




    delete p1;
    delete p2;

    delete factoryX;
    delete factoryY;
    
    
    
    return 0;
}