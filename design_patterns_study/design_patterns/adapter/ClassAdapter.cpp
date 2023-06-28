//不好，不建议使用

#include<iostream>

/*目标
定义客户端使用的特定接口*/
class Target
{
    public:
        virtual ~Target(){}
        virtual void request()=0;
};


/*Adaptee  旧的接口
所有请求都被委托给Adaptee，它定义了一个需要调整的现有接口*/

class Adaptee
{
    public:
        ~Adaptee(){}

        void specificRequest()
        {
            std::cout << "specific request" << std::endl;
        }
};


/*适配器
实现Target接口，并允许Adaptee通过扩展这两个类来响应Target上的请求，即使Adaptee的接口适应Target接口*/

class Adapter : public Target,private Adaptee
{
    public:
        virtual void request()
        {
            specificRequest();
        }
};

int main()
{
    Target *t=new Adapter();
    t->request();
    delete t;
    return 0;
}