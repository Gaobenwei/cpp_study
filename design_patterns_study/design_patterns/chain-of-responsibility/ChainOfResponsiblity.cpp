#include<iostream>

/*处理程序
定义用于处理请求的接口，并可选地实现后续链接*/

class Handler
{
    public:
        virtual~Handler(){}

        virtual void setHandler(Handler *s)
        {
            successor=s;
        }

        virtual void handleRequest()
        {
            if(successor!=nullptr)
            {
                successor->handleRequest();
            }
        }
    private:
        Handler* successor;
};

/*具体的处理程序
处理他们负责的请求*/
class ConcreteHandler1: public Handler
{
    public:
        ~ConcreteHandler1(){}

        bool canHandler()
        {
            return false;
        }

        void handleRequest()override
        {
            if(canHandler())
            {
                std::cout << "Handled by Concrete Handler 1" << std::endl;
            }
            else
            {
                std::cout << "Cannot be handled by Handler 1" << std::endl;
                Handler::handleRequest();
            }
        }
    private:
};

class ConcreteHandler2:public Handler
{
    public:
        ~ConcreteHandler2(){}

        bool canHandler()
        {
            return true;
        }

        virtual void handleRequest()override
        {
            if(canHandler())
            {
                 std::cout << "Handled by Handler 2" << std::endl;
            }
            else
            {
                std::cout<<"Cannot be handled by Handler 2"<<std::endl;
                Handler::handleRequest();
            }
        }
};

int main()
{
    ConcreteHandler1 handler1;
    ConcreteHandler2 handler2;

    handler1.setHandler(&handler2);
    handler1.handleRequest();

    return 0;
}