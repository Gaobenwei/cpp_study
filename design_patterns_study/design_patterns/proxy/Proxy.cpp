#include<iostream>

/*定义了RealSubject和Proxy的公共接口，这样Proxy就可以在任何需要RealSubject的地方使用*/

class Subject
{
    public:
        virtual ~Subject(){}
        virtual void request()=0;

};


/*真正的主题
定义代理所代表的真实对象*/
class Realsubject : public Subject
{
    public:
        void request()override
        {
            std::cout << "Real Subject request" << std::endl;
        }
};


/*代理
维护一个允许代理访问真实主题的引用*/
class Proxy : public Subject
{
    public:
        Proxy()
        {
            subject=new Realsubject();
        }

        ~Proxy()
        {
            delete subject;
        }

        void request()override{
            subject->request();
        }
    private:
        Realsubject * subject;
};

int main()
{
    Proxy *proxy=new Proxy();
    proxy->request();
    delete proxy;
    return 0;
}