#include<iostream>

/*产品
产品实现相同的接口，以便类可以引用接口而不是具体的产品*/

class Product
{
public:
    virtual ~Product(){}
    virtual std::string getName()=0;
};

/*具体的产品
通过具体的工厂子类创建*/
class ConcreateProductA:public Product
{
public:
    ~ConcreateProductA(){}
    std::string getName()
  {
    return "type A";
  } 
};

class ConcreteProductB : public Product
{
public:
  ~ConcreteProductB() {}
  
  std::string getName()
  {
    return "type B";
  }
  // ...
};

/*创造者
包含除工厂方法外操作产品的所有方法的实现*/
class Creater
{
public:
    virtual Product* createee()=0;
    virtual Product* createProductA()=0;  //A产品的工厂子类方法
    virtual Product* createProductB()=0; //B产品的工厂子类方法

    virtual void removeProduct( Product *product ) = 0;
 };


/*eg*/
class Proc:public Product
{
    public:
    ~Proc(){}
    std::string getName()
  {
    return "type C";
  }
};

class ProCcreate:public Creater
{
public:
    ~ProCcreate(){}
    Product* createee()
    {
        return new Proc();    
    }
    void removeProduct( Product *product )
    {
        delete product;
    }

// //为了配合A,B产品 随便写的
    virtual Product* createProductA(){return nullptr;} 
    virtual Product* createProductB(){return nullptr;}

};



 /*具体的创造者
实现负责创建一个或多个具体产品的工厂方法。它是拥有如何创造产品的知识的阶级*/

class ConcreateCreator:public Creater
{
public:
    Product* createee(){return nullptr;}  //为了配合C产品 随便写的
    ~ConcreateCreator(){}

    Product* createProductA()
    {
        return new ConcreateProductA();
    }

    Product* createProductB()
  {
    return new ConcreteProductB();
  }
    void removeProduct(Product *product)override
    {
        delete product;
    }
};


int main()
{
    Creater *creator=new ConcreateCreator();  //某个具体的子类工厂

    Product* p1=creator->createProductA();
 std::cout << "Product: " << p1->getName() << std::endl;
  creator->removeProduct( p1 );

Product *p2 = creator->createProductB();
  std::cout << "Product: " << p2->getName() << std::endl;
  creator->removeProduct( p2 );

    Creater *cc3=new ProCcreate();
    Product* p3=cc3->createee();
    std::cout << "Product: " << p3->getName() << std::endl;
  creator->removeProduct( p3 );

delete creator;
delete cc3;
    return 0;
}