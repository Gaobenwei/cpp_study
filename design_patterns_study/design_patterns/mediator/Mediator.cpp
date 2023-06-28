#include<iostream>
#include<vector>
#include<string>


class Mdeiator;
/*
同事类
每个同事在与其他同事通信时都与其调解员进行通信
*/
class Colleague
{   
    public:
        Colleague(Mdeiator *const m,const unsigned int i)
        :mediator(m),id{i}{}

        virtual ~Colleague(){}

        unsigned int getID()
        {
            return id;
        }

        virtual void send(std::string )=0;
        virtual void receive(std::string )=0;
    protected:
        Mdeiator *mediator;
        unsigned int id;
};

//具体的类
class ConcreteColleague: public Colleague{
public:
    ConcreteColleague(Mdeiator * const m,const unsigned int i):
    Colleague(m,i){}

    ~ConcreteColleague(){}

    void send(std::string msg);

    void receive(std::string msg)
    {
        std::cout << "Message '" << msg << "' received by Colleague " << id << std::endl;
    }

};

/*
*中介
定义了与同事对象通信的接口
*/

class Mdeiator
{
    public:
        virtual ~Mdeiator(){}

        virtual void add(Colleague *const c)=0;
        virtual void distribute(Colleague* const sender,std::string msg)=0;
    protected:
        Mdeiator(){}
};

/*
具体的中介
通过协调同事对象实现合作行为，并了解其同事
*/

class ConcreteMediator:public Mdeiator
{
    public:
        ~ConcreteMediator()
        {
            for(unsigned int i=0;i<colleagues.size();++i)
            {
                delete colleagues[i];
            }
            colleagues.clear();
        }

        void add(Colleague * const c)
        {
            colleagues.push_back(c);
        }

        void distribute(Colleague* const sender,std::string msg)override
        {
            for(unsigned int i=0;i<colleagues.size();i++)
            {
                if(colleagues.at(i)->getID()!=sender->getID())
                {
                    colleagues.at(i)->receive(msg);
                }
            }
        }
    protected:
    private:
        std::vector<Colleague *> colleagues;
};

void ConcreteColleague::send(std::string msg)
{
    std::cout << "Message '"<< msg << "' sent by Colleague " << id << std::endl;
  mediator->distribute( this, msg );
}

int main()
{
    Mdeiator * mediator=new ConcreteMediator();

    Colleague*c1=new ConcreteColleague(mediator,1);
    Colleague *c2 = new ConcreteColleague( mediator, 2 );
    Colleague *c3 = new ConcreteColleague( mediator, 3 );

    mediator->add(c1);
    mediator->add(c2);
    mediator->add(c3);

    c1->send("hi");
    c3->send("hello");

    delete mediator;
    return 0;
}