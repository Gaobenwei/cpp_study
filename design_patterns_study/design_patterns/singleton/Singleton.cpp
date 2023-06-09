#include<iostream>

//单例

/*有私有静态变量来保存类和方法的一个实例，这给了我们一个实例化类的方法*/\

class Singleton
{
public:
    Singleton(const  Singleton & rhs)=delete;
    Singleton& operator=(const Singleton& )=delete;

    static Singleton* get()
    {
        if(!instance)
        {
            instance=new Singleton();
        }
        return instance;
    }

    static void restart()
    {
        if(instance)
        {
            delete instance;
        }
    }

    void tell()
    {
        std::cout<<"This is Singleton." << std::endl;
    }
private:
    Singleton(){}

    static Singleton* instance;    
};

Singleton* Singleton::instance=nullptr;

int main()
{
    Singleton ::get()->tell();
    Singleton::restart();
    return 0;
}