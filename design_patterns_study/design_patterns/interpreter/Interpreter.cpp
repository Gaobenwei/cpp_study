//一个简单的表达式例子，eg 0&&1&&
#include<iostream>
#include<map>

/*包含解释器全局的信息*/
class Context
{
    public:
        void set(const std::string &var,const bool value)
        {
            vars.insert(std::pair<std::string,bool>(var,value));
        }

        bool get(const std::string& exp)
        {
            return vars[exp];
        }

    private:
        std::map<std::string,bool> vars;
};



/*抽象表达
声明抽象的explain操作，该操作对抽象语法树中的所有节点都是通用的*/

class AbstractExpression
{
    public:
        virtual ~AbstractExpression(){}

        virtual bool interpret(Context* const)
        {
            return false;
        }
};

/*终端表现
实现与语法中的终端符号相关联的explain操作(句子中的每个终端符号都需要一个实例)*/

class TerminalExpression:public AbstractExpression
{
    public:
        TerminalExpression(const std::string& val):value(val){}

        ~TerminalExpression(){}

        bool interpret(Context* const context)
        {
            return context->get(value);
        }
    private:
        std::string value;
};



/*非终结符表达式
为语法中的非终结符实现一个explain操作(语法中的每个规则都需要一个这样的类)*/

class NonterminalExpression : public AbstractExpression
{
    public:
        NonterminalExpression(AbstractExpression *left,AbstractExpression *right):
        lop(left),rop(right){}

        ~NonterminalExpression()
        {
            delete lop;
            delete rop;
        }

        bool interpret(Context *const context)
        {
            return lop->interpret(context) && rop->interpret(context);
        }


    private:
        AbstractExpression *lop;
        AbstractExpression *rop;
};


int main()
{
    AbstractExpression *A=new TerminalExpression("A");
    AbstractExpression *B=new TerminalExpression("B");
    AbstractExpression *exp=new NonterminalExpression(A,B);

    Context context;
    context.set("A",true);
    context.set("B",false);

    std::cout<<context.get("A")<<" AND "<<context.get("B");
    std::cout<<" = "<<exp->interpret(&context)<<std::endl;

    delete exp;
    return 0;
}
