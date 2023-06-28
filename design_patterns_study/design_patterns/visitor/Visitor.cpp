#include<iostream>

//元素的类体系
class Element;
class ConcreteElementA;
class ConcreteElementB;

//行为
/*为对象结构中的每个ConcreteElement类声明一个Visit操作*/
class Visitor
{
    public:
        virtual ~Visitor(){}

        virtual void visitElementA(ConcreteElementA* const element)=0;  //为A添加操作
        virtual void visitElementB(ConcreteElementB* const element)=0;      //为B添加操作
};

/*具体的行为
实现Visitor声明的每个操作，这些操作实现了为结构中相应的对象类定义的算法片段*/
class ConcreteVisitor1:public Visitor     //添加操作1
{
    public:
        ~ConcreteVisitor1(){}

        void visitElementA(ConcreteElementA* const)  //重写为A添加操作的虚函数
        {
            std::cout << "Concrete Visitor 1: Element A visited." << std::endl;
        }

        void visitElementB(ConcreteElementB* const b)  //重写为B添加操作的虚函数
        {
            std::cout << "Concrete Visitor 1: Element B visited." << std::endl;
        }
};
 
class ConcreteVisitor2:public Visitor    //添加操作2
{
    public:
        ~ConcreteVisitor2(){}

        void visitElementA(ConcreteElementA* const)
        {
            std::cout << "Concrete Visitor 2: Element A visited." << std::endl;
        }

        void visitElementB(ConcreteElementB* const)
        {
            std::cout << "Concrete Visitor 2: Element B visited." << std::endl;
        }
};


/*元素
定义接受操作，该操作将访问者作为参数*/

class Element
{
    public:
        virtual ~Element(){}

        virtual void accept(Visitor &visitor)=0;
};

/*具体的元素
实现以访问者作为参数的accept操作*/

class ConcreteElementA:public Element{
    public:
        ~ConcreteElementA(){}

        void accept(Visitor &visitor)override   //接受添加操作的对象作为参数，来添加操作
        {
            visitor.visitElementA(this);
        }
};

class ConcreteElementB:public Element
{
    public:
        ~ConcreteElementB(){}

        void accept(Visitor &visitor)override
        {
            visitor.visitElementB(this);
        }
};


int main()
{
    ConcreteElementA elementA;
    ConcreteElementB elementB;   //类体系结构中的两个类

    ConcreteVisitor1 visitor1;
    ConcreteVisitor2 visitor2;     //两种要添加的操作

    elementA.accept(visitor1); 
    elementA.accept(visitor2);     //A添加两种操作

    elementB.accept(visitor1);
    elementB.accept(visitor2);
    return 0;
}