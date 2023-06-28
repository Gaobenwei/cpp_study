# 一：尽量const ,enum,inline 替换#define

```c++
//eg
#define AI 3.14 -----> const double AI = 3.14; 
//eg
class GamePlayer
{
private:
	static const int NumTurns = 5; 
	int scores[NumTurns]; 
}; 
//若有的编译器是类内声明，类外定义，使用如下更好：
class GamePlayer
{
private:
	enum{NumTurns = 5};
	int scores[NumTurns]; 
}; 
//eg
#define CLA_MAX(a, b)   f((a) > (b) ? (a) : (b))
//使用模板更好
template<typename T > 
inline void CLA_MAX(const T& a, const T& b)
{
	f(a > b) ? a : b); 
}
```

# 二.使用const

​		关键字出现在星号左边，表示被指物是常量；如果出现在星号右边，表示指针自身是常量；如果出现在星号两边，表示被指物和指针两者都是常量。

​		迭代器声明为const类似于声明指针为const一样：eg

```c++
//iter 的作用就像T* const，指向不可变，指向物可变
const std::vector<int>::iterator iter = vec.begin();  
```

​		const_iterator 的作用想 const int *，指向物不可变，指向可变。

​		两个成员函数只依靠常量性（const）不同是可以被重载的。为了避免代码重复，可是使用non-const成员函数调用const成员函数来实现,反之是错误。

​		mutable成员变量总是可以修改，即使是在const成员函数中。

# 三.初始化

​	尽量地使用构造函数初始化列表来初始化；如果某些时候需要减少多个构造函数中的重复初始值列表，可以处理那些（赋值和初始化效率差不多的变量），将他们的赋值操作移往某个函数（通常是private），供所有的构造函数调用。

​		初始值顺序应该和声明次序一致。

​		对于不同编译单元内定义的non-local static对象，初始化次序需注意：（一般c++对于不同编译单元内定义的non-local static对象，相对次序无明确正确顺序）

```c++
//eg
class FileSystem
{....}
extern FileSystem tfs; // 预备留给客户使用的某个对象
class Directory
{
	Directory(...)
	{
		disk = tfs.num();//此处指明tfs必须在之前初始化
	}
}
```

//创建用户对象

Directory tempDir(params);

解决方案：将每个non-local static对象搬到自己的专属函数内（该对象在函数内被声明为static）,函数返回一个指向所含对象的reference。这就保证了你在使用函数调用返回的reference将指向一个已经初始化了的对象。

改动：

```c++
//eg
class FileSystem
{....}
FileSystem& tfs()
{     
	//这个函数将替换原来的tfs对象，他在FileSystem class中可能是个static。
	static FileSystem fs; 
	//定义并返回一个local static 对象，返回一个reference。
	return fs; 
}
class Directory{}；//同以前
Directory::Directory(...)
{
	disk=tfs().num();//此处指明tfs()取代原版的tfs
}
// 创建用户对象
Directory& tempDIR()
{
	static Directory td; //这个函数将替换原来的tfs对象，他在FileSystem class中可能是个static。
	return td;//定义并返回一个local static 对象，返回一个reference。
}
```

***在程序的单线程启动阶段手工调用多有的reference-returning函数，消除多线程环境下问题（暂时不懂）。

# 四.默认构造，拷贝构造，拷贝赋值

编译器可能会自动为class生出三者，一般而言只有党生出的代码合法且有适当机会证明其有意义才会生出 operator=。

```c++
//eg:
template <class T>
class  NameObject
{
public:
	NameObject(string& name, const T& object);
private:
	string& nameValue;
	const T object;
};
string newdog("p1");
string olddog("p2");
NameObject<int > p(newdog, 2);
NameObject<int > q(olddog, 36);
p=s;//?会发生什么c
```

赋值前，各自指向string对象，想要赋值，p.nameValue有变动么，指向另一个的string对象，不可，引用不可变换指向。或者改变对象值，会影响其他指向同一对象的东西。-----》拒绝生成operator=。

面对内含const成员的class也一样，更改const成员不合法。若基类将拷贝赋值声明为私有，派生类无法调用，也会拒绝自动生成拷贝赋值。类似class的拷贝赋值需要自己定义。

# 5.拒绝的构造成员

为了驳回编译器自动的提供功能，将不想调用的成员函数声明为私有并且不予以实现，或者将阻止的动作放在基类的私有中，派生类也不会生成相关函数。  c++11 =delete方便。

# 6.为多态基类声明虚析构函数

作为多态性质的基类的对象应该声明虚析构函数。如果class带有任何虚函数，它应该拥有一个虚析构函数。应该避免继承没有虚析构函数的类。classes的目的如果不是为了作为多态的基类，最好不要声明虚析构。

当抽象一个基类，声明了纯虚析构函数时，必须为这个纯虚析构函数提供一份定义这样便于派生类调用析构函数时能够成功调用抽象基类的析构。

# 7.c++不喜欢析构函数抛出异常。

如果一个被析构函数调用的函数可能抛出异常，析构函数应该捕捉它，吞下该异常（不再传播）或者结束程序。

如果客户需要对某个操作运行期间抛出的异常处理，那么class应该提供一个普通的函数（而非析构）执行该操作。

# 8.绝对不要在构造和析构过程中调用虚函数。

因为在派生类构造、析构时候这一类的调用并不会调用派生类的具体化虚函数（未初始化的成员变量），而是会调用基类的虚函数。

优秀的做法是将此函数设置为非虚函数，在派生类构造、析构时传递相关信息给基类供其调用非虚函数。传递信息时利用一个static辅助函数创建值给基类比较好，以为是避免了未初始化的成员变量。

# 9.令赋值操作返回一个引用to *this。

# 10.在拷贝赋值运算符中处理自我赋值。

常用手段：1.比较来源对象和目标对象的地址。

2.安排语句顺序使得获得异常安全性（往往自动获得自我赋值安全性）。eg:记录源对象，令源对象指向其他，通过记录项删除原来。

3.copy and swap技术 eg:

```c++
class A
{
	...
	void swap(A& rhs); //交换rhs和*this的数据
	...
}
A& A::operator=(const A& rhs)
{
	A temp(rhs);//制作副本 另一种方案利用by value传参的方式制作副本
	swap(temp); //将*this与副本数据交换
	return* this; 
}
```

# 11.copying函数应该确保不漏掉成分

1.确保复制对象的所有成员 以及 所有 base class 的成分。例如派生类拷贝构造调用基类拷贝构造，派生类拷贝赋值调用基类拷贝赋值。

```c++
//eg:
class A：public base_class
{
	A& operator=(const A& rhs)
	{
		...
		base_class::operator=(rhs);//对于基类部分的赋值操作
		...
		return *this;		
	}
}
```

# 12.资源管理：以对象管理资源

将资源放进对象中，依赖析构函数的自动调用机制，确保资源会被释放。

使用RAII对象（资源取得时机便是初始化时机），它们在构造函数取得资源，在析构函数释放资源。例如使用智能指针指向资源。

复制RAII对象必须一并复制他所管理的资源，资源的copying行为决定了RAII对象的copying行为。常见的行为：抑制copying、施行引用计数法、深度拷贝底部资源，转移底部资源的拥有权。

在资源管路类中提供取得的原始资源的方法，一般有显示转换eg提供一个get（）函数，或隐式转换，如重载*、->或者重载类型转换。

※务必以独立的语句将newed的对象存储于智能指针内。如果不这样做，一旦异常抛出，是有可能造成资源泄露的。

eg：调用一个函数：A1(std::tr1::shared_ptr<Widget>(new widget), proc());//proc()函数返回一个int值。

在一个语句中，操作序列是弹性的，假设次序：①执行new widget,②调用proc(),③调用std::tr1::shared_ptr的构造函数，而在②抛出异常，那么在“资源被创建”和“资源被转换为资源管理对象”之间被干扰，资源泄露。

应该：

std::tr1::shared_ptr<Widget> pw(new widget)；

A1(pw, proc());

# 13.设计与声明：接口应该容易被正确使用，不易被误用

促进正确使用方法如：保持一致性、与内置类型行为兼容

避免误用方法如：建立（或包装）新类型eg限制客户导入的类型、限制类型上的操作，约束对象的值，消除客户管理资源的责任，例如直接将创建资源接口返回指向资源的智能指针，绑定该用的删除器。

std::shared_ptr支持定制删除器，可以预防cross-DLL problem，（对象在DLL中被new创建，却在另一个DLLD中被delete销毁）。

# 14.设计class犹如设计type一样，需要考虑诸多问题

# 15。尽量以pass-by-reference-to-const替换pass-by-value。

reference可以使得效率提高，并在继承体系中避免对象切割的问题，const 保护对象不会被修改（因为pass-by-value传递修改的也只是副本而已），pass-by-value 的方式传递对象，有可能使得派生类对象传递给参数列表中的基类对象参数时产生切割，无法得到派生类中的独特成分。

内置类型、STL的迭代器或函数对象，往往pass-by-value 更合适，需要自己判断切割问题和效率。

# 16.必须返回对象时，不要返回reference

不要返回reference或pointer指向一个local stack对象；

不要返回reference指向一个heap-allocated对象；

不要返回pointer或reference指向一个local static对象而有可能同时需要多个这样的对象。

# 17.宁可以non-member\non-friend替换member函数

可以增强封装性，包裹弹性，机能扩充性。

越少的代码可以看到class内private成分，封装性越好。

c++的做法是让类与一个non-member函数位于类所在的统一命名空间内。

```c++
//eg:
//一个头文件"webbrowser.h"  该头文件针对class WebBrowser自身以及他的核心功能
namespace WebBrowserStuff
{
	class WebBrowser { ..... };
	.......//核心技能，例如所有客户都需要的
	// non-member函数
}
//一个头文件"webbrowserbookmarks.h" 
namespace WebBrowserStuff
{	
	...//与书签相关的函数
}
//一个头文件"webbrowsercookies.h" 
namespace WebBrowserStuff
{
	...//与cookies相关的函数
}
```

c++标准库也是这样的方式。可以见到，将所有的便利函数放到多个头文件但是同一个namespace中易于扩充，需要做的只是添加更多的non-member\non-friend函数，客户只需要和一部分编译相依。

# 18.如果某个函数的所有参数（包括被this指针所指的那个隐喻参数）进行类型转换，那么这个函数必须是non-member。

※只有当参数被列于参数列内，这个参数才是隐式转换的合格参与者。

```c++
//eg:
class A
{
	A(int a);//构造函数没有特意设为explicit,允许隐式转换。
}
```

一.假设存在成员函数： constA A：：operator*（const A& rhs）const;

```c++
A result, a1, b1;
resule = a1 * 2;
//成功，理解为 result=a1.operator*(2);
result = 2 * a1;
//错误 ，理解为result=2.operator*(a1);  没有隐式转换2为A。
二。假设存在非成员函数
constA operator*（const A& lhs，const A& rhs）;
A result, a1, b1;
resule = a1 * 2;
result = 2 * a1;//都成功 ，能够隐式转换
```

# 19.写出一个不抛出异常的swap函数

当std::swap对你的自定义类型效率不高时，应该：

## 1.提供一个public swap成员函数，在其中高效的置换两个对象的值。

## 2.在你的class 或者template所在的namespace提供一个非成员swap函数，令它调用上述的swap成员函数。

```c++
//eg:
namespcae wid
{
	...
	template<typename T>
	class widget { ... };
	...
	template<typename T>
	void swap(widget<T>& a,(widget<T>& b)
	{
		a.swap(b);
	}
}
```

## 3.如果你正在编写一个class（而非class template），为你的class特化std::swap.令他调用你的成员swap函数。

```c++
//eg:
namespace std
{
	template<>
	void swap<widget>(widget& a, widget& b)
	{
		a.swap(b);	
	}
}
```

不能是class template因为：

1.以下不合法，c++只允许class template偏特化，不允许function template 偏特化。

```c++
namespace std
{
	template<typename T>
	void swap<widget<t>>(widget <T>& a, widget <T>& b)
	{
		a.swap(b);	
	}
}
```

2.std命名空间不允许添加新的templates,不可在std内部添加全新的，只能提供自定义类型的全特化版本。

```c++
namespace std
{
	template<typename T>
	void swap(widget <T>& a, widget <T>& b) //此处是函数重载版本，一个新的模板
	{
		a.swap(b);	
	}
}
```

4.在调用swap时，确保包含一个using声明式，以便于让std::swap可见，并在调用时不加任何namespace修饰符。

```c++
//eg:
void t1()
{
	...
	using std::swap;
	swap(...);
}
```

# 20.尽量延后变量定义式的出现时间

```c++
string a("hello");  //此方法不仅符合初始化优点，也符合本条款，尝试延后变量的定义直到能够给他初值实参为止。
string a;
a = "hello";
widget A；
for (...)              for (...)
{                      {
	w = ....;	        	widget w(..)
}	                   }
```

除非明确知道赋值成本比构造加析构低，用方法2.

# 21.尽量避免转型

试着发展无转型的替代设计。如果转型是必要的，试着将她隐藏在某个函数背后。客户可以调用函数，而不用接触转型。

const_cast<T>通常用来将对象的常量性转除。

dynamic_cast<T>主要用来执行“安全向下转型”，在继承体系中决定某个对象是哪个类型。

reinterpret_cast<T>意图执行低级转型，不可移植。

static_cast<T>用来强迫隐式转换。例如将non-const转换成const，int转double，可以用来执行上述多种转换的反向转换。

# 22.避免返回handles指向对象内部部分（变量及不被公开使用的函数）。

handles包括references，指针，迭代器，该条款可增加封装性，帮助const成员函数的行为更像是const，※成员变量的封装性最多只等于“返回其reference”的函数的访问级别，若const成员函数传出一个reference，后者所指的数据与对象自身有关且存储于对象外，则这个函数的调用者就可以修改数据。

该条款可以降低虚吊号码牌的可能性。eg,

const Point* puff=&(binde1(*pgo).uppfer);//假设uppfer返回一个handles,binde1()返回value,语句结束，该函数返回的对象副本删除，upper返回的handles就虚吊了。

# 23.异常安全性函数

异常安全满足两个条件：不泄露任何资源、不允许数据损坏。

异常安全性函数提供三个保障之一：

1.基本承诺：如果异常被抛出，程序内的任何事物仍然保持在有效状态下。（合法状态即可）

2.强烈保证：异常抛出，程序状态不改变。函数成功就是成功后的状态，失败就是返回调用前的状态。

3.不抛掷（nothrow）保证：承诺不抛出异常。※带有空白异常声明不代表nothrow。

eg:int do() throw();//不是不抛出异常，而是如果抛出异常，将是严重错误。

所有的这些性质都由函数的实现决定，无关声明。该提供哪一种，可能的话提供nothrow保证，但一般都落在基本承诺和强烈保证中。



实现方式：eg使用智能指针避免资源泄露，改变语序（如在某事发生后在改变对象状态）。

一个策略有助于强烈保证。copy and swap（为修改对象做副本，在副本上改，交换），通常实现可以将隶属于对象的数据放入另一对象中，然后赋予源对象一个指针指向新对象。函数影响非局部数据的话，难以形成强烈保证。

函数提供的异常安全性保证通常最高只等于所调用的各个函数的异常安全性的最低者。

# 24.理解inline函数 看条款30

inline函数通常被置于头文件中。一般对virtual函数的调用会使inlinling落空；想取某个函数的地址，编译器会为函数生成一个outlined函数本体；构造和析构往往是inlining的糟糕候选人。 80-20经验法则。

# 25.将文件之间的编译依存关系降低（有点懵）

一般构想：相依于声明式而非定义式。基于此构想两种手段 handle class 和Interface class（特殊的抽象基类：接口类）。

程序的头文件应该以完全且仅有声明式的形式存在，无论是否涉及templates都适用。

基于声明依存和定义依存分离的策略eg:

如果使用object reference或object pointer可以完成任务，不用objects。

尽量以class声明式替代class定义式。为声明式和定义式提供不同头文件。

Interface class（特殊的抽象基类：接口类）的客户通常用一个特殊函数将真正被具象化的派生类构造出来。一般返回指针指向动态分配的对象。该函数在Interface class中一般又被声明为static。

# 26.public继承意味着is-a.适用于base class身上的每个事情也一定适用于derived class 身上。

# 27.避免遮掩继承而来的名称

## 1.继承会带来名称遮掩。即使基类和派生类内的函数有不同的参数类型也适用，无论函数是否是virtual或non-virtual。eg:

```c++
class A
{
private:
	int x;
public:
	virtual void mf1() = 0;
	virtual void mf1(int);
	virtual void mf2();
	void mf3();
	void mf3(double);
	...
};

class Derived :public Base
{
public:
	virtual void mf1();
	void mf3();
	void mf4();
	...
};
```

base内所有名为mf1和mf3的函数都被derived class 内的mf1和mf3函数遮掩掉了。

```c++
Derived d;
int x;
d.mf1();  //对
d.mf1(x);  //错，派生类的mf1遮掩掉了基类的mf1
d.mf2();   //对
d.mf3();  //对
d.mf3(x);   //错
```

## 2.想要解除遮掩，可以使用using声明或者转交函数

```c++
class A
{
private:
	int x;
public:
	virtual void mf1() = 0;
	virtual void mf1(int);
	virtual void mf2();
	void mf3();
	void mf3(double);
};

class Derived :public Base
{
public:
	using Base::mf1;    //让Base class内名为mf1和mf3的所有
	using Base::mf3;    //都在Derived的作用域中可见
	virtual void mf1();
	void mf3();
	void mf4();
};

Derived d;
int x;
d.mf1();  //对
d.mf1(x);  //对，调用Base::mf1
d.mf2();   //对
d.mf3();  //对
d.mf3(x);   //现在没问题，调用Base::mf3

class A
{
public:
	virtual void mf1() = 0;
	virtual void mf1(int);
	virtual void mf2();
	void mf3();
	void mf3(double);
};

class Derived :private Base  //私有继承下不适用using声明，会使继承而来的所有同名函数在派生类中都可见
{
public:
	virtual void mf1() //转交函数
	{
		Base::mf1();  
	}
	...
};

Derived d;
int x;
d.mf1();  //对
d.mf1(x);  //错，私有继承下现在是被遮掩了
```

# 28.区分接口继承、实现继承

在public继承之下，派生类总是继承基类的接口。

纯虚函数：只具体指定接口继承；

非纯虚函数：具体制定接口继承及缺省的实现继承；

非虚函数：具体指定接口继承以及强制性实现继承。

一种安全的缺省实现的模式，eg

```c++
class Airplane
{
public:
	virtual void fly(const Airport& a) = 0; //纯虚，派生类务必重写
	...
};

void Airplane::fly(const Airport& a) //纯虚的实现，
{
	//缺省行为，即默认的方式
}

class A :public Airplane
{
public:
	virtual void fly(const Airport& a)
	{
		Airplane::fly(a); //缺省的调用
	}
};

class B :public Airplane
{
public:
	virtual void fly(const Airport& a)
	{
		//独特的实现，重写虚函数
	}
};
```

# 29.virtual函数的替代选择

## 1.使用non-virtual interface（NVI 非虚接口）手法，是以public non-virtual成员函数包裹访问性低的（protected\private)的virtual函数。

## 2.将virtual函数替换为“成员指针变量”，可在对象初始化时进行绑定。

## 3.以可调用对象成员变量（eg：function<int(int,int)>类）替换virtual函数，只要搭配一个兼容于需求的签名式。

## 4.传统的strategy设计模式手法，将继承体系内的virtual函数替换为另一个继承体系内的virtual函数。

将技能从成员函数转移到类外部，有一个缺点：非成员函数无法访问non-public成员。

# 30.绝对不要重新定义继承来的non-virtual函数

# 31.绝对不要重新定义继承而来的缺省参数值。

针对的是继承一个带有缺省参数值的virtual函数。

virtual函数是动态绑定的，而缺省参数却是静态绑定的。如果重新定义了继承而来的缺省参数值，通过指针或引用调用相关虚函数时，会混乱（虚函数是派生类重写的，但缺省参数是静态绑定，用的确实基类的）。

可以考虑用NVI（non-virtual interface）处理，带有缺省参数的非虚函数（public）调用无缺省参数的private虚函数，只需要重写虚函数。

# 32.复合塑模出 has-a 或者根据某物实现出

eg人有电话号码（应用域）；set通过一个list实现（实现域）。

```c++
template<typename T>
class set
{
public:
	bool member(const T& item)const;
	bool insert(const T& item);
private:
	std::list<T> rep;//用来表述set的数据
};
```

# 33.谨慎使用private继承

private继承意味着根据某物实现出。private继承意味着只有实现部分被继承，而接口部分应省略（基类中的public和protected在派生类中都会变为私有属性）。

## 1.如果classes之间继承关系是private，编译器不会自动将一个派生类对象转化为基类对象。

```c++
//eg
class person{...};
class student:private person{...};
void eat(const person& p);
void study(const student& s);

person p;
student s;
eat(p);//对，p是人，人会吃
eat(s);  //错，参数没有转换成功
```

## 2.private继承意味着根据某物实现出。它通常比复合的级别低。主要用于“当一个意欲成为derived class者想访问一个意欲成为base class者的protected成分，或是打算重新定义虚函数”，但是两者关系并非是is-a的关系。

和复合不同，私有继承可以造成empty class的最优化。

```c++
//eg
class Timer
{
public:
	explicit Timer(int tickF);
	virtual void onTick()const;  //函数意思：定时钟滴答一次，此函数调用一次
};
```

有一个类想要使用这种计数的功能。

```c++
//私有继承
class Widget :private Timer
{
private:
	virtual void onTick()const;//查看widget的数据..等
	...
};
```

或复合法

```c++
class Widget
{
private:
	class WidgetTimer :public Timer
	{
	public:
		virtual void onTick()const;
		...
	};
	...
};
```

# 34.谨慎使用多重继承

多重继承复杂，不建议使用。介绍一个正当的用途。涉及需要public继承接口类，而private继承某个协助实现的类。

```c++
//eg
class IPerson  //这个类指出需要实现的接口
{
public:
	virtual ~IPerson();
	virtual std::string name()const = 0;
	virtual std::string birthdate() = 0;
};
class Dateid(){...}  //稍后使用，不重要
class PersonInfo  //这个类有若干函数实现IPerson类接口
{
public:
	explicit PersonInfo(Dateid pid);
	virtual ~PersonInfo();
	virtual const char* theName() const;
	virtual const char* theBirthDate() const;
	virtual const char* valueDelimOpen() const;
	virtual const char* valueDelimClose() const;
	...
};
class Cperson :public IPerson, private PersonInfo
{
public:
	explicit Cperson(Dateid pid):PersonInfo(pid){}
	virtual std::string name()const //实现必要的IPerson成员函数
	{
		return PersonInfo::theName();
	}
	virtual std::string birthdate()const//实现必要的IPerson成员函数
	{
		return PersonInfo::theBirthDate();
	}
private:
	virtual const char* valueDelimOpen() const//重写继承而来的虚函数
	{
		return "";
	}
	virtual const char* valueDelimClose() const
	{
		return "";
	}
};
```

# 35.模板与泛型编程 隐式接口与编译期多态

classes与templates都支持接口以及多态。

classes接口是显示的，主要以函数签名（函数名称、参数、返回类型）为中心。多态是通过virtual函数发生在运行期（哪一个virtual函数应该被绑定）。

template参数而言，接口是隐式的，基于有效表达式。多态是通过template具现化和函数重载解析发生在编译期。

基于有效表达式：代码中的一组表达式对于template而言必须有效编译就是一组隐式接口。

# 36.typename指涉嵌套从属类型名称

任何时候当你想要在template中指涉一个嵌套从属类型名称，在紧邻它之前的位置上加关键字typename。

例外：typename不可以出现在基类列和成员初始化列表中。

```c++
//eg
template<typename T>
classs Derived:public Base<T>::Nested //base class list 中不许出现typename
{
public://成员初值列表 中不许出现typename
	explicit Derived(int x) :Base<T>::Nested(x)
	{//既不在base class list也不再成员初值列表，要加上typename
		typename Base<T>::Nested temp;
		...
	}

}
```

# 37.处理模板化基类内的名称

```c++
//eg
class Coma
{
public:
	...
	void sendClear(const string& msg);
	void sendEny(const string& msg);
};
class Comb
{
public:
	...
	void sendCleartext(const string& msg);
	void sendEnyyext(const string& msg);
};
class MsgInfo{...};
template<typename Company>
class MsgSender
{
public:
	...
	void sendClear(const MsgInfo& info)
	{
		string msg;
		Company c;
		c.sendCleaartext(msg);
	}
	void sendEnyd(const MsgInfo& info){...}//类似上面唯一不同调用sendEnyyext
};
```

在派生类中调用模板化基类内的函数要注意。

```c++
template<typename Company>
class Log :public MsgSender<Company>
{
public:
	...
	void sendClearMsg(const MsgInfo& info)
	{
		sendClear(info);//调用基类函数，无法编译通过，因为在编译class template Log时，
		//并不知道他继承什么样的class，有可能模板参数Company的某个特化版本并没有sendClear这个函数
	}
};
```

eg:

```c++
template<>
class MsgSender<Company>
{
public:
	...
	void sendSercet(...){...}
};
```

有三种方法使得我们令C++不进入模板化基类观察的行为失效：

1.this

```c++
template<typename Company>
class Log :public MsgSender<Company>
{
public:
	...
	void sendClearMsg(const MsgInfo& info)
	{
		this->sendClear(info);//在模板化基类的函数前加上指针调用
	}
};
```

2.using 声明

```c++
template<typename Company>
class Log :public MsgSender<Company>
{
public:
	using MsgSender<Company>::sendClear;//告诉编译器它位于哪里
	...
	void sendClearMsg(const MsgInfo& info)
	{
		sendClear(info);
	}
};
```

3.明确指出函数位于base class内：

```c++
template<typename Company>
class Log :public MsgSender<Company>
{
public:
	...
	void sendClearMsg(const MsgInfo& info)
	{
		MsgSender<Company>::sendClear(info)//但是者种行为在处理virtual函数是会关闭虚函数的绑定行为	
	}
};
```

※但是在编译通过后，实现具体化调用时，若模板化基类内的确没有相关函数，也会检测出。

# 38.将于参数无关的代码抽离templates.(懵懂)

# 39.运用成员函数模板接受所有兼容类型

问题提出：同一个template的不同具现体之间不存在与生俱来的固有关系，例如带有base-derived关系的B,D类型，以他们具现化的某个template不带有base-derived关系。以下的转化想法失败。

```c++
//eg
template<typename T>
class SmartPtr
{
pubcli:
	explicit SmartPtr(T* realPtr);//智能指针通常以内置原始指针初始化
	...
};
SmartPtr<Top> pt1 = SmartPtr<Middle>(new Middle);//欲将SmartPtr<Middle>转换为SmartPtr<Top>
SmartPtr<Top> pt2 = SmartPtr<Bottom>(new Bottom);//类似上
SmartPtr<const Top> pc2 = pt1;//SmartPtr<Top>转换为SmartPtr<const Top>
```

所以可能要编写合适的构造函数实现转型需要。构造模板提出：

```c++
template<typename T>
class SmartPtr
{
pubcli:
	template<typename U>                   //成员函数模板，为了生成copy构造
	SmartPtr(const SmartPtr<U>& other);
	...
};  //这一类构造函数根据对象U创建对象T，而U和T的类型是同一个template的不同具现体，成为泛化copy构造函数
```

也要约束一些转换行为：方法

```c++
template<typename T>
class SmartPtr
{
pubcli:
	template<typename U>
	SmartPtr(const SmartPtr<U>& other):heldPtr(other.get()){...} //以other的heldptr来初始化this的heldptr
	...
private:
	T* heldPtr; //这个smartptr持有内置的指针
};  //只有当存在某个隐式转换可将U*转换为一个T*才能通过编译，此构造函数只有在所获得的实参适当类型时才可以。
```

最后，如果你声明成员模板函数哟用于泛化copy构造和泛化copy 赋值，最终还需要声明正常的copy构造和copy赋值操作。

# 40.需要类型转换时请为模板定义非成员函数

## 一、非成员函数模板出错的例子

我们在条款24说过，对于Rational类来说，为了让所有的乘法操作都能通过操作，我们将operator*()函数定义为非成员函数版本（详情可以回过头再去看）。但是对于模板来说，这些规则可能不成立

例如下面我们把Rational和operator*()函数都定义为模板，代码如下：

```c++
//下面与条款24都相同，只是改为了模板
template<typename T>
class Rational {
public:
	Rational(const T& numerator = 0, const T& denominator = 1);
	const T numerator()const;
	const T denominator()const;
};

template<typename T>
const Rational<T> operator*(const Rational<T>& lhs, const Rational<T>& rhs)
{
}
```

现在我们书写下面的代码，不能编译通过。例如：

```c++
Rational<int> oneHalf(1, 2);

//在条款24中，Rational和operator*为非模板，此处代码可编译通过
//此处，Rational和operator*为模板，编译不通过
Rational<int> result = oneHalf * 2;
```

错误原因分析
错误的原因在于：上面的“oneHalf*2”没有找到匹配的operator*()函数，从而导致出错
原因：
1.oneHalf的类型为Rational<int>，因此其匹配到operator*()函数模板，并将operator*()函数模板的模板类型T实例化为int
2.但是当运行到第二个参数的时候，2为int，operator*()函数模板无法根据int推断出T的类型
3.因此最终编译器找不到合适的operator*()函数而导致出错
在非模板的operator*()中，operator*()可以将2隐式转换为Rational对象（Rational提供了隐式转换，详情再去看条款24）。但是在模板中，这种隐式转换不被允许
template实参推导取到决定性的因素

## 二、将模板函数声明为friend

在“一”中，我们使用的模板类和模板函数出错了，一种解决办法是将operator()*声明为Rational模板类的友元（friend）
下面我们进过两次修改，最终实现错误的代码

### 第一次修改（编译通过，但是链接时出错）

我们先将operator()*声明为Rational的友元。代码如下：
	template<typename T>
	class Rational {
	public:
		//友元，函数声明
		friend const Rational operator*(const Rational& lhs, const Rational& rhs);
	};

```c++
//函数定义
template<typename T>
const Rational<T> operator*(const Rational<T>& lhs, const Rational<T>& rhs);
```


```c++
int main()
{
	Rational<int> oneHalf(1, 2);
	//编译通过，但是链接时出错
	Rational<int> result = oneHalf * 2;

	return 0;
}
```

备注：上面friend生命中，我们没有在Rational后面添加<T>，这与Rational<T>是一样的，这两都可以
编译通过的原因：
当对象oneHalf被定义时，程序会生成一份Rational<int>类的定义，于是class Rational<int>被实例化出来
class Rational<int>被实例化之后，friend operator*()函数也被自动声明出来（备注：只是声明而没有定义）
friend operator*()函数声明出来之后，该函数为一个函数了，而不是一个模板了，因此我们可以将2传递给该函数（并且参数2可以隐式类型转换，将2转换为Rational）
于是上面的代码就可以编译通过了，但是链接时出错（下面介绍）
链接时出错的原因：
因为在class Rational<int>被实例化出来的时候，friend只是将operator*()声明了，但是没有定义，因此程序报错

### 第二次修改（正确版本）

上面的代码编译通过，但是链接通过，因为找不到operator*()的定义
解决链接出错的方法是：在friend函数的声明同时，定义这个函数。代码如下：
template<typename T>
class Rational {
public:
    //声明的时候，同时实现该函数（备注，该函数虽然在类中定义，但不属于成员函数）
    friend const Rational operator*(const Rational& lhs, const Rational& rhs)
    {
        return Rational(lhs.numerator()*rhs.numerator(),
        lhs.denominator()*lhs.denominator());
    }
};
此处friend的另一个好处：我们虽然将该函数声明为friend，但是却没有访问任何class的non-public成员，因此比较安全

## 三、令friend函数调用辅助函数（与inline相关）

我们曾在条款30中说过，如果函数定义在class中都会成为inline，因此上面我们定义在class中的friend operator()*也会称为inline
为了使inline带来的冲击最小化，我们可以：
我们可以在operator()*中什么事情都不做，将所完成的事情移动到另一个辅助函数中
这个辅助函数应该是一个template
代码如下：

为了使operator*()在类中的inline最优化，我们定义了一个辅助函数，在其中完成operator*()原本要完成的功能

doMultiply()不支持混合式操作（例如将Rational对象和一个int相乘），但是该函数只由operator*()调用，而operator*()支持混合操作，因此功能完善

//声明
template<typename T> class Rational

//函数声明(定义在下)
template<typename T>
const Rational<T> doMultiply(const Rational<T>& lhs, const Rational<T>& rhs);

template<typename T>
class Rational {
public:
    friend
        const Rational operator*(const Rational& lhs, const Rational& rhs)
    {
        //在其中调用doMultiply()，可使inline最优化
        return doMultiply(lhs, rhs);
    }
};

//完成原本operator*()要完成的功能
template<typename T>
const Rational<T> doMultiply(const Rational<T>& lhs, const Rational<T>& rhs)
{
    return Rational<T>(lhs.numerator()*rhs.numerator(),
        lhs.denominator()*lhs.denominator());
}

## 四、总结

当我们编写一个class template，而它所提供之“与此template相关的”函数支持“所有参数之隐式类型转换”时，请将那些函数定义为“class template内部的friend函数”

# 41.使用traits classes表示出类型信息

## 本节条款的题目：请使用trait classes来表示类型信息

本节条款主要讲述的技术是怎样在编译期间实现对迭代器类型的推断，依据推断的类型进行最优处理。
我们先来看一下迭代器的种类：
1.input_iterator：仅仅读，仅仅能逐个前移
2.output_iterator：仅仅写，仅仅能逐个前移
3.forward_iterator：可读可写，仅仅能逐个前移
4.bidirectional_iterator：可读可写，支持逐个前移和后移
5.random_access_iterator：可读可写，支持随机訪问（随意步数移动）
假设一下，假设有一个函数用于对迭代器指定移动次数d,我们怎样实现这个函数，以达到最理想的状态。
我们肯定会处理迭代器是random access类型的情况和不是random access类型的情况。当时random access的时候，直接加减迭代器在常数时间内完毕，假设不是，仅仅能一步一步向前或向后移动。

例如以下代码：

```c++
template<typename Iter, typename DistT>
void advance(IteT& iter, DistT d)
{
	if (iter is a random access iterator)
		iter += d;
	else
	{
		if (d >= 0)
			while (d--) ++iter;
		else
			while (d++) --iter;
	}
}
```

但是，我们怎么才干做到“iter is a random access iterator”在编译期间的实现？

实现这个功能。须要下面几步，而且相对繁杂的几步。

首先为每一种迭代器设置struct标签，每一个struct都是空的结构体，例如以下代码：

```c++
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator : public forward_iterator_tag {};
struct random_access_iterator : public bidirectional_iterator_tag {};
```

接着。我们就要引入traits,traits是一种技术，理解为萃取机，可以通过此技术问答出相关信息。
我们来看一下关于各种容器的定义：

```c++
template class <T>
class vector
{
public:
	class iterator
	{
	public:
		typedef random_access_iterator iterator_category;
		…
	}
	…
}
template class <T>
class list
{
public:
	class iterator
	{
	public:
		typedef bidirectional_iterator iterator_category;
		…
	}
	…
}
```

从上面代码能够看出，每一个容器中都有一个iterator类。但是每一个iterator类中的迭代器标签却不一样。
当我们调用语句`vector<int>::iterator::iterator_category`时会返回random_access_iterator类的类型,当我们调用`list<int>::iterator::iterator_category`时会返回bidirectional_iterator类的类型。
我们再来看一下有关iterator_traits结构体的定义，

```c++
template<typename IterT>
struct iterator_traits {
	typedef typename IterT::iterator_category iterator_category;//typedef typename的使用见条款42
};
```

假设我们通过语句`iterator_traits<vector<int>::iterator>::iterator_category`调用就会返回有关`vector<int>`迭代器iterator的类型，该语句返回random_access_iterator类型。
同理`iterator_traits<list<int>::iterator>::it`也能得到想要的类型信息。

为了支持指针迭代器，iterator_traits 特别针对类型提供一个偏特化版本：

template<typename IterT>
struct iterator_traits<IterT*>//针对内置指针
{
    typedef random_access_iterator_tag iterator_category;
    ...
};

现在可以知道如何设计并实现一个traits class了：

- 确认若干我们希望将来可取得的类型相关信息。例如对迭代器来说，就是可以取得其分类
- 为该信息选择一个名称（例如 `iterator_category`）
- 提供一个 template 和一组特化版本，内含你希望支持的类型和相关信息

现在有了 iterator_traits ，可以实现一下advance了,对于之前advance 中的伪代码，替换为：

template<typename IterT, typename DistT>
void advance(IterT& iter,DisT d)
{
    if(typeid(typename std::iterator_traits<IterT>::iterator_category) ==
       typeid(std::random_access_iterator_tag))
    ...
}
虽然逻辑是正确，但并非是我们想要的，抛开编译问题不说（见条款48），还有一个更根本的问题：IterT 类型在编译期得知，所以 iterator_traits::iterator_category 也在编译期确定。但是 if 语句却是在运行期核定。可以在编译期完成的事情却推到运行期，这不仅浪费时间，还造成执行文件膨胀。

**要在编译期确定，可以使用重载。重载是在编译期确定的，编译器会找到最匹配的函数来调用。**为了让 advance 的行为如我们所期望，我们需要做得是产生两版重载函数，内含 advance 的本质内容，但各自接受不同类型的 iterator_category 对象，这个函数取名为 doAdvance：

template<typename IterT, typename DisT>
void doAdvance(IterT& iter, Dist d, std::random_access_iterator_tag) {	//随机访问迭代器
    iter += d;
}

template<typename IterT, typename DisT>
void doAdvance(IterT& iter, Dist d, std::bidirectional_iterator_tag) {	//双向迭代器
    if(d >= 0)
        while(d--) ++iter;
    else 
        while(d++) --iter;
}

template<typename IterT, typename DisT>
void doAdvance(IterT& iter, Dist d, std::input_iterator_tag) {			//输入迭代器
    if(d < 0)
        throw std::out_of_range("Negative distance");
    while(d++) --iter;
}
//在advance中调用doAdvance
template<typename IterT,typename DistT>
void advance(IterT& iter,DistT d) {
    doAdvance(iter,d,typename std::iterator_traits<IterT>::iterator_category());
}
由于前向迭代器继承自输入迭代器，所以上述的输入迭代器版本也适用于前向迭代器。

现在来总结一下如何使用traits class

建立一组重载函数或函数模板（例如doAdvance），彼此间差异只在于各自的traits参数。令每个函数实现与其接受的traits信息匹配
建立一个控制函数或函数模板（例如advance），调用上面的函数并传递traits class的信息

※traits classes 使得类型相关信息在编译期可用，他们以template 或者template特化 完成实现。

整合函数重载技术，使得traits classes可能在编译期对类型执行if..else 测试。（将在运行期才能确定的信息转化为在编译期调用最合适的重载函数）。

# 42.template元编程

## 什么是模板元编程？模板元编程有什么好处？按照作者的原话就是：

1.TMP可将工作由运行期转移到编译期，因而得以实现早期错误侦测或者更高的执行效率。

2.TMP可被用来生成“基于政策选择组合”的客户定制代码，也可以用来避免生成对某些特殊类型并不适合的代码。

模板元编程，元编程本质上就是将运行期的代价转移到编译期，它利用template编译生成C++源码，举下面阶乘例子：

```c++
template <int N>
struct Factorial
{
	enum
	{
		value = N * Factorial<N - 1>::value
	};
};
template <>
struct Factorial<0>
{
	enum
	{
		value = 1
	};
};
int main()
{
	cout << Factorial<5>::value << endl;
}
```

在编译期，`Factorial<5>::value`就被翻译成了5 * 4 * 3 * 2 * 1，在运行期直接执行乘法即可。

如果我们对条款47掌握较好的话，我们就能再次感受模板元编程的优点，可以进行编译期间的类型识别和编译期间的函数分配（47中的例子是编译期间利用了函数重载）。
我们知道条款47也能通过typeid方式实现，可是typeid方式有的时候却会出现编译不通过的错误，如下代码：

```c++
template<typename Iter, typename DistT>
void advance(IteT& iter, DistT d)
{
	if (typeid(typename std::iterator_traits<IterT>::iterator_category)
		== typeid(std::random_access_iterator_tag))
		iter += d;
	else
	{
		if (d >= 0)
			while (d--) ++iter;
		else
			while (d++) --iter;
	}
}
```

如果我们调用语句`std::list<int>::iterator iter; advance(iter,10);`实例化advance函数，这时候实例化过程如下：

```c++
void advance(std::list<int>::iterator& iter, int d)
{
	if (typeid(typenamestd::iterator_traits<std::list<int>::iterator>::iterator_category)
		== typeid(std::random_access_iterator_tag))
		iter += d;//错误
	else
	{
		if (d >= 0)
			while (d--) ++iter;
		else
			while (d++) --iter;
	}c
}
```

上述的语句iter+=d;将无法通过编译，原因是list<int>::iterator的iterator_category类型是bidirectional的，而bidirectional不存在+=运算符的操作，所以编译不通过。虽然实际运行中if不会内不会执行，但是编译器却需要在编译期间对所有语句进行编译。

# 43.了解new-handler的行为

## 一、set_new_handler()函数

operator new无法满足某一内存分配需求时，在抛出异常前，会先调用一个客户指定的错误处理函数。new-handler。为了指定这个函数，客户调用set_new_handler。

- 当opertor new分配内存时:
  - 旧C++标准中，让operator new返回null
  - 现代C++标准中，operator new抛出异常（bad_alloc），并且该异常不能被operator new捕获，因此会被传播到内存索求处
- 我们可以在使用operator new之前，**使用set_new_handler()绑定一个错误处理函数**，这个错误处理函数将在opertor new分配内存时内存不足被调用执行（不断地调用）

### set_new_handler()函数的定义

- 该函数定义于<new>头文件中
- 源码如下，实际上是一个typedef

namespace std
{
	typedef void (*new_handler) ();
	new_handler set_new_handler(new_handler p) throw();
}

- 该函数接受一个new_handler参数（这个参数通常指向一个函数），并返回一个new_handler数据
- **throw()说明符：**表示该函数不会抛出异常。在C++11中已经被改为了noexcept

### 使用演示案例

- 下面是我们定义的一个函数，在operator new无法分配足够内存时被调用：

```c++
void outOfMem() 
{
    std::cerr << "Unable to satisfy request for memory\n";
    std::abort();
}
int main()
{
    //绑定函数
    std::set_new_handler(outOfMem);
    //如果此处内存分配失败，将会调用outOfMem()
    int *pBigDataArray = new int[100000000L];
    return 0;
}
```

## 二、new_handler函数的设计原则

- 当在内存分配不足时，就会调用set_new_handler()参数所指定的new_handler函数

- 如果内存不足时，new_handler函数被执行，但是我们同时又在new_handler函数中进行了动态内存分配，那么后果不可预期

- 因此，一个设计良好的new_handler函数有以下几项原则：

  ①让更多内存可被使用**：这种做法时，程序在一开始执行就分配一大块内存，而后当new_handler函数第一次被执行时，将它们释放给系统

  ②安装另一个new_handler：

  - 如果目前这个new_handler无法取得更多可用内存，或许它知道另外哪个new_handler有此能力
  - 目前这个new_handler就可以安装另外的new_handler来替换自己（只要调用set_new_handler）
  - 下次当operator new调用new_handler，调用的将是最新安装的那个（这个旋律的变奏之一是让new_handler修改自己的行为，于是当它下次被调用，就会做某些不同的事。为达到此目的，做法之一是令new_handler修改“会影响new_handler行为”的static数据、namespace数据或global数据）

  ③卸除new_handler**：也就是将null指针传给set_new_handler。一旦没有安装任何new_handler函数，operator new会在内存分配不成功时抛出异常

  ④抛出bad_alloc（或派生自bad_alloc）的异常**：这样的异常不会被operator new捕获，因此会被传播到内存索求处

  ⑤不返回：通常调用abort()或exit

## 三、为类设置new-handler行为

- 有时我们希望为每个类单独设置new-handler行为
- 例如不同的类分配操作失败就调用相对应的new-handler函数。**例如下面是伪代码：**

```c++
class X {
public:
	static void outOfMemory();
};
class Y {
public:
	static void outOfMemory();
};
int main()
{
	X* p1 = new X; //如果new失败了，我们希望调用X::outOfMemory()
	Y* p2 = new Y; //如果new失败了，我们希望调用Y::outOfMemory()
	return 0;
}
```

### 实现class的new-handler行为

- 默认情况下，C++不支持class专属的new-handlers行为，但是我们可以自己实现
- 实现方法为：
  - 为class设置operator new()内存分配函数，和set_new_handler()函数
  - 然后添加一个new_handler成员，代表本class在new失败时的调用函数

```c++
class Widget {
public:
	static std::new_handler set_new_handler(std::new_handler p)throw();
	static void* operator new(std::size_t size)throw(std::bad_alloc);
private:
	static std::new_handler currentHandler;
};
```

- 成员函数与成员变量的初始化：
  - currentHandler代表当前class的内存分配错误处理函数，初始化时将其设置为0
  - set_new_handler()用来设置当前的new_handler函数，原理与标准库set_new_handler()函数类似

```c++
std::new_handler Widget::currentHandler = 0; 
std::new_handler Widget::set_new_handler(std::new_handler p)throw() 
{
    std::new_handler oldHandler = currentHandler;
    currentHandler = p;
    return oldHandler;
}
```

- **针对于Widget::operator new()我们需要再定义一个class，使用RAII封装，**其在构造过程中获得一笔资源，在析构过程中释放。代码如下：

    class NewHandlerHolder 
    { 
    public:
        explicit NewHandlerHolder(std::new_handler nh)
        :handler(nh) {}
        ~NewHandlerHolder() {
            std::set_new_handler(handler);
        }
    private:
        std::new_handler handler;
        //阻止拷贝操作
        NewHandlerHolder(const NewHandlerHolder&);
        NewHandlerHolder& operator=(const NewHandlerHolder&);
    };

- **Widget::operator new()的实现如下：**

```c++
void* Widget::operator new(std::size_t size)throw(std::bad_alloc)
{
    //我们调用std::set_new_handler()，使其返回全局的new-handler
    //然后使用全局的new-handler初始化h
    NewHandlerHolder h(std::set_new_handler(currentHandler));
    //分配实际内存操作
    return ::operator new(size);
}
//不论分配是否成功，函数结束之后，h自动析构
//其析构函数中调用std::set_new_handler()，将之前的全局new-handler设置回来
```

- **客户端应该使用下面的格式使用Widget类：**

    void outOfMem(); 
    int main()
    {
        //设定outOfMem()为Widget的new-handler函数
        Widget::set_new_handler(outOfMem);
        //如果内存分配失败，调用outOfMem()
        Widget* pw1 = new Widget;
        //Widget::operator new()执行结束后还会将全局new-handler设置回来
        //如果内存分配失败，调用全局new-handler函数
        std::string* ps = new std::string;
        //设定Widget的new-handler函数为null
        Widget::set_new_handler(0);
        //如果内存分配失败，立刻抛出异常（因为Widget的new-handler函数置空了）
        Widget* pw2 = new Widget;
        return 0;
    }

## 四、封装一个new-handler行为的base class

- 在“三”中，我们最终设计出的Widget拥有自己的new-handler函数。借助Widget的实现原始，我们想设计一个类，让其只拥有new-handler的处理行为，然后让其他类继承于这个类，那么每个类就可以都拥有自己的new-handler行为了
- 为此：
  - 我们将Widget中的功能重新封装为一个单独的类，在其中实现new-handler行为
  - 为了使每种class拥有自己的new-handler行为，我们将该类设置为模板，然后让各自的class继承于这个模板
  - 当class在定义时，就会实例化模板，因此不同的类将会拥有各自的new-handler行为
- **代码如下：**    

    //定义为模板
    template<typename T>
    class NewHandlerSupport {
    //只有声明，定义未变，同上 
    public: 
        static std::new_handler set_new_handler(std::new_handler p)throw();
        static void* operator new(std::size_t size)throw(std::bad_alloc);
    private:
        static std::new_handler currentHandler;
    };
    //仍未非模板
    class NewHandlerHolder
    {
        //内容同上
    };

- 有了这个模板，**我们可以将自己的类继承于这个类，那么我们的类也将拥有自己的new-handler行为。**例如：

```c++
//继承NewHandlerSupport
class Widget :public NewHandlerSupport<Widget> {};
class Widget2 :public NewHandlerSupport<Widget2> {};
```

- 当我们不同的类继承于NewHandlerSupport之后，定义对象时，会分别实例化处不同的NewHandlerSupport模板，互相之间不为干扰
- 总结：
  - 上面的NewHandlerSupport模板，我们通常称之为“mixin”风格的base class
  - 继承于NewHandlerSupport，可能会导致多重继承带来的争议（可以参阅条款40）

## 五、nothrow说明

- 当opertor new分配内存时:
  - 旧C++标准中，让operator new返回null
  - 现代C++标准中，operator new抛出异常（bad_alloc），并且该异常不能被operator new捕获，因此会被传播到内存索求处
- **nothrow说明符：**为了与之前的C++标准中new操作符分配失败时返回null保持兼容，C++提供了这个关键字，使用了这个关键字能让operator new()在内存分配失败时返回null，而不抛出bad_alloc异常
- **例如：**

    class Widget {}; 
    int main()
    {
        //当new出错时，抛出bad_alloc异常
        Widget* pw1 = new Widget;
        //当new出错时，返回null
        Widget* pw2 = new (std::nothrow) Widget;  
        return 0;
    }

## 六、总结

- set_new_handler允许客户指定一个函数，在内存分配无法获得满足时被调用
- Nothrow new是一个颇为局限的工具，因为它只适用于内存分配；后继的构造函数调用还是可能会抛出异常

# 44.详看内存管理

## 了解new和delete的合理替换时机

## 编写new和delete需要守的规则

## placement new和placement delete
