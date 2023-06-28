

/*适配器
实现Target接口，当它获得方法调用时，它将调用委托给Adaptee*/
#include<iostream>

class Target
{
    public:
        virtual ~Target(){}

        virtual void request()=0;
};

class Adaptee
{
    public:
        void specificRequest()
        {
            std::cout << "specific request" << std::endl;
        }
};


class Adapter :public Target  //适配使得适用于接口类Target
{
    public:
        Adapter(Adaptee * t):adaptee(t){}
        ~Adapter(){
            delete adaptee;
        }

        void request()
        {
            adaptee->specificRequest();
        }
    private:
        Adaptee *adaptee;  //旧的接口

};

int main()
{
    Adaptee *p= new Adaptee();
    Target *t=new Adapter(p);
    t->request();
    delete t;
    return 0;
}