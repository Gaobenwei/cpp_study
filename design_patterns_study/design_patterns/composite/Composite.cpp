#include<iostream>
#include<vector>
using namespace std;

/*组件
为组合中的所有对象(包括组合和叶节点)定义一个接口*/

class Component
{
    public:
        virtual ~Component(){}

        virtual Component* getChild(int )
        {
            return nullptr;
        }

        virtual void add(Component*){}
        virtual void remove(int){}

        virtual void operation()=0;
};

/*复合
定义具有子组件和存储子组件的组件的行为*/
class Composite:public Component
{
    public:
        ~Composite()
        {
            for(unsigned int i=0;i<children.size();++i)
            {
                delete children[i];
            }   
        }

        Component* getChild(const unsigned int index)
        {
            return children[index];
        }

        void add(Component* component)
        {
            children.push_back(component);
        }

        void remove(const unsigned int index)
        {
            Component *child=children[index];
            children.erase(children.begin()+index);
            delete child;
        }

        void operation()
        {
            for(unsigned int i=0;i<children.size();++i)
            {
                children[i]->operation();
            }
        }

    private:
        std::vector<Component*> children;
};


/*叶
定义组合中元素的行为，它没有子元素*/
class Leaf:public Component
{
    public:
        Leaf(const int i):id(i){}

        ~Leaf(){}

        void operation()
        {
            std::cout << "Leaf "<< id <<" operation" << std::endl;
        }
    private:
        int id;
};

int main()
{
    Composite composite;
    for(unsigned int i=0;i<5;++i)
    {
        composite.add(new Leaf(i));
    }
    
    composite.remove(1);
    composite.operation();
    return 0;
}