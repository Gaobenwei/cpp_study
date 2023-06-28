/*状态
定义了一个接口，用于封装与上下文的特定状态相关的行为*/

#include<iostream>

class State
{
    public:
        virtual ~State(){}
        virtual void handle()=0;
};

/*每个子类实现与上下文状态相关联的行为*/
class CConcreteStateA : public State{
    public:
        ~CConcreteStateA(){}
        void handle()
        {
            std::cout << "State A handled." << std::endl;
        }
};

class ConcreteStateB :public State
{
    public:
        ~ConcreteStateB(){}

        void handle()
        { 
            std::cout<< "State B handled." << std::endl;
        }
};

/*上下文
定义客户端感兴趣的接口*/
class Context
{
    public:
        Context():state(){}

        ~Context()
        {
            delete state;
        }

        void setState(State *const s)
        {
            if(state)
            {
                delete state;
            }
            state=s;
        }

        void request()
        {
            state->handle();
        }
    private:
        State *state;
};

int main()
{
    Context *context=new Context();
    context->setState(new CConcreteStateA());
    context->request();

    context->setState(new ConcreteStateB());
    context->request();

    delete context;
    return 0;
}