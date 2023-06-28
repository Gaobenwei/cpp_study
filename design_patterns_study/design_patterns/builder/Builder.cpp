#include<iostream>

#include<string>

/*
*产品
*使用Builder创建的最终对象
*/
class Product
{
public:
    void makeA(const std::string &part)
    {
        partA=part;
    }
    void makeB( const std::string &part )
  {
    partB = part;
  }
  void makeC( const std::string &part )
  {
    partC = part;
  }
  std::string get()
  {
    return (partA + " " + partB + " " + partC);
  }
private:
  std::string partA;
  std::string partB;
  std::string partC;    
};


/*
*建设者
*用于创建产品的抽象接口
*/
class Builder
{
public:
    virtual ~Builder(){}
    Product get()
    {
        return product;
    }

  virtual void buildPartA() = 0;
  virtual void buildPartB() = 0;
  virtual void buildPartC() = 0;
protected:
    Product product;    
};

/*创建真实的产品并将其存储在复合结构中*/
class ConcreteBuilderX : public Builder{
public:
    void buildPartA()override
    {
        product.makeA("A-X");
    }
    void buildPartB()
    {
        product.makeB( "B-X" );
    }
    void buildPartC()
    {
        product.makeC( "C-X" );
    }
};

class ConcreteBuilderY : public Builder
{
public:
  void buildPartA()
  {
    product.makeA( "A-Y" );
  }
  void buildPartB()
  {
    product.makeB( "B-Y" );
  }
  void buildPartC()
  {
    product.makeC( "C-Y" );
  }
  // ...
};

/*主管
负责管理对象创建的正确顺序*/

class Director
{
    private:
    Builder* builder;
public:
    Director():builder(){}
    ~Director()
    {
        if(builder)
        {
            delete builder;
        }
    }

    void set(Builder* b)
    {
        if(builder)
        {
            delete builder;
        }
        builder=b;
    }

    Product get()
    {
        return builder->get();
    }

    void construct()
    {
        builder->buildPartA();
        builder->buildPartB();
    builder->buildPartC();
    }
};


int main()
{
    Director director;
    director.set(new ConcreteBuilderX());
    director.construct();

    Product product1=director.get();
    std::cout << "1st product parts: " << product1.get() << std::endl;

    director.set(new ConcreteBuilderY());
    director.construct();

    Product product2=director.get();
    std::cout << "2nd product parts: " << product2.get() << std::endl;




    return 0;
}