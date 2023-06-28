#include<iostream>

/*接收机
知道如何执行与执行请求(行为)相关的操作*/

class Receiver
{
    public:
        void action()
        {
            std::cout<<"Receiver:excute action"<<std::endl;
        }
};

/*命令
声明所有命令的接口*/
class Command
{
    public:
        virtual ~Command(){}
        virtual void excute()=0;
    protected:
        Command(){}
};

/*具体的命令
通过调用Receiver上的相应操作来实现execute*/

class ConcreteCommand:public Command
{
    public:
        ConcreteCommand(Receiver* r):receiver(r){}

        ~ConcreteCommand()
        {
            if(receiver)
            {
                delete receiver;
            }
        }

        void excute()override
        {
            receiver->action();
        }
    private:
        Receiver* receiver;
};

/*调用程序
请求命令执行请求*/
class Invoker
{
    public:
        void set(Command* c)
        {
            command=c;
        }

        void confirm()
        {
            if(command)
            {
                command->excute();
            }
        }
    private:
        Command* command;
};

int main()
{
    ConcreteCommand command(new Receiver());

    Invoker invoker;
    invoker.set(&command);
    invoker.confirm();

    return 0;
}