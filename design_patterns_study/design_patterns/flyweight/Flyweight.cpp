//简单理解相似对象，没有就创建，有的话就返回已有的

#include<iostream>
#include<map>
/*
轻量级选手
声明一个接口，flyweight可以通过该接口接收外部状态并对其进行操作
*/
class Flyweight
{
public:
    virtual ~Flyweight(){}
    virtual void operation()=0;
};

/*
不是所有的子类都需要共享
*/

class UnsharedConcreteFlyweight : public Flyweight
{
public:
    UnsharedConcreteFlyweight(const int stat):state(stat){}

    ~UnsharedConcreteFlyweight(){}

    void operation()
    {
        std::cout << "Unshared Flyweight with state " << state << std::endl;

    }

    private:
    int state;
};

/*
ConcreteFlyweight
实现Flyweight接口并为固有状态添加存储
*/

class ConcreteFlyweight :public Flyweight
{
public:
    ConcreteFlyweight(const int all_state):state(all_state){}
    ~ConcreteFlyweight(){}

    void operation()
    {
        std::cout << "Concrete Flyweight with state " << state << std::endl;
    }
    private:
    int state;
};


/*
FlyweightFactory
创建和管理轻量级对象，并确保适当地共享轻量级对象
*/
class FlyweightFactory
{
    public:
        ~FlyweightFactory()
        {
            for(auto it=files.begin();it!=files.end();it++)
            {
                delete it->second;
            }
            files.clear();
        }
        //共享关键 有返回，无创建
        Flyweight* getFlyweight(const int key)
        {
            if(files.find(key)!=files.end())
            {
                return files[key];
            }
            Flyweight *fly=new ConcreteFlyweight(key);
            files[key]=fly;
            return fly;
        }


    private:
        std::map<int,Flyweight*> files;
};

int main()
{
    FlyweightFactory *factory=new FlyweightFactory();
    factory->getFlyweight(1)->operation();
    factory->getFlyweight(2)->operation();
    delete factory;
    return 0;
}