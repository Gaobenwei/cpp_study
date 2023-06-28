# 1.区分pointer和reference

当知道需要指向某个东西，而且绝对不改变指向其他东西，或者当你实现一个操作符而其语法无法有pointer完成，你应该使用reference，其他时候，请使用pointers。 

# 2.c++转型操作符

const_cast用于改变表达式中的常量性或变易性。

dynamic_cast继承体系中安全的向下转型或跨系转型动作。

reinterpret_cast常用用途是转换函数指针类型。

eg:

```c++
typedef void (*FuncPtr)();
FuncPtr funcPtrArray[10];
int dosometing();
funcPtrArray[0] = &dosometing;  //错误类型不符
funcPtrArray[0] = reinterpret_cast<FuncPtr>(&dosometing);  //可以通过编译
```

# 3.多态不要处理数组

数组对象会涉及指针的算术运算，多态（基类的指针或引用指向派生类）不适合和数组混用。eg基类的指针或引用操纵派生类数组往往不好。

# 4.非必要不提供默认构造函数

缺乏默认构造函数可能会带来一些问题，eg无法初始化空的数组，不适用于某些容器等；但任意添加默认构造函数也不可以。详见条款。

# 5.对于定制的类型转换函数 警觉

自定义类型有两种函数用于转换。单自变量的构造函数；类型转换操作符。

尽量不要提供隐式的类型转换，他可能在无意中被调用出错。

对于单自变量的构造函数，禁止隐式类型转换，可以叫explicit。或者使用另一模式：

```c++
template<typename T>
class Array
{
public:
	class ArraySize
	{
	public:
		ArraySize(int numele):theSize(numele){}
		int size()const { return thesize; }
	private:
		int theSize;
	};
	Array(int low, int high);
	Array(ArraySize size);
};

Array<int> a(10);//可以通过
bool operator==(const Array<int>& lhs, const Array<int>& rhs);
Array<int> a(10);
Array<int> b(10);
...
for (int i = 0; i < 10; ++i)
{
	if(a==b[i])... //错误，此时没有单一自变量，类型为int的构造函数
		//编译器不能考虑将int 转换为零时的ArraySize，再根据此对象产生Array对象
}
```

# 6.++\ --操作符的前后置式

利用一个额外的整型区分两者，还要注意返回式的区别，后置式操作符应该以前置式为实现基础eg:

```c++
class UPInt
{
public:
	UPInt& operator++();
	const UPInt operator++(int); //用不到的参数，定义式省略名称
	UPInt& operator--();
	const UPInt operator--(int);
};
UPInt& UPInt::operator++()
{
	*this += 1;
	return *this;
}
const UPInt UPInt::operator++(int)
{
	UPInt oldValue = *this;
	++(*this);
	return oldValue;
}
```

# 7.最好不要重载操作符&&  ||  ，  。

# 8.异常

## 8.1利用析构函数避免内存泄漏

使用原始指针指向创建的heap object对象时，需要在函数中自行释放资源（eg:delete），如果在中间过程中抛出异常，会导致资源泄露。可以使用资源管理类（eg：智能指针）指向资源，利用类的析构函数会被调用，在其中释放资源解决内存泄漏。

## 8.2在构造函数内阻止资源泄露

### 1.“C++ 只会析构已完成的对象”

“面对未完成的对象，C++ 拒绝调用其析构函数”，因为对于一个尚未构造完成的对象，构造函数不知道对象已经被构造到何种程度，也就无法析构。当然，并非不能采取某种机制使对象的数据成员附带某种指示，“指示constructor进行到何种程度，那么destructor就可以检查这些数据并(或许能够)理解应该如何应对。但这种机制无疑会降低constructor的效率，，处于效率与程序行为的取舍，C++ 并没有使用这种机制。所以说，”C++ 不自动清理那些’构造期间跑出exception‘的对象“。

### 2.异常发生时可能的泄露

当在构造函数中抛出异常的时候，由于对象尚未构造完全，因此并不会调用其析构函数，问题是此时如果对象已经被部分构造，那么我们应当保证被部分构造的内容适当地析构。

考虑这样一个类：

```c++
class A {
public:
	A(const int& number, const string& a, const string& b) :num(number), ptr_a(0), ptr_b(0) {
		if (a != "")
			ptr_a = new string(a);
		if (b != "")
			ptr_b = new string(b);
	}
	~A() { delete ptr_a; delete ptr_b; }
private:
	int num;
	string* ptr_a;
	string* ptr_b;
};
```

如果A的构造函数中在为ptr_b所指向的对象分配内存的时候抛出一个异常，正如1所说，编译器不会调用A的析构函数，那么释放ptr_a所指向的内存的任务就落到了程序员身上，可以将构造函数改写为以下形式来适当的释放ptr_a所指向的内存：

```c++
class A {
public:
	A(const int& number, const string& a, const string& b) :num(number), ptr_a(0), ptr_b(0) {
		try {
			if (a != "")
				ptr_a = new string(a);
			if (b != "")
				ptr_b = new string(b);
		}
		catch (...) {
			delete ptr_a; //可以直接delete，因为delete一个空指针不会有影响
			delete ptr_b; //同上
			throw;
		}
	}
	~A() { delete ptr_a; delete ptr_b; }
private:
	int num;
	string* ptr_a;
	string* ptr_b;
};
```

不需要担心A类的non-pointer data members(指的是num),data member会在A的构造函数内代码执行之前就被初始化好(因为使用了初始化列表)，一旦A类型对象被销毁，这些data member会像已经被构造好的对象一样被自动销毁。

你可以发现catch语句块内的动作与A类的析构函数动作相同， 因此可以把他们放入一个辅助函数内，如下：

```c++
class A {
public:
	A(const int& number, const string& a, const string& b) :num(number), ptr_a(0), ptr_b(0) {
		try {
			if (a != "")
				ptr_a = new string(a);
			if (b != "")
				ptr_b = new string(b);
		}
		catch (...) {
			cleanup();  //可以是一个private的函数
			throw;
		}
	}
	~A() { cleanup(); }
	void cleanup() {
		delete ptr_a;
		delete ptr_b;
	}
private:
	int num;
	string* ptr_a;
	string* ptr_b;
};
```

### 3.针对const指针的设计

以上代码可以比较好的解决构造函数内抛出异常时对象的析构问题，但是如果A类的ptr_a和ptr_b被声明为const，则需要另外一番考虑(此时必须在初始化列表内初始化ptr_a和ptr_b，const指针一旦初始化，不能被修改)，A的构造函数像以下这样定义：

```c++
class A {
public:
	A(const int& number, const string& a, const string& b) :num(number), ptr_a(a != "" ? new string(a) : 0), ptr_b(b != "" ? new string(b) : 0) {}

	~A() {
		delete ptr_a;
		delete ptr_b;
	}
private:
	int num;
	string* const ptr_a;//const指针
	string* const ptr_b;
};
```

但是很明显又会产生成员的析构问题，遗憾的是我们无法在初始化列表中使用try/catch语句来处理异常，因为try/catch是语句，而**初始化列表只接受表达式(或函数,其实可以把表达式看成的调用)**，这也是为什么代码里使用？：代替if语句。一个解决办法正如括号内所说，使用函数代替？：，在函数内完成异常处理的工作，代码如下：

```c++
class A {
public:
	A(const int& number, const string& a, const string& b) :num(number), ptr_a(initStringa(a)), ptr_b(initStringb(b)) {
	}
	~A() { cleanup(); }
private:
	void cleanup() { delete ptr_a; delete ptr_b; }
	string* initStringa(const string& a) {
		if (a != "") return new string(a);
		else return 0;
	} //ptr_a首先被初始化，所以即使失败也不需要担心内存泄露问题，所以不需要处理exceptioon
	string* initStringb(const string& b) {
		try {
			if (b != "") return new string(b);
			else return 0;
		}
		catch (...) {
			delete ptr_a;
			throw;
		}
	} //ptr_b第二个被初始化，如果它在被初始化的过程中有异常抛出，它必须保证将ptr_a所指资源释放掉
	int num;
	string* const ptr_a;
	string* const ptr_b;
};
```

到这里解决方法已经比较完美了，但还是有缺点，那就是构造函数的任务被分散在各个函数当中，造成我们维护上的困扰。

一个更好的办法是，接受条款9的忠告，使用标准库类模板auto_ptr，将ptr_a封装在auto_ptr之中，如下：

```c++
class A {
public:
	A(const int& number, const string& a, const string& b) :num(number), ptr_a(a != "" ? new string(a) : 0), ptr_b(b != "" ? new string(b) : 0) {
	}
private:
	int num;
	const auto_ptr<string> ptr_a;
	const auto_ptr<string> ptr_b;
};
//在此设计中，如果ptr_b在构造过程中有任何异常，由于ptr_a已经是构造好的对象，在运行其析构函数时，所指内存会被自动释放，此外，由于ptr_a和ptr_b如今都是对象，当当其”宿主“(A类对象)被销毁时，它们说所指内存也会被自动释放
```

### 4.auto_ptr类模板的使用目的

由2过程可知，auto_ptr类模板的使用可以在不增加代码量的情况下完美处理构造函数抛出异常的问题，auto_ptr类模板的设计目的正在于当指向动态分配的内存的指针本身停止活动(被销毁)时，所指内存被释放掉。

## 8.3阻止异常流出析构函数

两种情况下析构会被调用。第一种是当对象在状态下被销毁，也就是离开它的生存空间或是被明确删除；第二种情况是当对象被异常处理机制——也就是异常传播过程中的stack-unwinding（栈展开）机制——销毁。

在运行时期间从函数调用栈中删除函数实体，称为栈展开。栈展开通常用于异常处理。
在C++中，如果一个异常发生了，会线性的搜索函数调用栈，来寻找异常处理者，并且带有异常处理的函数之前的所有实体，都会从函数调用栈中删除。所以，如果异常没有在抛出它的函数中被处理，则会激活栈展开。

参考下面程序：

```c++
#include <iostream>
using namespace std;

// 函数f1会抛出一个异常
void f1() throw (int) {
	cout << "f1() Start " << endl;
	VehicleSurrogate va;
	throw 100;
	cout << "f1() End " << endl;
}

// 调用函数f1
void f2() throw (int) {
	cout << "f2() Start " << endl;;
	f1();
	cout << "f2() End " << endl;
}

// 调用函数f2,并处理f1()抛上来的异常
void f3() {
	cout << "f3() Start " << endl;
	try {
		f2();
	}
	catch (int i) {
		cout << "Caught Exception: " << i << endl;
	}
	cout << "f3() End" << endl;
}

// 演示栈展开过程的程序
int main() {
	f3();
	return 0;
}

//输出：
f3() Start
	f2() Start
	f1() Start
	VehicleSurrogate  constructor
	VehicleSurrogate  destructor
	Caught Exception : 100
	f3() End
```

上述程序中, 当函数f1抛出异常后，它的实体会从函数调用栈中删除掉(因为f1中没有包含异常处理代码)，然后下一个调用栈的实体用来查找异常处理者。
此例子中，下一个实体是f2()。因为f2中也没有包含异常处理代码，则它的实体也会从函数调用栈中移除。
再下一个实体是f3()。因为f3包含了异常处理，则f3中的catch代码块会被执行，最后catch代码块后面的代码也会执行。

注意：f1()与f2()中的如下面所示的代码完全没有执行。
//f1()中没有被执行的代码
cout<<"\n f1() End ";
//f2()中没有被执行的代码
cout<<"\n f2() End ";

另外需要注意的是：如果f1()和f2()中定义了一些局部对象，则在栈展开的过程中，这些局部对象的析构函数会被调用到。
这条规则只使用于栈上分配的对象，对于new出来的对象，则还是需要手动delete。
**如果控制权基于异常的因素离开析构，而此时正有另一个异常处于作用状态，C++会调用terminate函数，结束掉程序。**

示例程序：

```c++
Session::~Session()
{
	try {
		logDestruction(this);
	}
	catch (...) {} //什么都不做，阻止异常传出析构之外
}
```

这样的好处是，如果一个Session对象因为栈展开而被销毁，terminate并不会被调用。

禁止异常流出析构之外的第二个理由是，如果异常从析构内抛出，而没有在当地被捕捉，那么这个析构就执行不全，如果析构执行不全，就是没有完成它应该完成的每一件事。

因此，有两个好的理由支持我们全力阻止异常流出析构之外。第一，它可以避免terminate函数在异常传播过程的栈展开机制中被调用。第二，它可以协助确保析构完成其应该完成的所有事情。

## 8.4抛出异常与传递参数或调用虚函数的差异

相同点**传递函数参数与异常的途径可以是传值、传递引用或传递指针。**

不同点**但是当你传递参数和异常时，系统所要完成的操作过程则是完全不同的**

产生这个差异的原因是：**你调用函数时，程序的控制权最终还会返回到函数的调用处，但是当你抛出一个异常时，控制权永远不会回到抛出异常的地方**

### 1.异常对象总是会被复制

```c++
istream operator>>(istream& s, Widget& w);
void passAndThrowWidget()
{
	Widget localWidget;
	cin >> localWidget; //传递 localWidget 到 operator>> 
	throw localWidget; // 抛出 localWidget 异常 
}
```

当传递 localWidget 到函数 operator>>里，不用进行拷贝操作，而是把 operator>>内的引用类型变量 w 指向localWidget，任何对 w 的操作实际上都施加到localWidget 上。这与抛出 localWidget 异常有很大不同

不论通过传值捕获异常还是通过引用捕获都将进行 lcalWidget 的拷贝操作，也就说传递到catch子句中的是 localWidget 的拷贝。

原因：因为当 localWidget 离开了生存空间后，其析构函数将被调用。如果把 localWidget 本身（而不是它的拷贝）传递给 catch 子句，这个子句接收到的只是一个被析构了的Widget，一个 Widget 的“尸体”。这是无法使用的。因此C++规范要求被做为异常抛出的对象必须被复制。

**即使被抛出的对象不会被释放，也会进行拷贝操作**。例如如果 `passAndThrowWidget` 函数声明 `localWidget` 为静态变量（static）：

```c++
void passAndThrowWidget()
{
	static Widget localWidget; // 现在是静态变量（static）; 
	 //一直存在至程序结束 
	cin >> localWidget; // 像以前那样运行 
	throw localWidget; // 仍将对 localWidget进行拷贝操作
}
```

对异常对象进行强制复制拷贝，这个限制有助于我们理解参数传递与抛出异常的第二个差异：**抛出异常运行速度比参数传递要慢。**

当异常对象被拷贝时，拷贝操作是由对象的拷贝构造函数完成的。**该拷贝构造函数是对象的静态类型（static type）所对应类的拷贝构造函数，而不是对象的动态类型（dynamictype）对应类的拷贝构造函数**。比如以下这经过少许修改的 `passAndThrowWidget`：

```c++
class Widget { ... };
class SpecialWidget : public Widget { ... };
void passAndThrowWidget()
{
	SpecialWidget localSpecialWidget;
	...
		Widget& rw = localSpecialWidget; // rw 引用 SpecialWidget  throw rw; //它抛出一个类型为 Widget 的异常 
}
```

这里抛出的异常对象是 Widget，即使 rw 引用的是一个 SpecialWidget。因为 rw 的静态类型（static type）是Widget，而不是 SpecialWidget。你的编译器根本没有注意到 rw 引用的是一个SpecialWidget。编译器所注意的是 rw 的静态类型（static type）。这种行为可能与你所期待的不一样，但是这与在其他情况下 C++中拷贝构造函数的行为是一致的

**异常是其它对象的拷贝，这个事实影响到你如何在 `catch` 块中再抛出一个异常**。比如下面这两个 `catch` 块，乍一看好像一样：

```c++
catch (Widget& w) // 捕获 Widget 异常 
{
	... // 处理异常 
		throw; // 重新抛出异常，让它继续传递 
}
catch (Widget& w) // 捕获 Widget 异常 
{
	... // 处理异常 
		throw w; // 传递被捕获异常的拷贝
}
```

这两个catch块的差别在于第一个 catch 块中重新抛出的是当前捕获的异常，而第二个catch 块中重新抛出的是当前捕获异常的一个新的拷贝。如果忽略生成额外拷贝的系统开销，这两种方法还有差异么？

差异：
第一个块中重新抛出的是当前异常,无论它是什么类型。特别是如果这个异常开始就是做为 SpecialWidget 类型抛出的，那么第一个块中传递出去的还是 SpecialWidget 异常，即使 w 的静态类型（static type）是 Widget。这是因为重新抛出异常时没有进行拷贝操作
第二个 catch 块重新抛出的是新异常，类型总是 Widget，因为 w 的静态类型（static type）是 Widget

一般来说，你应该用 throw 来重新抛出当前的异常，因为这样不会改变被传递出去的异常类型，而且更有效率，因为不用生成一个新拷贝。

```c++
catch (Widget w) ... // 通过传值捕获异常 
	catch (Widget& w) ... // 通过传递引用捕获  异常  
	catch (const Widget& w) ... //通过传递指向 const 的引用捕获异常
```

可以注意到传递参数与传递异常的另一个差异：一个被异常抛出的对象可以通过普通的引用捕获；它不需要通过指向 const 对象的引用捕获。在函数调用中不允许转递一个临时对象到一个非 const 引用类型的参数里，但是在异常中却被允许。

当我们这样声明一个 catch 子句时：

catch (Widget w) ... // 通过传值捕获
会建立两个被抛出对象的拷贝，一个是所有异常都必须建立的临时对象，第二个是把临时对象拷贝进 w 中。
而下面这样声明：

catch (Widget& w) ... // 通过引用捕获 
catch (const Widget& w) ... //也通过引用捕获
这仍旧会建立一个被抛出对象的拷贝：拷贝同样是一个临时对象。相反当我们通过引用传递函数参数时，没有进行对象拷贝。当抛出一个异常时，系统构造的（以后会析构掉）被抛出对象的拷贝数比以相同对象做为参数传递给函数时构造的拷贝数要多一个。

### 2.函数调用者或抛出异常者与被调用者或异常捕获者之间的类型匹配的过程不同

比如在标准数学库中的sqrt 函数：

double sqrt(double); // from <cmath> or <math.h>
我们能这样计算一个整数的平方根，如下所示：

int i;
double sqrtOfi = sqrt(i);
//C++允许进行从 int 到 double 的隐式类型转换，所以在 sqrt 的调用中，i 被悄悄地转变为 double 类型，并且其返回值也是 double。
一般来说，catch 子句匹配异常类型时不会进行这样的转换

```c++
	void f(int value)
	{
		try
		{
			if (someFunction()) // 如果 someFunction()返回真 
			{
				throw value; //抛出一个整形值 
				...
			｝
		}
		catch (double d) // 只处理 double 类型的异常 
		{
			...
		}
		...
	}
```

在 try 块中抛出的 `int` 异常不会被处理 `double` 异常的 `catch` 子句捕获。**该子句只能捕获类型真真正正为 `double` 的异常，不进行类型转换。因此如果要想捕获 `int` 异常，必须使用带有 `int` 或 `int&`参数的 `catch` 子句**

不过在 catch 子句中进行异常匹配时可以进行两种类型转换：
第一种是**继承类与基类间的转换**。一个用来捕获基类的 catch 子句也可以处理派生类类型的异常，比如：捕获 runtime_errors 异常的 Catch 子句可以捕获 range_error 类型和 overflow_error类型的异常；可以接收根类 exception 异常的 catch 子句能捕获其任意派生类异常。所谓的继承架构中的exception转换规则可以适用于by value，by reference, by pointer三种形式。

第二种是**允许从一个类型化指针转变成无类型指针**），所以带有 const void* 指针的 catch 子句能捕获任何类型的指针类型异常：

### 3.传递参数和传递异常间**最后一点差别是 `catch` 子句匹配顺序总是取决于它们在程序中出现的顺序。**

因此一个派生类异常可能被处理其基类异常的 `catch` 子句捕获，即使同时存在有能直接处理该派生类异常的 `catch` 子句，与相同的 `try` 块相对应。例如：

```c++
try
{
	...
}
catch (logic_error& ex) // 这个 catch 块 将捕获所有的 
//logic_error异常, 包括它的派生类 
{
	...
}

catch (invalid_argument& ex) { // 这个块永远不会被执行 
	... //因为所有的 
} // invalid_argument 
 // 异常 都被上面的 
 // catch 子句捕获。
```

与上面这种行为相反，**当你调用一个虚拟函数时，被调用的函数位于与发出函数调用的对象的动态类型最相近的类里。你可以这样说虚拟函数采用最优适合法，而异常处理采用的是最先适合法**。
如果一个处理派生类异常的 `catch` 子句位于处理基类异常
的 `catch` 子句后面，编译器会发出警告。

## 8.5以by reference的方式捕获异常

## 8.6慎重的使用异常规格说明

### 1 异常限定符与unexpected调用

如下面的代码所示，标识符throw()即为异常限定符。异常限定符标识了函数可以抛出的异常类型。当throw后面的括号内容为空，表示该函数不抛出任何异常。

```c++
class Exception {
	public:
		const char* what() throw() const; // 该函数不抛出任何异常
		void doSomething() throw(int, Widget); // 该函数只抛出int和Widget类型的异常
}
```

throw()限定符以文档的形式，规定了函数可以抛出异常类型的范围。但是，多数编译器允许throw()修饰的函数，调用没有throw()修饰的函数。如下面的例子所示。

```c++
void func0() {
	// may throw some exception;
}
void func1() throw() {
	func0();
}
```

在运行时期，若func0()抛出异常，则特殊函数unexpected会被调用，该函数调用terminate函数，terminate默认行为会调用abort。abort停止程序运行，而程序中的局部变量无法被销毁。

### 2 避免策略

编译器允许带有“异常限定符”的函数调用没有“异常限定符”的函数，这种做法极具弹性，但会带来程序被迫终止，内存泄漏等问题。下面将会讨论如何避免unexpected函数被调用。

#### 1 不要用“异常限定符throw()”修饰带有模板参数的函数

如下面代码。对于判断“==”的操作不会出现异常。但是我们无法确定，取地址操作符“&”是否已经被重载，且可能抛出异常。

此种情况的实质是，我们无法确定，所有类对象的同名函数都不会抛出异常。

```c++
template<class T>
bool operator==(const T& left, const T& right) throw() {
	return &left == &right;
}
```

#### 2 外层函数不使用throw()进行修饰

若被调用的内层函数B没有throw()修饰，则外层的调用函数A也不要有throw()修饰——我们无法确定函数B的的确确不会抛出异常。

我们常常会忽略的一种情况是：注册“回调函数”。如下面代码所示，如果注册的“回调函数”没有throw修饰，而调用“回调函数”的外层函数却有throw修饰，“回调函数”抛出异常就会引起程序终止。

```c++
typedef void (*CallbackPtr)();
class Callback {
	public:
		Callback(CallbackPtr func) : m_func(func) {}
		void doSomething() throw() {
			m_func(); // 可能抛出异常
		}
	private:
		CallbackPtr m_func;
}
```

由于较新的编译器支持typedef后加入throw进行修饰。因此我们可以定义如下函数指针类型。但是，有可能CallbackPtr所指向的函数依然会抛出异常。因此最好还是采用本节最开始的主张——如果不确定内层函数是否会抛出异常，那么外层函数也不要用throw限制。

```c++
typedef void (*CallbackPtr)() throw();
void func0();
void func1() throw();
Callback object0(func0); // 错误，func0没有throw修饰
Callback object1(func1);  // 正确，func1有throw修饰
```

#### 3 自定义unexpected函数

上面提到当throw()修饰的函数抛出异常，则系统会自动调用函数unexpected()，unexpected()函数会调用terminate()，terminate()继续调用abort()，进而终止函数程序。

新的思路是，当出现问题时，抛出自定义函数，而不是调用unexpected()函数。

C++提供了函数set_unexpected()，向该函数中传递我们自定义的函数，来替换默认的unexpected()。如下面的例子所示。自定义函数抛出我们自定义的异常，从而进一步处理。

```c++
class UnexpectedException { //所有非预期的exception objects都将被取代为此类的object
}
void unExpected() {  //如果有一个非预期的exception被抛出，就调用此函数
	throw UnexpectedException();
}
set_unexpected(unExpected);
```

这样，未知异常就变成了已知异常，方便捕获和进一步处理。

### 3 异常的成本

若在编译过程中加入异常，就会需要额外的数据结构来记录try…catch结构。而代码、运行速度也会整体下降5%-10%。

同时，若异常没有妥善处理，则会造成程序崩溃，退出等情况发生。因此，使用异常的原则是：如果能使用参数传递、返回值，就尽量减少对异常的使用。异常只是出现在很少的一部份内。

# 9.效率

## 9.1了解80-20法则，和缓释评估的技术。

lazy evaluation实际上是"拖延战术":延缓运算直到运算结果被需要为止。如果运算结果一直不被需要,运算也就不被执行,从而提高了效率.所谓的运算结果不被执行,有时指只有部分运算结果被需要,那么采用拖延战术,便可避免另一部分不被需要的运算,从而提高效率,以下是lazy evaluation的四种用途.

### 1.Referenounting(引用计数)

 如果要自己实现一个string类,那么对于以下代码:ce C

String s1="Hello";
String s2=s1;//调用String的copy constructor
最直接的是采用eager evalutation(急式评估):copy constructor为s1做一个副本并放入s2内（new operator分配堆内存；调用strcpy将s1的数据复制到s2所分配的内存中）。尽管此时s2尚未被使用.

采用lazy evaluation的思想,可以先让s2分享s1的值,这样就省去了"调用new"以及"复制任何东西"的高昂成本.唯一要做的是一些簿记工作,以记录共享同一内容的各个对象.对s2的任何读操作,只需要s1的值即可,然而,一旦需要对s2的值进行写操作,就不能再做任何拖延,必须为s2做一份真实副本并进行写操作.数据共享所引起唯一危机是在其中某一字符串被修改时发生的，若修改s2，则s1不应被修改，此时就必须做一个s1的副本给s2.
这种"数据共享"的观念就是lazy evaluation:在真正需要之前,不为对象构造副本.在某些应用领域,可能永远也不需要提供那样一份副本,从而提高效率.

### 2.区分读和写

读写的功能不同，实现和代价也不同，要判断operator[]用于读操作还是写操作几乎是不可能的是,但如果利用lazy evaluation和条款30所描述的proxy classes,便可以延缓决定"究竟是读还是写",知道确定其答案.

### 3.Lazy Fetching(缓式取出)

对于程序需要使用内含许多字段的大型对象,比如:

```c++
class LargeObject {
public:
	LargeObject(ObjectID id);
	const string& field1()const;
	int field2()const;
	double field3()const;
	const string& field4()const;
	const string& field5()const;
	...
}
```

那么对于从磁盘中回复一个LargeObject对象,如果要取出此对象的所有字段,数据库相关操作成本可能极高,尤其是如果这些数据需要从远程数据库跨越网络而来.但如果只需要该对象的某个或某几个字段,那么读取所有数据的操作其实是不必要的.
采用Lazy evaluation的思想,在产生一个LargeObject对象时,可以只产生该对象的"外壳",而不从磁盘读取任何数据.只有当该对象的某个字段被需要时,才从数据库中取回对应的数据,以下做法可以实现这种"demand-page"式的对象初始化行为:

```c++
class LargeObject{
public:
    LargeObject(ObjectID id);
    const string&field1()const;
    int field2()const;
    double field3()const;
    const string& field4()const;
    const string& field5()const;
    ...
private:
    ObjectID oid;
    mutable string *field1Vaule;//注意使用了mutable修饰符，即使在const成员函数内，也可以修改该数据成员
    mutable int *field2Value;
    mutable double *field3Value;
    mutable string *field4Value;
    ...
}
LargeObject::LargeObject(Object  id):oid(id),field1Value(0),field2Value(0),field3Value(3)...{}
const string& LargeObject::field1()const{
    if(field1Value==0){
        read the data from field 1 from the database and make field1Vaule point to it;
    }
    return *field1Value;
}
```

对象内的每个字段都是指针,指向必要的数据,NULL初始值表示该字段未被读入,需要先从数据库读入.将字段指针声明为mutable,可以保证字段指针可以在任何时候都能被更改以指向实际数据,即使是在const成员函数内也一样.

### 4.Lazy Expression Evaluation(表达式缓评估)

```c++
template<class T>
class Matrix { ... }
Matrix<int> m1(1000, 1000);
Matrix<int> m2(1000, 1000);
...
	Matrix<int>m3 = m1 + m2;
```

对于operator+,通常的做法是eager ecaluation:计算并返回m1和m2的和,这是一个大规模运算,并需要大量内存分配成本.

    采用lazy evaluation的思想,可以先设一个数据结构于m3中,用于标记m3是m1和m2的总和,这个数据结构可能只由两个指针和一个enum组成,前者指向m1和m2,后者用来表示运算动作.假设在m3被使用之前,程序又执行以下动作:

Matrix m4(1000,1000);
...
m3=m4*m1;
那么便可直接将m3定位为m4和m1的乘积,之前没有用到的矩阵加法操作实际上并没有进行.当然,对m1和m2加和却没有用到的情况比较夸张,但不排除维护过程中程序员更改代码使得m1+m2不被用到的请况出现.


当然,lazy evaluation在此处还有更大用法——只计算大型运算中需要的部分运算结果.

```c++
对于以下代码:
cout<<m3[4];
 
此时不能再使用拖延战术,但也只需要计算m3第四行的值,除此以外,不需要计算其他任何值.实际上,正是这种策略使得APL(20世纪60年代的一款如软件,允许用户以交谈方式使用软件执行矩阵运算)能够快速处理加法,减法,乘法甚至除法.
```


当然,有时lazy evaluation并不能起作用,比如如下操作:

cout<<m3;
    或者:

m3=m1+m2;
m1=m4;
这时就要采取某些措施以确保对m1的改变不会影响m3的值,可以在对m1进行改变之前先对m3求解,也可以将m1的旧值复制一份,然后令m3依从该值等,其他可能会修改矩阵值的情况,也要采取类似措施.

  此外,由于存储数值间的相依关系,必须维护一些数据结构一存取数值,相依关系等,此外还必须将赋值,复制,加法等操作符进行重载,因此lazy evaluation用于数值运算领域也有许多工作要做,但与节省的时间和空间相比可能是微不足道的.

## 9.2分摊计算成本，over-eager evaluation 超急评估

超前进度地做"要求以外"的更多工作,也就是在要求之前就把事情做下去.例如,以下class template用于表现数值数据的大型收集中心:

```c++
template<class NumericalType>
class DataCollection {
public:
	NumericalType min()const;
	NumericalType max()const;
	NumericalType avg()const;
	...
}
```

随时记录程序集执行过程中的最小,最大,平均值,一旦min,max,avg常被调用,便可以分期(逐次)摊还"随时记录数据群之最大,最小,平均值"的成本,而每次调用所需付出的(摊还后)成本,将比eager evaluation和lazy evaluation低.

 Over-eager的背后观念是,如果预期程序常常会用到某个计算,可以降低每次计算的平均成本,方法就是设计一份数据结构以便能够极有效率地处理需求.

其中一种设计方法就是将"已经计算好而有可能再次被需要"的数值保留下来(所谓caching),下面是findCubicNumber的一种设计方法,使用STL的map容器作为局部缓存:

```c++
int findCubicNumber(const string& employeeName) {
	//map存储(employee,cubicle number)数据对,作为局部缓存(local cache)
	typedef map<string, int> CubicleMap;
	static CubicleMap cubes;
	Cubic::iterator it = cubes.find(employeeName);
	if (it == cubes.end()) {
		int cubicle = the result of looking up employeeName's cubicle number in the database;
			cubes[employeeName] = cubicle;
		return cubicle;
	}
	else
		return it->second;
}
```

 此策略将昂贵的"数据库查询动作"以相对低廉的"内存内数据结构产找动作"取代,从而降低"返回一个职员的房间号码"的平均成本.

 另一种做法是Prefetching(预先取出).例如stl中的vector每次需要扩充容量时两倍扩充，预先为更多的索引提供“容量”。

Over-eager evaluation实际上是一种"以空间换时间"的策略:Caching和Fetching都需要额外空间来(待)存放数据以降低访问所需时间.少数情况下,对象变大会降低软件性能:换页(paging)活动可能会增加,缓存击中率(cache hit rate)可能会降低.

  over-eager evaluation和条款17的lazy evaluation并不矛盾:当必须支持某些运算但运算结果并不总是需要的时候,lazy evaluation可以改善程序效率;当必须支持某些运算且其结果总是被需要,或常常被需要的时候,over-eager evaluation可以改善程序效率.两者都比直接了当的eager evaluation难实现,但都可以为程序带来巨大的性能提升.

## 9.3了解临时对象的来源

匿名对象通常发生于两种状况：

### 1.当隐式类型转换被施行以求函数调用可以成功。

eg:

```c++
// 返回ch在str中出现的次数
size_t countChar(const string& str, char ch);
char buffer[MAX_STRING_LEN];
char c;
// 读入到一个字符和字符串中，用setw
// 避免缓存溢出，当读取一个字符串时
cin >> c >> setw(MAX_STRING_LEN) >> buffer;
cout << "There are " << countChar(buffer, c)
  << " occurrences of the character " << c
  << " in " << buffer << endl;
```

看一下countChar的调用。第一个被传送的参数是字符数组，但是对应函数的正被绑定的参数的类型是const string&。通过以buffer做为参数调用string的构造函数来初始化这个临时对象。countChar的参数str被绑定在这个临时的string对象上。当countChar返回时，临时对象自动释放。

仅当通过传值（by value）方式传递对象或传递常量引用（reference-to-`const``）参数时，才会发生`这些类型转换。当传递一个非常量引用（reference-to-non-`const``）`参数对象，就不会发生。考虑一下这个函数：

void uppercasify(string& str); // 把str中所有的字符改变成大写

在字符计数的例子里，能够成功传递char数组到countChar中，但是在这里试图用char数组调用upeercasify函数，则不会成功：

char subtleBookPlug[] = "Effective C++"; 

uppercasify(subtleBookPlug); // 错误!没有为使调用成功而建立临时对象，为什么呢？

假设建立一个临时对象，那么临时对象将被传递到upeercasify中，其会修改这个临时对象，把它的字符改成大写。但是对`subtleBookPlug`函数调用的真正参数`没有任何影响；仅仅改变了临时从subtleBookPlug生成的string对象。无疑这不是程序员所希望的。程序员传递subtleBookPlug参数到uppercasify函数中，期望修改subtleBookPlug的值。当程序员期望修改非临时对象时，对非常量引用（`references-to-non-`const``）进行的隐式类型转换却修改临时对象。这就是为什么C++语言禁止为非常量引用（`reference-to-non-`const``）产生临时对象。这样非常量引用（`reference-to-non-`const``）参数就不会遇到这种问题。`reference-to-const不会产生问题，因为const导致无法改变。

### 2.第二种环境是函数返回对象时

eg:

const Number operator+(const Number& lhs,const Number& rhs);

这个函数的返回值是临时的，因为它没有被命名；它只是函数的返回值。你必须为每次调用operator+构造和释放这个对象而付出代价。通常你不想付出这样的开销。

## 9.4完成返回值优化

首先，如果函数一定要以by value的方式返回对象，无法消除。应该返回constructor argument以取代对象。

eg:

```c++
inline const Rational operator*(const Rational& lhs,const Rational& rhs)
{
	return Rational(lhs.numerator() * rhs.numerator(),
		lhs.denominator() * rhs.denominator()); 
}
Rational a=12;
Rational b(1,2);
Rational c=a*b;  //编译器会优化operator*内的临时对象以及被operator*返回的临时对象。
```

## 9.5可以通过函数重载技术避免隐式类型转换使得匿名对象减少。

## 9.6考虑以复合形式的操作符（eg+=）取代简单操作符形式。

以复合形式的操作符重载为基础实现简单形式的。

## 9.7理解虚函数、多重继承、虚基类和 RTTI （运行时期类型辨识）所需的代价

# 10.构造函数、非成员函数虚化

## 1 构造函数的虚化

构造函数与类的名称相同，因此，此处所指的虚化，并非让构造函数变成虚函数，而是让其具有类似虚函数的行为：

> 当条件不同时，不同的对象会被创建出来。

```c++
class Base {
}
class Derive1 : public Base {
}
class Derive2: public Base {
}

Base* constructObject(std::istream& inStream) {
	Base* obj;
	if (/*condition0*/) {
		// create Base's object
	} else if (/*condition1*/) {
		// create Derive1's object;
	} else {
		// create Derive2's object
	}
	return obj;
}
```

## 2 “虚拷贝构造函数”

所谓虚拷贝构造函数可能会返回一个指针，指向其调用者的一个新副本。

如下代码所示。C++11中，虚函数允许返回类型不同（返回类型，抛出异常属性兼容即可），因此，Base, Derive1, Derive2三个类型的clone可以看作是override。这样，每次调用clone函数，可以根据相应的对象，来进行复制。

```c++
class Base {
	public:
		virtual Base* clone() const;
}
class Derive1 : public Base {
	public:
		virtual Derive1* clone() const;
}
class Derive2 : public Base {
	public:
		virtual Derive2* clone() const;
}
void func()
{
	std::vector<Base*> objList, targetList;
	for (int idx = 0; idx < objList.size(); ++idx) {
		targetList.push_back(objList[idx]->clone());  // 关键代码
	}
}
//clone()通常直接调用函数的“复制构造函数”。代码如下：
Base* Base::clone()
{
	return new Base(*this);
}
Derive1* Derive1::clone()
{
	return new Derive1(*this);
}
... 
```

总结：通过增加一层虚函数clone，我们实现了，根据不同类型来进行复制构造的操作，从而实现多态。

## 3.非成员函数的虚化

非成员函数无法被真正的虚化，但是应该可以让其行为视其参数的动态类型不同而不同。

做法eg：写一个虚函数执行实际的工作，再写一个非虚函数单纯调用虚函数即可。

```c++
class Base{
public:
    virtual ostream& print(ostream& s)const=0;
    ...
};
class Text:public Base{
public:
    virtual ostream& print(ostream& s)const;
    ...
};
class Text2:public Base{
public:
    virtual ostream& print(ostream& s)const;
    ...
};
//非虚函数
inline ostream& operator<<(ostream& s,const Base & T)
{
    return T.print();
}
```

# 11.限制某个class能够产生的对象数量

## 1.允许0个或一个对象   策略1限制构造函数的调用

0个对象简单操作：将构造函数等放入private无法产生。

要限制对象数目,最直接的策略是限制构造函数的调用(以下称**策略1**),采用这种思想将构造函数设为private,然后声明一个友元函数调用它,并生成一个static对象,像这样:

```c++
class PrintJob;
class Printer {
public:
    void submitJob(const PrintJob& job);
    void reset();
    void performSelfTest();
    ...
    friend Printer& thePrinter();//友元函数
private:
    Printer();//默认构造函数
    Printer(const Printer& rhs);//copy constructor
    ...
};
Printer& thePrinter(){
    static Printer p; //静态局部变量，保证了只有一个
    return p;
}
```

当要使用Printer对象时,就调用thePrinter,它返回Printer的引用且保证只产生一个Printer对象,除了将thePrinter声明为friend,还可以使它成为Printer类的static成员,像这样:

```c++
class Printer {
public:
    static Printer& thePrinter(); //改变
    ...
private:
    Printer();
    Printer(const Printer& rhs);
    ...
};
Printer& Printer::thePrinter(){
    static Printer p;
    return p;
}
```

 **thePrinter的实现使用了函数的static对象,与class的static对象相比,函数中的static对象保证在第一次调用该函数时才被初始化,这可以避免由于"C++没有规定定义于不同编译单元内的全局对象的初始化顺序"而导致的错误.**

##  2另外一种策略(以下称策略2)是当外界申请太多对象时,在构造函数内抛出异常:

```c++
class Printer {
public:
    class TooManyObjects{};    //需要时跑出的异常类
    Printer();
    ~Printer();
    ...
private:
    static size_t numObjects;
    Printer(const Printer& rhs);//由于只允许产生一个对象,所以不允许拷贝
};
size_t Printer::numObjects = 0;
Printer::Printer(){
    if (numObjects >= 1) {
        throw TooManyObjects();
    }
    proceed with normal construction here;
    ++numObjects;
}
Printer::~Printer(){
    perform normal destruction here;
    --numObjects;
}
```

策略2很容易被一般化,而且可以使对象数目限制在除1以外的值,但在**涉及到类的继承以及内含的情况**下不起作用,因为"Printer对象可于三种不同状态下生存:(1)它自己,(2)派生物的'base class'成分,(3) 内嵌于较大对象之中",例如:

```c++
//涉及继承
//彩色打印机类
class ColorPrinter: public Printer {
    ...
};
Printer p;
ColorPrinter cp; //抛出异常
//涉及内含
class CPFMachine { 
private:
    Printer p; 
    FaxMachine f; 
    CopyMachine c;
    ...
};
CPFMachine m1; 
CPFMachine m2; //抛出异常
```

策略1不存在这个问题,因为将构造函数设为private实际上禁止了继承和内含的发生.由此启发,要禁止类被继承,可以将构造函数设为private,然后通过开放的接口提供对象引用或指针.

##  3将两种策略结合,使其既有策略1的禁止被继承和内含的特点,又具有策略2的一般化特性,像这样:

限制构造函数的调用，提供了伪构造函数的接口。

```c++
class Printer {
public:
    class TooManyObjects{};
    static Printer * makePrinter();  //伪构造函数，实际作用类似于构造函数
    static Printer * makePrinter(const Printer& rhs);
    ...
private:
    static size_t numObjects;
    static const size_t maxObjects = 10; // 对象的数目限制,也可以使用枚举
    Printer();
    Printer(const Printer& rhs);
};
size_t Printer::numObjects = 0;  //class static的类外义务性定义
const size_t Printer::maxObjects;//同上
Printer::Printer(){
    if (numObjects >= maxObjects) {
        throw TooManyObjects();
    }
    ...
}
Printer::Printer(const Printer& rhs){
    if (numObjects >= maxObjects) {
        throw TooManyObjects();
    }
     ...
}
Printer * Printer::makePrinter(){ 
    return new Printer;   //如果担心忘记delete掉分配的内存，可以使用智能指针保存返回的指针，在使用结束释放智能指针会自动调                           //用delete
}
Printer * Printer::makePrinter(const Printer& rhs){ 
    return new Printer(rhs); 
}
```

## 4一个用来计算对象个数的Base Class

可以将2中的策略1和策略2的结合一般化,将它抽象为一个类模板,任何需要限制对象数目的类只要继承这个模板的实例化即可:

```c++
template<class BeingCounted>
class Counted {
public:
    class TooManyObjects{}; 
    static int objectCount() { return numObjects; }
protected:
    Counted();
    Counted(const Counted& rhs);
    ~Counted() { --numObjects; }
private:
    static int numObjects;
    static const size_t maxObjects;
    void init(); //初始化函数
}; 
template<class BeingCounted>
Counted<BeingCounted>::Counted(){ 
    init(); 
}
template<class BeingCounted>
Counted<BeingCounted>::Counted(const Counted<BeingCounted>&){ 
    init(); 
}
template<class BeingCounted>
void Counted<BeingCounted>::init(){
    if (numObjects >= maxObjects) throw TooManyObjects()
        ++numObjects;
}
```

将Counted定义为模板,同一继承层次中的不同类共享同一对象计数,因此通过使用类模板,不同派生类的对象计数得以相互独立.

  Printer要使用Counted就像这样:

```c++
class Printer: private Counted<Printer> {  //注意私有继承
public:
    static Printer * makePrinter(); //伪构造函数
    static Printer * makePrinter(const Printer& rhs);
    ~Printer();
    void submitJob(const PrintJob& job);
    void reset();
    void performSelfTest();
    ...
using Counted<Printer>::objectCount; //开放某些接口使得可见
using Counted<Printer>::TooManyObjects;
private:
    Printer();
    Printer(const Printer& rhs);
};
```

**由于Counted<Printer>是Printer的基类,因此构造Printer之前一定会发生Counted<Printer>的构造,这样就把限制对象个数的任务交由基类Counted<Printer>来完成.此外,由于Printer只继承Counted的*实现而不继承接口*,因此使用private继承.如果要使开放Counted的部分接口,可以使用using declaration:**

最后一个需要注意的地方是**,static成员的定义问题,**由于Counted类模板使用了static成员,因此必须要在类外定义,对于numObjects可以在头文件中初始化为0,但**用于限制对象个数的maxObjects**只能由用户定义并初始化,如果用户忘记定义,链接时将会报错并提醒。

# 12.要求（或禁止）对象产生于heap之中

## 1.要求对象产生于heap之中

一个好的方法是让析构函数成为private，构造函数仍然是public，导入一个伪的析构函数调用析构，客户可以调用伪析构解决问题。

```c++
class UPNumber {
public:
    UPNumber();
    UPNumber(int initValue);
    UPNumber(double initValue);
    UPNumber(const UPNumber& rhs);//copy constructor
    // pseudo(伪的) destructor,它是const member function,因为const对象也需要被销毁（const成员函数存在的意义在于它能被const对象调用）
    void destroy() const { delete this; }
    ...
private:
    ~UPNumber();
};

//对UPNumber的使用像这样:
UPNumber n; //错误（虽然合法，但n的析构函数稍后被隐式调用，就不合法了）
UPNumber *p = new UPNumber; //正确
...
delete p; // 错误! 试图调用private 析构函数
p->destroy();//正确

//通过限制costructor或destructor的使用便可以阻止non-heap对象的产生,但同时也阻止了继承和内含:
class UPNumber { ... }; //将析构函数或构造函数声明为private
//继承
class NonNegativeUPNumber: public UPNumber { ... }; // 错误! destructor或constructor不能通过编译
//内含
class Asset {
private:
    UPNumber value;
    ... // 错误! destructor或constructor不能通过编译
};
```

对于继承,解决方法就是将UPNumber的构造函数或析构函数声明为protected。对于内含,解决方法则是将内含UPNmuber对象改为内含一个指向UPNumber对象的指针,像这样:

```c++
class UPNumber { ... }; // 声明析构函数为protected（构造函数仍为public）
class NonNegativeUPNumber:public UPNumber { ... }; // 现在可以通过编译，derived class可以取用protected member
class Asset {
public:
    Asset(int initValue);
    ~Asset();
    ...
private:
    UPNumber *value;//指向基类的指针
};
Asset::Asset(int initValue):value(new UPNumber(initValue)){ ... }
Asset::~Asset(){ value->destroy(); }
```

## 2.判断某个对象是否位于heap内

  1中提出的方法依然不能解决在继承情况下基类可能位于non-heap的问题,用户可以产生一个non-heap NonNegativeUPNumber对象：

NonNegativeUPNumber n;//没问题

但UPNumber却无法阻止,实际上它甚至无法知道自己是否是作为某个heap-based object的base class部分而产生,也就没有办法检测以下状态有什么不同:

```c++
NonNegativeUPNumber *n1 = new NonNegativeUPNumber; // 在heap内
NonNegativeUPNumber n2;//不在heap内
```

###  策略1:

利用"new 操作符调用operator new且我们可以对operator new进行重载"的特点(稍后会解释这种方法有硬伤),像这样:

```c++
class UPNumber {
public:
// 如果产生一个非堆对象，就抛出异常
    class HeapConstraintViolation {};
    static void * operator new(size_t size);
    UPNumber();
    ...
private:
    static bool onTheHeap; //标志对象是否被构造于堆上
    ... 
};
// 类外部定义静态成员
bool UPNumber::onTheHeap = false;
void *UPNumber::operator new(size_t size)
{
    onTheHeap = true;//此为新增动作
    return ::operator new(size);//此为原本就该有的动作
}
UPNumber::UPNumber()
{
    if (!onTheHeap) {
        throw HeapConstraintViolation();
    }
    proceed with normal construction here;
    onTheHeap = false;//清除flag，供下一个对象使用
}
```

 这种方法对于产生单个对象的确可行:用户如果通过new来产生对象,onTheHeap就会在operator new中被设为true,构造函数被正常调用,如果对象不是产生于堆中,onTheHeap就为false,调用构造函数时就会抛 出类型为HeapConstraintViolation的异常.但对于数组的产生却存在硬伤,对于以下代码:

```c++
UPNumber *numberArray=new UPNumber[100];
```

即使对operator new[]也进行了重载,使它具有和之前operator new类似的动作,但是由于调用new[]时,内存一次性分配而构造函数多次调用,因此operator new[]只能将onTheHeap在第一次调用构造函数时设为true,以后将不再调用operator new[],onTheHeap也只能为false,也就是说,为数组中的元素第二次调用构造函数时就会抛出异常.此外,即使没有数组,这种设计也可能会失败,对于以下操作:

```cpp
UPNumber *pn = new UPNumber(*new UPNumber);//会造成资源泄露,但是先不考虑这个问题。
//某些编译器产生出来的函数调用顺序是这个样子:"
//1.为第一个对象调用operator new
//2.为第二个对象调用operator new
//3.为第一个对象调用constructor
//4.为第二个对象调用constructor
```

### 策略2

"利用许多系统都有的一个事实:程序的地址空间以现行序列组织而成,其中stack(栈)高地址往低地址成长,heap(堆)往低地址成长"像这样:

```cpp
bool onHeap(const void *address){//函数参数address
    char onTheStack; // 局部栈变量
    return address < &onTheStack;
}
```

这种方法无疑不具有移植性,因为有的系统是这样,有的系统却不是这样,此外,static对象(包括global scope和namespace scope)既不是位于stack也不是位于heap中,它的位置视系统而定,可能位于heap之下,在这种情况下,策略2无法区分heap对象和static对象!

**区分heap和stack的目的通常是为了判断对一个指针使用delete是否安全,幸运的是,实现后者比实现前者更容易,因为对象是否位于heap内和指针是否可以被delete并不完全等价,对于以下代码:**

```cpp
struct Asset{
    int a;
    UPNumber value;
    ...
}
Asset* a(new Asset);
UPNumber* ptr=&(a->value);
```

 尽管ptr指向的是heap内存,但对ptr实行delete会出错,原因在于a是通过new取得,但它的成员——value并不是通过new取得.从这里可以看出**,对一个指针使用delete是否安全并不取决于对象是否位于heap中,而是取决于它是否是通过new获得**(其实本质上取决于它是否位于申请的一段heap内存的开始处,但这里不讨论).

### 策略3(用于判断对指针delete是否安全):

这里采用了较朴素的方法,将由动态分配而来的地址加入到一个表中,isSafeToDelete负责查找特定地址是否在表中,从而判断delete是否安全.

```c++
void *operator new(size_t size)
{
    void *p = getMemory(size); //调用函数分配内存并处理内存不足的情况
    add p to the collection of allocated addresses;//将p加入已分配地址的集合中
    return p;
}
void operator delete(void *ptr)
{
    releaseMemory(ptr); // 归还内存
    //remove ptr from the collection of allocated addresses//从分配的地址集合中删除ptr
}
bool isSafeToDelete(const void *address)
{
    return whether address is in collection of allocated addresses;//地址是否在已分配地址的集合中
}
```

此方法有个很大的缺点：当对象涉及多重继承或虚继承的基类时,会拥有多个地址,因此不能保证"交给isSafeToDelete"和"被operator new返回"的地址是同一个,纵使使用delete是安全的,像这样:

```cpp
class Base1{
public:
    virtual ~Base(){}
    ...
private:
    int a;
}
class Base2{
public:
    virtual ~Base2(){}
    ...
private:
    int b;
}
class Derived:public Base1,public Base2{}
Base2* ptr=new Derived;
//ptr所指地址显然不在所维护的表中,因此isSafeToDelete返回false,但对ptr使用delete却是安全的,因为Base2的析构函数为虚.
```

### 策略4:使用mixin模式,

设计一abstract base class,用于提供一组定义完好的能力,像这样: 

```cpp
class HeapTracked { 
public: 
    class MissingAddress{}; // 异常类
    virtual ~HeapTracked() = 0;//纯虚函数
    static void *operator new(size_t size);
    static void operator delete(void *ptr);
    bool isOnHeap() const;
private:
    typedef const void* RawAddress;
    static list<RawAddress> addresses;//维护heap地址的表
}；
list<RawAddress> HeapTracked::addresses;
 
// 析构函数设为纯虚函数以使得该类成为抽象类,但必须有定义.
HeapTracked::~HeapTracked() {}
void * HeapTracked::operator new(size_t size)
{
    void *memPtr = ::operator new(size); //取得内存
    addresses.push_front(memPtr); // 在表头插入新地址
    return memPtr;
}
void HeapTracked::operator delete(void *ptr)
{
    //查找是否在表中
    list<RawAddress>::iterator it =find(addresses.begin(), addresses.end(), ptr);
    if (it != addresses.end()) { 
        addresses.erase(it); 
        ::operator delete(ptr); //释放内存
    } 
    else {
        throw MissingAddress(); 
    } 
}
bool HeapTracked::isOnHeap() const
{
    // 得到一个指针，指向*this占据的内存空间的起始处，
    const void *rawAddress = dynamic_cast<const void*>(this);
    // 在表中查找
    list<RawAddress>::iterator it =find(addresses.begin(), addresses.end(), rawAddress);
    return it != addresses.end(); // 返回it是否被找到
}
```

唯一需要解释的一点就是isOnTheHeap中的以下语句:

```cpp
const void *rawAddress = dynamic_cast<const void*>(this);
```

  这里利用了dynamic_cast<void*>的一个特性——它返回的指针指向原生指针的内存起始处,从而解决了策略3的多继承对象内存不唯一问题.(要使用dynamic_cast,要求对象至少有一个virtual function).

  任何类如果需要判断delete是否安全,只需要继承HeapTracked即可.

## 3.禁止对象产生于heap之中

对象的存在形式有三种可能:1) 对象被直接实例化 2)对象被实例化为derived class objects内的"base class 成分" 3)对象被内嵌与其他对象之中   

 要阻止对象直接实例化于heap之中,只要利用"new 操作符调用opearator new而我们可以重载operator new"的原理即可,将operator new或operator delete设为[private](https://so.csdn.net/so/search?q=private&spm=1001.2101.3001.7020),像这样:

```cpp
class UPNumber {
private:
    static void *operator new(size_t size);
    static void operator delete(void *ptr);
    ...
};
```

operator new和operator delete一同设为private是为了统一它们的访问层级,值得注意的是,将operator new声明为private,也会阻止UPNumber对象被实例化为heap-based derived class objects的"base class 成分",因为operator new和operator delete都会被继承,如果这些函数不再derived class中重定义,derived class使用的就是base class版本(但已被设为private)

  但如果derived class声明自己的operator new和operator delete或涉及到内含的情况时,对象仍然可能位于heap内,正如2所总结,**没有一个有效办法判断一个对象是否位于heap内.**

# 13.Smart Pointers

可以很容易想到的行为控制权：构造和析构、赋值和复制、解引用等等。

## 1.Smart Pointers的构造、赋值、析构

构造容易理解。

赋值和复制以及析构会因为拥有权的问题有些复杂。涉及一个对象资源只可以被一个拥有、被多个可以拥有、所有权转移、自我赋值等问题都要考虑。

当所有权会转移时，以by value的形式传递智能指针是糟糕的，以pass-by-reference-to-const才是适当的。

析构等一定要考虑是否存在引用计数的问题。

## 2.解引操作符

解引时要考虑是否有缓式取值的设计。

返回值应该是reference的形式，可以避免指向派生体系的问题。

考虑operator->,eg:

```c++
void F1(DBPtr<Tuole>& pt)  //假设函数传入的是一个智能指针
{
    do{
        pt->display();
    }while(pt->isValid==faluse);
}
//其中的语句pt->display();会被编译器解释为：（pt.operator->()）->display();
//这意味着不论operator->返回什么，在该传回值上执行->动作是合法的。
//因此，一般operator->这只能返回两种东西，要么一个实际的指针，要么返回一个智能指针
//eg
template<class T>
T*DBPtr<T>::operator->()const
{
    ...
}
```

## 3.测试Smart Pointers是否为Null

有时希望完成类似的测试：

```cpp
SmartPtr<TreeNode> ptn;
if(ptn==0)...;
if(!ptn)...;
if(ptn)...;
//一种做法是提供一些隐式类型转换操作符，eg:
template<class T>
class SmartPtr
{
public:
    operator void*();  //以上判断没有问题了，但是有明显缺点，建条款5
}
//eg以下程序居然可以过关
SmartPtr<Apple> pa;
SmartPtr<banana> pa;
if(pa==pb)...;
//另有一种做法如重载！
template<class T>
class SmartPtr
{
public:
    bool operator！()const;  //也有明显问题
}
//eg
SmartPtr<Apple> pa;
SmartPtr<banana> pa;
if(!pa==!pb)...; //居然可以过关
```

## 4.将Smart Pointers转化为Dumb Pointers

提供隐式类型转换操作符。

template<class T>
class SmartPtr
{
public:
    operator T*();  //首先有明显缺点，建条款5
}

即使提供了这种转换，也还是不好使用。

```cpp
DBPtr<Tuple> pt=new Tuple;
...
delete pt; //本不应该通过，但现在通过了，可能造成这里删除一次，智能指针析构再删除一次
//还有一个问题，c++是无法施行一次以上的自定义类型转换
class A
{
public：
    A（const Tuple*pt）;  
}；
//一个函数
A F1(const A &ta1,const A &ta2);
//假设
DBPtr<Tuple> ptr1,ptr2;
F1(ptr1,ptr2);  //失败，需要两次转换，DBPtr<Tuple> 转化为Tuple*,  之后Tuple*转化为A；
```

## 5.智能指针和与继承有关系的类型转换

智能指针本质是个类，如果不做特殊处理，在涉及继承体系类型转换时会出问题。

```cpp
class Base  //一个简易的继承体系
{
public:
    Base(const string& title);
    virtual void f1()const=0;
    virtual void f2()const=0;
};
class A1:public Base
{
public:
    A1(const string& title);
    virtual void f1()const;
    virtual void f2()const;
};
class A2:public Base
{
public:
    A2(const string& title);
    virtual void f1()const;
    virtual void f2()const;
};
void displayandPlay(const SmartPtr<Base> &pmp,int cont); //传入的是智能指针类
SmartPtr<A1> a1(new A1("hello"));
SmartPtr<A2> a2(new A2("fuck"));
displayandPlay(a1,10);//错误无法将指向派生类的智能指针转化为指向基类的智能指针，因为它们之间没有继承关系，与我们想要的预期不
displayandPlay(a2,5); //符合
```

简单的加入一个隐式类型转换操作符不能很好地解除问题，因为首先会很麻烦，而且想要指向派生类的智能指针转换为指向非直接基类的也会很不方便。

**合理的做法：**利用非虚成员函数的template化。

```cpp
//eg:
template<class T>
class SmartPtr
{
public:
    ...
	template<class Newtype>
    operator SmartPtr<Newtype>()
    {
        return SmartPtr<Newtype>(pointee);
    }
    ...
};//此时，以上的调用可以成功.
void displayandPlay(const SmartPtr<Base> &pmp,int cont); //传入的是智能指针类
SmartPtr<A1> a1(new A1("hello"));
SmartPtr<A2> a2(new A2("fuck"));
displayandPlay(a1,10);
displayandPlay(a2,5); 
```

成功原因：

a1是一个SmartPtr<A1>类型，而displayandPlay函数希望得到的是SmartPtr<Base>类型的对象。于是编译器先寻找某种方法企图将a1转换成一个SmartPtr<Base>，在SmartPtr<Base> 类内寻找单一变量的构造函数，且变量类型为SmartPtr<A1>，未果；在继续在SmartPtr<A1>寻找合适的类型转换操作，也失败，最后寻找一个可以实例化导出转换函数的member function template，在SmartPtr<A1>内找到一个，实例化如下：

```cpp
SmartPtr<A1>::operator SmartPtr<Base>()
{  
    return SmartPtr<Base>(pointee);//至此，问题转化为是否可以以一个A1类型的指针构造SmartPtr<Base>，即A1*能不能
}//转化为Base*，明显可以。
```

但是，有一个问题无法解决：

```cpp
template<class T>
class SmartPtr
{...};
class A11:public A1{...};
void displayandPlay(const SmartPtr<Base> &pmp,int cont); 
void displayandPlay(const SmartPtr<A1> &pmp,int cont); //参数不一样，重载了
SmartPtr<A11> a1(new A1("hello"));
displayandPlay(a1,10);//错误
//如果是指针本体，预期调用的自然是传入A1的类型指针，因为A11直接继承自它而非Base,但是智能指针类中的成员函数模板实例化为转换函数
//对于转换函数而言，他们一样好。
```

## 6.Smart Pointers与const

普通指针与+const关系：

```cpp
cd goodcd("fuck");
const cd *p;//p是一个non-const指针，指向一个const cd object
cd* const p=&goodcd;//p是一个const指针，指向一个non-const cd object,由于P是const，必须有初值
const cd* const p=&doodcd;//p是一个const指针，指向一个const cd object
```

我们自然希望智能指针也有类似关系，但是智能指针只有一个地方可以放const：只能施与指针身上，不能给予所指对象。

const SmartPtr<cd> p=&goodcd; //p是一个const智能指针，指向一个non-const cd object

似乎可以：SmartPtr<const cd> p=&goodcd;//p是一个non-const智能指针，指向一个const cd object

我们希望有以下组合：

SmartPtr<cd> p;//non-const object  non-const pointers

SmartPtr<const cd> p;//const object  non-const pointers

const SmartPtr<cd> p=&doodcd;;//non-const object  const pointers

const SmartPtr<const cd> p;//const object  const pointers

**但是，普通指针我们可以non-const指针作为const指针的初值，也可以指向non-const对象的指针作为指向const对象的初值。**

而SmartPtr<cd> p；

SmartPtr<const cd> p1=p;//显然不对，类型都不一样

**从non-const转换至const安全，反之不可，对const指针做的事可以对non-const指针做，但更多；对pointer to const 做的事可以对pointer to non const做，但更多**，听起来特别想继承体系的规则，于是：另一个Smart pointer to class,Smart pointer to const classc成为继承体系。

```cpp
template<class T>
class SmartPtrTOconst
{
public:
    ...//相关function
protected:
    union{
        const T* constPointee;
        T*pointee;
    }      
};
template<class T>
class SmartPtr:public SmartPtrTOconst<T>
{...};
SmartPtr<cd> p=new cd("fuck")；
SmartPtr<const cd> p1=p;//可以
```

# 14.引用计数 (需要再次阅读)

## 1.实现引用计数

“我们需要一个地方来存储这个计数值”是很重要的。这个地方不能在String对象内部，因为需要的是每个String值一个引用计数值，而不是每个String对象一个引用计数。这意味着String值和引用计数间是一一对应的关系，所以我们将创建一个类来保存引用计数及其跟踪的值。我们叫这个类StringValue，又因为它唯一的用处就是帮助我们实现String类，所以我们将它嵌套在String类的私有区内。另外，为了便于Sting的所有成员函数读取其数据区，我们将StringValue申明为struct。需要知道的是：将一个struct内嵌在类的私有区内，能便于这个类的所有成员访问这个结构，但阻止了其它任何人对它的访问（当然，除了友元）。

```cpp
class String {
private:
    struct StringValue {
        int refCount;
        char *data;
        StringValue(const char *initValue);
        ~StringValue();
    };
    StringValue* value;
    ...
};
String::StringValue::StringValue(const char *initValue): refCount(1)
{
    data = new char[strlen(initValue) + 1];
    strcpy(data, initValue);
}
String::StringValue::~StringValue()
{
    delete [] data;
}
//完成相应的string成员函数
String::String(const char *initValue): value(new StringValue(initValue)){}
String::String(const String& rhs): value(rhs.value){ ++value->refCount; }
String::~String()// 析构函数负责在引用计数为0的时候撤销内存:
{
    if (--value->refCount == 0) 
        delete value;
}
String& String::operator=(const String& rhs)//赋值操作符要注意自身赋值的情况:
{
    if(value==rhs.value)
        return *this;
    if(--value->refCount==0)
        delete value;
    value=rhs.value;
    ++value->refCount;
    return *this;
}
```

## 2.Copy-on-Write(写时才复制)

 对operator[]的重载比较复杂:const版本是只读动作,因而只返回指定字符即可,但non-const版本面临着被写入新的值的可能,由于对当前String的修改不应影响到共享内存的其他String对象,因此需要先为当前String分配独立内存并将原值进行拷贝,

```cpp
const char& String::operator[](int index) const{ return value->data[index]; }
char& String::operator[](int index)
{
    if (value->refCount > 1) {
        --value->refCount; 
        value =new StringValue(value->data); 
    }
    return value->data[index];
}
```

不仅是operator[],其他可能改变String对象的操作也应该采取和non-cons版本operator[]相同的动作.这其实是lazy evaluation的一种应用.

## 3.Pointers,References,以及Copy-on-Write

3中对operator[]的重载解释并解决了可能的写操作篡改共享内存的问题,但是却无法阻止外部指针或引用对共享内存的篡改,像这样:

```c++
String s1 = "Hello";
char *p = &s1[1];
String s2 = s1;
```

对p所指向的内存的任何写操作都会同时更改s1和s2的值,但是s1却对此一无所知,因为p和s1没有内在联系.解决办法并不难:为每一个StringValue对象加一个标志(flag)变量,用表示是否可以被共享,开始时先将flag设为true(可以共享),一旦non-const operator[]被调用就将该flag设为false,并可能永远不许再更改(除非重新为StringValue分配更大内存而导致指针失效),StringValue的修改版像这样:

```c++
class String {
private:
    struct StringValue {
        int refCount;
        bool shareable; //新加入的标志位
        char *data;
        StringValue(const char *initValue);
        ~StringValue();
    };
    ...
};
String::StringValue::StringValue(const char *initValue): refCount(1),shareable(true) 
{
    data = new char[strlen(initValue) + 1];
    strcpy(data, initValue);
}
String::StringValue::~StringValue()
{
    delete [] data;
}
```

 String的member function在企图使用共享内存前,就必须测试内存是否允许被共享:

```c++
String::String(const String& rhs)
{
    if(rhs.value->shareable)
    {
        value=rhs.rhs.value;
        ++value->refCount;
    }
    else
    {
        value=new StringValue(rhs.value->data);
    }
}
//其他返回引用的member function(对于String只有operator[])都涉及到对flag的修改,而其他可能需要共享内存的member function都涉及到对flag的检测.
```

## 4.一个引用计数的基类

任何要支持内存共享的class都可以使用reference-counting,因此可以考虑把它抽象为一个类,任何需要reference-counting功能的class只要使用这个类即可.第一步就是产生一个base class RCObject,执行引用计数的功能并标记对象是否可被共享,像这样:

```cpp
class RCObject {
public:
    RCObject();
    RCObject(const RCObject& rhs);
    RCObject& operator=(const RCObject& rhs);
    virtual ~RCObject() = 0;
    void addReference();
    void removeReference();
    void markUnshareable();
    bool isShareable() const;
    bool isShared() const;
private:
    int refCount;
    bool shareable;
};
```

由于RCObject的作用只是实现引用计数的辅助功能,然后让StringValue继承它,因此RCObject被设为一个抽象基类——通过将析构函数设为纯虚函数,但仍需要为析构函数提供定义.RCObject的实现像这样:

```c++
RCObject::RCObject(): refCount(0), shareable(true) {}
RCObject::RCObject(const RCObject&):refCount(0),shareable(true) {}
RCObject& RCObject::operator=(const RCObject&)
{ return *this; }
RCObject::~RCObject() {} // virtual dtors must always
void RCObject::addReference() { ++refCount; }
void RCObject::removeReference()
{ if (--refCount == 0) delete this; }
void RCObject::markUnshareable()
{ shareable = false; }
bool RCObject::isShareable() const
{ return shareable; }
bool RCObject::isShared() const
{ return refCount > 1; }
//其拷贝构造函数和赋值操作符有些特殊——它们的参数没有名字,也就是说参数没有作用,其拷贝构造函数和赋值操作符都只是形式上的:
```

RCObjetc拷贝构造函数与RCObject的作用相对应——RCObject一旦被构造,就说明一个新的对象被产生出来,那么RCObject对象本身的初始值和默认构造函数相同,至于refCount设为0而不是1,这要求对象创建者自行将refCount设为1.

RCObject的赋值操作符什么也不做,仅仅返回*this,因为它不应该被调用,正如之前的StringValue,如果对String对象赋值,那么或者StringValue被共享,或者拷贝构造一个新的StringValue,实际上StringValue的赋值操作永远不会被调用.即使要对StringValue做赋值操作,像这样:

```c++
sv1=sv2;//sv1和sv2是StringValue型对象
```

  指向sv1和sv2的对象数目实际上并未改变,因此sv1的基类部分RCObject什么也不做仍然是正确的.

  removeReference的责任不仅在于将refCount减1,实际上还承担了析构函数的作用——在refCount=1的时候delete销毁对象,从这里可以看出RCObject必须被产生于heap中.

StringValue要直接使用RCObject,像这样:

```c++
class String {
private:
    struct StringValue: public RCObject {
        // StringValue类public继承自RCObject,因此它继承了RCObject的接口并供String使用,StringValue也必须构造在heap中.
        char *data;
        StringValue(const char *initValue);
        ~StringValue();
    };
    ...
};
String::StringValue::StringValue(const char *initValue)
{
    data = new char[strlen(initValue) + 1];
    strcpy(data, initValue);
}
String::StringValue::~StringValue()
{
    delete [] data;
}
```

## 5.自动操作Reference Count(引用计数)

RCObject提供了一定程度的代码复用功能,但还远远不够——String类仍然需要手动调用RCObject的成员函数来对引用计数进行更改.解决方法就是"计算机科学领域中大部分问题得以解决的原理"——在中间加一层,也就是在String和StringValue中间加一层智能指针类对引用计数进行管理,像这样:

```c++
//管理引用计数的智能指针类
template<class T>
class RCPtr {
public:
    RCPtr(T* realPtr = 0);
    RCPtr(const RCPtr& rhs);
    ~RCPtr();
    RCPtr& operator=(const RCPtr& rhs);
    T* operator->() const; // see Item 28
    T& operator*() const; // see Item 28
private:
    T *pointee; 
    void init(); //将构造函数中的重复操作提取成一个函数
};
```

之前RCPtr是一个类模板,String之前有一个StringValue*成员,现在只要将它替换为RCPtr<StringValue>即可.

  RCPtr的构造函数像这样:

```c++
template<class T>
RCPtr<T>::RCPtr(T* realPtr): pointee(realPtr)
{
    init();
}
template<class T>
RCPtr<T>::RCPtr(const RCPtr& rhs): pointee(rhs.pointee)
{
    init();
}
template<class T>
void RCPtr<T>::init()
{
    if (pointee == 0) {
        return; 
    }
    if (pointee->isShareable() == false) { 
        pointee = new T(*pointee); 
    } 
    pointee->addReference();//引用计数的更改负担转移到这里
}
```

init中使用了new关键字,它调用T的拷贝构造函数,为防止编译器为StringValue合成的拷贝构造函数执行浅复制,需要为StringValue定义执行深度复制的拷贝构造函数,像这样:

```c++
String::StringValue::StringValue(const StringValue& rhs)
{
    data = new char[strlen(rhs.data) + 1];
    strcpy(data, rhs.data);
}
```

 此外,由于多态性的存在,尽管pointee是T*类型,但它实际可能指向T类型的派生类,在此情况下new调用的却是T的拷贝构造函数,要防止这种现象,可以使用virtual copy constructor,这里不再讨论.

```c++
// RCPrt的其余实现像这样:
template<class T>
RCPtr<T>& RCPtr<T>::operator=(const RCPtr& rhs)
{
    if (pointee != rhs.pointee) {
        if (pointee) {
            pointee->removeReference();
        } 
        pointee = rhs.pointee; 
        init(); 
    } 
    return *this;
}
template<class T>
RCPtr<T>::~RCPtr()
{
    if (pointee)pointee->removeReference();
}
template<class T>
T* RCPtr<T>::operator->() const { return pointee; }
template<class T>
T& RCPtr<T>::operator*() const { return *pointee; }
```

## 6.一个完整的引用计数例子

```c++
template<class T>  //模板用来产生智能指针指向 T object，T必须继承自RCObject。
class RCPtr
{
public:
    RCPtr(T* realPtr=0);
    RCPtr(const RCPtr& rhs);
    ~RCPtr();
    RCPtr& operator=(const RCPtr& rhs);
    T* operator->()const;
    T& operator*()const;
private:
    T* pointee;
    void init();
}

class RCObject  //base class 用于reference-counted object
{
public:
    void addReference();
    void removeReference();
    void markUnshareable();
    bool isShareable()const;
    bool isShared()const;
protected:
    RCObject();
    RCObject(const RCObject& rhs);
    RCObject& operator=(const RCObject& rhs);
    virtual ~RCObject()=0;
private: 
    int refCount;
    bool shareable;
};

class String  //应用性class，是应用程序开发接触到的层面
{
public:
    String(const char* value="");
    const char& operaator[](int index)const;
    char& operator[](int index);
private:    
    //以下struct用于表现字符串实值
    struct StringValue:public RCObject
    {
        char* data;
        SrtingValue(const char* initValue);
        StringValue(const StringValue& rhs);
        void init(const char* initValue);
        ~StringValue();
    };
    RCPtr<StringValue> value;
};
// RCObject的实现:
RCObjcet::RCObjcet():refCount(0),shareable(true){}
RCObjcet::RCObjcet(const RCObject&):refCount(0),shareable(true){}
RCObject& RCObject::operator=(const RCObject& rhs){return *this;}
RCObjcet::~RCObjcet(){}
void RCObject::addReference()
{
    ++refCount;
}
void RCObject::removeReference(){
    if(--refCount==0)
        delete this;
}
void RCObject::markUnshareable()
{
    shareable=false;
}
bool RCObject::isShareable()const
{
    return shareable;
}
bool RCObject::isShared()const
{
    return refCount>1;
}
//RCPtr的实现
template<class T>
void RCPtr<T>::init()
{
    if (pointee == 0) return;
    if (pointee->isShareable() == false) {
        pointee = new T(*pointee);
    }
    pointee->addReference();
}
template<class T>
RCPtr<T>::RCPtr(T* realPtr): pointee(realPtr)
{ init(); }
template<class T>
RCPtr<T>::RCPtr(const RCPtr& rhs): pointee(rhs.pointee)
{ init(); }
template<class T>
RCPtr<T>::~RCPtr()
{ if (pointee)pointee->removeReference(); }
template<class T>
RCPtr<T>& RCPtr<T>::operator=(const RCPtr& rhs)
{
    if (pointee != rhs.pointee) {
        if (pointee) pointee->removeReference();
        pointee = rhs.pointee;
        init();
    }
    return *this;
}
template<class T>
T* RCPtr<T>::operator->() const { return pointee; }
template<class T>
T& RCPtr<T>::operator*() const { return *pointee; }
//StringValue的实现:
void String::StringValue::init(const char *initValue)
{
    data = new char[strlen(initValue) + 1];
    strcpy(data, initValue);
}
String::StringValue::StringValue(const char *initValue)
{ init(initValue); }
String::StringValue::StringValue(const StringValue& rhs)
{ init(rhs.data); }
String::StringValue::~StringValue()
{ delete [] data; }
//String的实现:
String::String(const char *initValue): value(new StringValue(initValue)) {}
const char& String::operator[](int index) const
{ return value->data[index]; }
char& String::operator[](int index)
{
    //String类唯一需要接触底层成员的负担
    if (value->isShared()) { value = new StringValue(value->data); }             
    value->markUnshareable(); return value->data[index]; 
}
```

## 7.引用计数加到既有的class上。

有了以上设计,就可以使任何需要引用计数功能的类只要继承RCObject,并作为已有的RCPtr模板的类型参数即可.但程序库中的类却无法更改:假设程序库中存在一个名为Widget的类,我们无法修改它,因此也就无法使它继承RCObject.但只要采取之前所用的"中间加一层的方法,这种目标仍可以达成":

```c++
//RCIPtr和CountHolder的实现如下:
template<class T>
class RCIPtr {
public:
    RCIPtr(T* realPtr = 0);
    RCIPtr(const RCIPtr& rhs);
    ~RCIPtr();
    RCIPtr& operator=(const RCIPtr& rhs);
    const T* operator->() const; 
    T* operator->(); 
    const T& operator*() const; 
    T& operator*(); 
private:
    struct CountHolder: public RCObject {
        ~CountHolder() { delete pointee; }
        T *pointee;
    };
    CountHolder *counter;
    void init();
    void makeCopy(); 
};
template<class T>
void RCIPtr<T>::init()
{
    if (counter->isShareable() == false) {
        T *oldValue = counter->pointee;
        counter = new CountHolder;
        counter->pointee = new T(*oldValue);
    }
    counter->addReference();
}
template<class T>
RCIPtr<T>::RCIPtr(T* realPtr): counter(new CountHolder)
{
    counter->pointee = realPtr;
    init();
}
template<class T>
RCIPtr<T>::RCIPtr(const RCIPtr& rhs): counter(rhs.counter)
{ init(); }
template<class T>
RCIPtr<T>::~RCIPtr()
{ counter->removeReference(); }
template<class T>
RCIPtr<T>& RCIPtr<T>::operator=(const RCIPtr& rhs)
{
    if (counter != rhs.counter) {
        counter->removeReference();
        counter = rhs.counter;
        init();
    }
    return *this;
}
template<class T> 
void RCIPtr<T>::makeCopy()
{ 
    if (counter->isShared()) {
        T *oldValue = counter->pointee;
        counter->removeReference();
        counter = new CountHolder;
        counter->pointee = new T(*oldValue);
        counter->addReference();
    }
}
template<class T>
const T* RCIPtr<T>::operator->() const 
{ return counter->pointee; }
template<class T> 
T* RCIPtr<T>::operator->() 
{ 
    makeCopy();
    return counter->pointee; 
}
template<class T>
const T& RCIPtr<T>::operator*() const 
{ return *(counter->pointee); }
template<class T>
T& RCIPtr<T>::operator*() 
{ 
    makeCopy(); 
    return *(counter->pointee); 
} 
```

 CountHolder只对RCIPter可见,因此设为private,此外,RCIPtr提供了non-const版本的operator->和operator*,只要有non-const 函数被调用,copy-on-write就被执行.

  有引用计数功能的RCWidget只要通过底层的RCIPtr调用对应的Widget函数即可,如果Widget的接口像这样:

```c++
class Widget {
public:
    Widget(int size);
    Widget(const Widget& rhs);
    ~Widget();
    Widget& operator=(const Widget& rhs);
    void doThis();
    int showThat() const;
    ...
};
```

View Code

   那么RCWidget只要被定义成这样:

```c++
class RCWidget {
public:
    RCWidget(int size): value(new Widget(size)) {}
    void doThis() { value->doThis(); }
    int showThat() const { return value->showThat(); }
private:
    RCIPtr<Widget> value;
};
```

# 15.Proxy classes (替身类)

## 1.实现二维数组

.假设产生一个class：

```c++
template<class T>
class Array2D
{
public:
    Array2D(int dim1,int dim2);
    ...
};
//于是一些简单的定义和使用可以实现
Array2D<int> data(10,20);
Array2D<float> *data=new Array2D<float>(10,20);
//但是数组索引不是很方便。无法重载operator[][]。看普通数组int data[10][20];
//data[5][6]真正表达的意思是(data[5])[6]  ,即data第5个元素所表现的数组的第6个元素。
//在Array2D class内重载operator[]，返回一个Array1D对象，再在Array1D class内重载operator[]，令其返回一个元素。
template<class T>
class Array2D
{
public:
    class Array1D
    {
    public:
        T& operator[](int index);
        const T& operator[](int index)const;
        ...
    }
    Array1D operator[](int index);
    const Array1D operator[](int index)const;
    ...
};
```

每一个Array1D对象象征一个一维数组，观念上它并不存在与Array2D客户心中。用来代表其他对象的对象，成为proxy objects（替身对象）。本例中，Array1D是个替身类，代表观念上并不存在的一维数组。

## 2.区分operator[]的读写动作

首先，对于一个引用计数的字符串类型，它支持operator[]。但是以下操作并不能解决区分读写。

```c++
class String
{
public:
    const char& operator[](int index)const;
    char& operator[](int index);
    ...
};
//无用的，因为只根据调用该函数的对象是否是const为依据，eg:
String s1,s2;
cout<<s1[5]; //调用non-const operator[]，因为s1不是const
s1[3]=s2[5];//左右都调用non-const operator[]，因为s1,s2都不是non-const对象
```

读操作是所谓的右值引用；写操作是左值引用。虽然编译器无法告诉我们operator[]到底是用来读还是写，但是只要将所要处理的动作放缓，直到operator[]的返回结果被使用为止。可以修改operator[]，使其返回字符串中字符的proxy，就可以看见该proxy如何被使用：

```c++
class String {
public:
    String(const char *value = "");
     
    class CharProxy {   //字符的proxies类，哪个字符串的哪个字符
    public:
        CharProxy(String& str, int index);
        CharProxy& operator=(const CharProxy& rhs);   //  左值引用
        CharProxy& operator=(char c);
        operator char() const; //  右值引用
    private:
        String& theString;
        int charIndex;
    };
    const CharProxy operator[](int index) const; //针对const string
    CharProxy operator[](int index); //针对non-const string

friend class CharProxy;

private:
    struct StringValue: public RCObject  {
        char *data;
        StringValue(const char *initValue);
        StringValue(const StringValue& rhs);
        void init(const char *initValue);
        ~StringValue();
    };

    RCPtr<StringValue> value;
};
```

有了上面的代码，考虑这条语句：cout<<s1[5]，s1[5]产生一个CharProxy对象，但是该对象没有定义output操作符，所以编译器需要找一个合适的隐式类型转换：将CharProxy隐式转换为char，然后进行输出即可。

而对于s1[5]=’x’，s1[5]返回CharProxy，调用CharProxy::operator=函数即可，这时，被赋值的CharProxy对象被用来作为一个左值。同样的，s1[5]=s1[8]也是一样。String的operator[]代码如下：

```c++
const String::CharProxy String::operator[](int index) const{
    return CharProxy(const_cast<String&>(*this), index);
}
String::CharProxy String::operator[](int index){
    return CharProxy(*this, index);
}
```

注意const operator[]返回一个const CharProxy，由于CharProxy::operator=不是一个 const成员函数，const CharProxy因此不能被用来做为赋值的目标物。因此不论是const operator[] 所传回的 proxy，或是该proxy所代表的字符，都不能被用来做为左值。这正是我们希望 const operator[]所具备的行为。

另外，当const operator[] 返回CharProxy对象时，需要对其进行const_cast转换，去除const属性，这是因为CharProxy的构造函数只接受non-const String。

operator[]返回的每一个CharProxy都会记住它所附属的字符串，以及它所在的索引位置，以便将来进行读取或赋值：

```c++
String::CharProxy::CharProxy(String& str, int index)
: theString(str), charIndex(index) {}
```

，将proxy转换为右值，直截了当。CharProxy的char转换函数如下：

```c++
String::CharProxy::operator char() const{
    return theString.value->data[charIndex];
}
```

 该函数以by value的方式返回一个字符，由于C++ 限制只能在右值情境下使用这样的返回值，所以这个转换函数只能用于右值合法之处。

实现charProxy的operator=操作符；

```c++
String::CharProxy& String::CharProxy::operator=(const CharProxy& rhs)
{
    if(theString.value->isShared())  //如果本字符串与其他的String对象共享一个实值，复制一份
    {
        theString.value=new StringVlaue(theString.value->data);
    }
    //进行赋值操作，
    theString.value->data[charIndex]=rhs.theString.value->data[rhs.charIndex];
    return *this;
}

String::CharProxy& String::CharProxy::operator=(char c)
{
    if(theString.value->isShared())  //如果本字符串与其他的String对象共享一个实值，复制一份
    {
        theString.value=new StringVlaue(theString.value->data);
    }
    //进行赋值操作，
    theString.value->data[charIndex]=c;
    return *this;
}
```

 与上一章中的non-const String::operator[]比较，会发现它们非常类似。在上一章中悲观地假设所有non-const operator[]的调用都是为了写操作，此处们把完成“写操作”的代码转移到CharProxy的operator=中，避免了non-const operator[]在右值情境下也需付出写操作的昂贵代价。

## 3.某些限制

 尽管我们希望proxy对象能够无间隙的代表它所表示的对象，但是这种想法很难达成，因为除了赋值之外，对象还有很多其他操作，比如：char *p = &s1[5]，这条语句有两个报错，s1[5]返回一个临时CharProxy对象，既不能取临时对象的地址，也无法将CharProxy*赋值给char *，这种情况下，需要重载operator&：

```c++
const char * String::CharProxy::operator&() const{
    return &(theString.value->data[charIndex]); 
}

char * String::CharProxy::operator&(){
    if (theString.value->isShared()) {
        theString.value = new StringValue(theString.value->data);
    }

    theString.value->markUnshareable();
    return &(theString.value->data[charIndex]);
}// non-const版本需要返回一个指针，指向一个可能被修改的字符，因此，需要将StringValue成为其专属副本
```

   这还仅仅是取地址，原始对象可能还支持+=，++等操作，这些需要左值的操作想要成功，必须一一为proxy类定义相应的操作符（CharProxy转换为char的操作符，返回的是右值，因此无法使用这些操作符）。

另外，如果proxy代表的对象具有成员函数，则为了能使proxy看起来像原始对象一样能调用相应的成员函数，也需要在proxy中定义相应的函数。

 另外，proxy虽然能隐式转换为它所代表的对象，但是这种隐式转换只能发生一次，如果原始对象A能隐式转换为B，则在需要参数B的函数调用中可以使用A，但不能使用A的proxy。

# 16.让函数根据一个以上的对象类型决定如何虚化

 假设你在写一个游戏，游戏中需要处理宇宙飞船、太空站、小行星等的碰撞问题，不同物体之间的碰撞需要做不同的处理。于是有下面的代码：

```c++
class GameObject { ... };
class SpaceShip: public GameObject { ... };
class SpaceStation: public GameObject { ... };
class Asteroid: public GameObject { ... };
void checkForCollision(GameObject& object1, GameObject& object2)
{
    if (theyJustCollided(object1, object2)) {
        processCollision(object1, object2);
    }
    else { ...}
}
```

processCollision需要根据object1和object2的具体类型做不同的处理。这就是所谓的double-dispatching问题。某个函数调用如果根据两个参数而虚化，自然而然地就被称为  "double dispatch"。更广泛的情况则被称为multiple dispatch。

## 1.最一般化的double-dispatching实现，就是利用虚函数和RTTI（运行时类型辨识）：

```c++
class GameObject {
public:
    virtual void collide(GameObject& otherObject) = 0;
    ...
};
class SpaceShip: public GameObject {
public:
    virtual void collide(GameObject& otherObject);//虚函数实现出single dispatch，一半的类型处理
    ...
};

class CollisionWithUnknownObject {  //需要的异常类
public:
    CollisionWithUnknownObject(GameObject& whatWeHit);
    ...
};
void SpaceShip::collide(GameObject& otherObject){  //在具体函数实现内利用if-else实现另一半类型辨识
    const type_info& objectType = typeid(otherObject);
    if (objectType == typeid(SpaceShip)) {
        SpaceShip& ss = static_cast<SpaceShip&>(otherObject);
        process a SpaceShip-SpaceShip collision;
    }
    else if (objectType == typeid(SpaceStation)) {
        SpaceStation& ss = static_cast<SpaceStation&>(otherObject);
        process a SpaceShip-SpaceStation collision;
    }
    else if (objectType == typeid(Asteroid)) {
        Asteroid& a = static_cast<Asteroid&>(otherObject);
        process a SpaceShip-Asteroid collision;
    }
    else {
        throw CollisionWithUnknownObject(otherObject);
    }
}
```

 上面这段代码的缺点，collide函数必须知道其每一个兄弟类（所有继承自GameObject的那些类）。如果有新的类型加入了这个游戏，就必须修改程序中每一个可能遭遇新对象的RTTI-based if-then-else链，这会造成程序难以维护。

## 2.只用虚函数就解决这个问题

```c++
class SpaceShip;  //前置声明
class SpaceStation;
class Asteroid;
class GameObject {
public:
    virtual void collide(GameObject&  otherObject) = 0;
    virtual void collide(SpaceShip& otherObject) = 0;
    virtual void collide(SpaceStation& otherObject) = 0;
    virtual void collide(Asteroid& otherobject) = 0;
    ...
};
class SpaceShip: public GameObject {
public:
    virtual void collide(GameObject&  otherObject);
    virtual void collide(SpaceShip& otherObject);
    virtual void collide(SpaceStation& otherObject);
    virtual void collide(Asteroid& otherobject);
    ...
};
void SpaceShip::collide(GameObject& otherObject){//
    otherObject.collide(*this);
}
void SpaceShip::collide(SpaceShip&  otherObject){
    process a SpaceShip-SpaceShip collision;
}
void SpaceShip::collide(SpaceStation&  otherObject){
    process a SpaceShip-SpaceStation collision;
}
void SpaceShip::collide(Asteroid&  otherObject){
    process a SpaceShip-Asteroid collision;
}
// 比较有意思的是接收GameObject&的那个collide函数，当对象的静态类型是GameObject时调用该函数，函数内部，根据参数的动态类型决定调用该类型的哪个虚函数，而函数参数为*this，也就是个SpaceShip类型。
//eg：	
SpaceShip b;
SpaceStation c;
GameObject& a = b;
GameObject& a2 = c;
a.collide(c);  //调用SpaceShip::collide(SpaceShip&  otherObject)
a.collide(a2); //调用SpaceShip::collide(GameObject& otherObject)
```

## 3.仿真虚函数表

虚函数是通过vtbl实现的。在这个double- dispatching场景中，我们可以自己实现一个vtbl，这比RTTI-based会更有效率，而且可以将RTTI的使用集中在vtbl初始化的地方.

修改GameObject继承体系内的函数：

```c++
class GameObject {
public:
    virtual void collide(GameObject& otherObject) = 0;//虚函数实现第一次的dispatch
    ...
};

class SpaceShip: public GameObject {
public:
    virtual void collide(GameObject& otherObject);
    virtual void hitSpaceShip(SpaceShip& otherObject);
    virtual void hitSpaceStation(SpaceStation& otherObject);
    virtual void hitAsteroid(Asteroid& otherobject);
    ...
private:
    typedef void (SpaceShip::*HitFunctionPtr)(GameObject&);
    static HitFunctionPtr lookup(const GameObject& whatWeHit);  //根据具体类返回指向特定的函数的指针，实现第二次的dispatch
};

void SpaceShip::collide(GameObject& otherObject){
    HitFunctionPtr hfp = lookup(otherObject);
    if (hfp) {
        (this->*hfp)(otherObject);//使用指向成员函数的指针
    }
    else {
        throw CollisionWithUnknownObject(otherObject);
    }
}

void SpaceShip::hitSpaceShip(SpaceShip& otherObject){
    process a SpaceShip-SpaceShip collision;
}
void SpaceShip::hitSpaceStation(SpaceStation& otherObject){
    process a SpaceShip-SpaceStation collision;
}
void SpaceShip::hitAsteroid(Asteroid& otherObject){
    process a SpaceShip-Asteroid collision;
}// 这里没有将collide重载，而是用函数名区分不同对象之间的碰撞
```

在SpaceShip::collide函数内，调用lookup函数，根据GameObject参数查找合适的成员函数指针，一旦找到则调用即可，找不到的时候抛出异常。

 现在考虑lookup函数的实现，在lookup中，需要一个关系型数组，lookup查找该数组，根据对象类型得到某个成员函数指针。这个关系型数组应该在使用之前就产生并初始化了，并在不再需要时进行销毁，可以使用new和delete来产生和销毁数组，但那样容易发生错误。为了保证数组会在使用之前进行初始化，比较好的解决办法就是让关系型数组成为 lookup内的static对象，只有在lookup第一次调用时它才会被产生，而在main结束之后它才会被摧毁：

```c++
class SpaceShip: public GameObject {
private:
    typedef map<string, HitFunctionPtr> HitMap;
    ...
};
SpaceShip::HitFunctionPtr SpaceShip::lookup(const GameObject& whatWeHit){
    static HitMap collisionMap; //稍后我们将看到如何初始化这玩意儿。
    HitMap::iterator mapEntry = collisionMap.find(typeid(whatWeHit).name());
    if (mapEntry == collisionMap.end()) return 0;
    return (*mapEntry).second;
}
```

## 4.将自行仿真的虚函数表初始化

 现在，唯一的问题就是collisionMap的初始化问题了，可以将其初始化放进一个名为initializeCollisionMap的private static成员函数中：

```c++
SpaceShip::HitFunctionPtr SpaceShip::lookup(const GameObject& whatWeHit)
{
    static HitMap collisionMap = initializeCollisionMap();
    ...
}
```

 但是这种方法会有map的复制成本，因此，这里最好改为指针，为了不操心delete指针的问题，可以使用智能指针：

```c++
SpaceShip::HitFunctionPtr SpaceShip::lookup(const GameObject& whatWeHit){
    static unique_ptr<HitMap> collisionMap(initializeCollisionMap());
    ...
}
SpaceShip::HitMap * SpaceShip::initializeCollisionMap(){
    HitMap *phm = new HitMap;
    (*phm)["SpaceShip"] = &hitSpaceShip;
    (*phm)["SpaceStation"] = &hitSpaceStation;
    (*phm)["Asteroid"] = &hitAsteroid;
    return phm;
}
```

这个初始化代码还有最后一个问题，map中的value类型是：typedef void (SpaceShip::*HitFunctionPtr)(GameObject&)，这种函数的参数为GameObject，但是hitSpaceShip、hitSpaceStation、hitAsteroid它们的参数分别为 SpaceShip、SpaceStation、Asteroid。虽然它们都可以隐式转换为GameObject，但是函数指针之前却不存在这种转换。因此，只能改变hitSpaceShip这些成员函数的原型，使他们接受GameObject对象：

```c++
class SpaceShip: public GameObject {
public:
    virtual void collide(GameObject& otherObject);  
    virtual void hitSpaceShip(GameObject&  spaceShip); //函数参数改变，全部是GameObject&
    virtual void hitSpaceStation(GameObject&  spaceStation);
    virtual void hitAsteroid(GameObject&  asteroid);
    ...
};

SpaceShip::HitMap * SpaceShip::initializeCollisionMap(){
    HitMap *phm = new HitMap;
    (*phm)["SpaceShip"] = &hitSpaceShip;
    (*phm)["SpaceStation"] = &hitSpaceStation;
    (*phm)["Asteroid"] = &hitAsteroid;
    return phm;
}
//撞击函数获得的参数时一般性的GameObiect,而非他们所期望的精确的派生类，所以在函数顶端做一个转化，保持具体真实性
void SpaceShip::hitSpaceShip(GameObject&  spaceShip){
    SpaceShip& otherShip=dynamic_cast<SpaceShip&>(spaceShip);
    process a SpaceShip-SpaceShip collision;
}

void SpaceShip::hitSpaceStation(GameObject&  spaceStation){
    SpaceStation& station=dynamic_cast<SpaceStation&>(spaceStation);
    process a SpaceShip-SpaceStation collision;
}

void SpaceShip::hitAsteroid(GameObject&  asteroid){
    Asteroid& theAsteroid = dynamic_cast<Asteroid&>(asteroid);
    process a SpaceShip-Asteroid collision;
}//在hitSpaceShip这样的函数中，需要将参数对象使用dynamic_cast强制转换为真正的类型。
```

## 5.使用“非成员函数”的碰撞处理函数

实际上，上面这种自行实现vtbl的解法依然有类似的问题，因为针对每一个兄弟类，都需要有一个成员函数处理碰撞。一旦有新的GameObject类型加入到这个游戏中，还是需要修改类的定义。

如果关系型数组内含的指针指向的是non-member functions，重新编译的问题便可消除：

```c++
#include"SpaceShip.h"
#include"SpaceStation.h"
#include"Asteroid.h"
namespcae //这里使用了匿名namespace，匿名namespace内的所有东西对其所在的编译单元而言都是私有的，也就是说，其效果好像是在文件中将函数声明为static一样，更推荐使用匿名namespace。
{
    //主要的碰撞处理函数
    void shipAsteroid(GameObject& spaceShip, GameObject& asteroid);
    void shipStation(GameObject& spaceShip, GameObject& spaceStation);
    void asteroidStation(GameObject& asteroid, GameObject& spaceStation);
    //次要的碰撞处理函数，为了实现对称性，仅仅调用了主要的碰撞处理函数
    void asteroidShip(GameObject& asteroid, GameObject& spaceShip)
    { shipAsteroid(spaceShip, asteroid); }
    void stationShip(GameObject& spaceStation, GameObject& spaceShip)
    { shipStation(spaceShip, spaceStation); }
    void stationAsteroid(GameObject& spaceStation, GameObject& asteroid)
    { asteroidStation(asteroid, spaceStation); }

    typedef void (*HitFunctionPtr)(GameObject&, GameObject&);
    typedef map< pair<string,string>, HitFunctionPtr > HitMap;
    pair<string,string> makeStringPair(const char *s1, const char *s2);
    HitMap * initializeCollisionMap();
    HitFunctionPtr lookup(const string& class1, const string& class2);
}
 
void processCollision(GameObject& object1, GameObject& object2){
    HitFunctionPtr phf = lookup(typeid(object1).name(), typeid(object2).name());
    if (phf) phf(object1, object2);
    else throw UnknownCollision(object1, object2);
}
```

剩下的代码如下：

```c++
namespace  {
    pair<string,string> makeStringPair(const char *s1, const char *s2)
    { return pair<string,string>(s1, s2);  }
}

namespace  {
    HitMap * initializeCollisionMap() {
        HitMap *phm = new HitMap;
        (*phm)[makeStringPair("SpaceShip","Asteroid")] = &shipAsteroid;
        (*phm)[makeStringPair("SpaceShip", "SpaceStation")] = &shipStation;
        ...
        return phm;
    }
}

namespace  {
    HitFunctionPtr lookup(const string& class1, const string& class2) {
        static unique_ptr<HitMap> collisionMap(initializeCollisionMap());

        HitMap::iterator mapEntry = collisionMap->find(make_pair(class1, class2));
        if (mapEntry == collisionMap->end()) return 0;
        return (*mapEntry).second;
    }
}
//由于makeStringPair,initializerCollisionMap和lookup都被声明与一个匿名的namespace中，所以他们的实现必须位于相同的namespace中。（和他们的声明驻在同一编译单元）
```

现在，一旦有新的GameObject子类加进这个继承体系中，原有的类不再需要重新编译，也不需维护纠葛混乱的“以 RTTI 为基础的”switch 或 if-then-else。如果新的类加入到GameObject的继承体系中，只要其本身定义良好；我们的系统只需在initializeCollisionMap 内调整代码，并在“与processCollision相应的那个匿名namespace”内增加新碰撞处理函数。

还有一个问题，如果需要满足inheritance-based类型转换，比如现在宇宙飞船需要区分商业宇宙飞船和军事宇宙飞船：SpaceShip现在派生出了CommercialShip和MilitaryShip，而他们与原有对象的碰撞处理与SpaceShip完全相同，因此如果有一个MilitaryShip和一个Asteroid碰撞，我们希望调用的是void shipAsteroid(GameObject& spaceShip, GameObject& asteroid)。但是就目前的代码而言，这种情况下实际上会抛出一个UnknownCollision异常，因为lookup中会根据”MilitaryShip”和”Asteroid”寻找对应的函数，而map中没有这样的函数。

没有什么简单的办法解决这个问题，如果需要实现double-dispatching，而且需要支持 inheritance-based 参数转换，只能使用最早介绍的“双虚拟函数调用”机制。

## 6.新的collisionMap

以上的代码中，collisionMap是静态的，一旦初始化便不再改动，如果需要动态处理，也就是能增加、删除、修改collisionMap中内容，则需要重新设计一个CollisionMap类：

```c++
class CollisionMap {
public:
    typedef void (*HitFunctionPtr)(GameObject&, GameObject&);
    void addEntry(const string& type1, const string& type2, 
                HitFunctionPtr collisionFunction, bool symmetric = true);

    void removeEntry(const string& type1, const string& type2);
    HitFunctionPtr lookup(const string& type1, const string& type2);

    static CollisionMap& theCollisionMap();//返回一个reference，代表仅有一个map
private:
    CollisionMap(); //这些函数是私有的，防止产生多个maps
    CollisionMap(const CollisionMap&);
};

void shipAsteroid(GameObject& spaceShip, GameObject& asteroid);
CollisionMap::theCollisionMap().addEntry("SpaceShip", "Asteroid", &shipAsteroid);

void shipStation(GameObject& spaceShip, GameObject& spaceStation);
CollisionMap::theCollisionMap().addEntry("SpaceShip", "SpaceStation", &shipStation);

void asteroidStation(GameObject& asteroid, GameObject& spaceStation);
CollisionMap::theCollisionMap().addEntry("Asteroid", "SpaceStation", &asteroidStation);
...
```

addEntry的symmetric参数，主要是为了能对称处理条目而设，也就是增加<T1,T2>时也会增加<T2,T1>。

为了确保这些map条目在其对应的任何撞击发生**之前就被加入map**，可以使用RegisterCollisionFunction类：

```c++
class RegisterCollisionFunction {
public:
    RegisterCollisionFunction(const string& type1, const string& type2,
            CollisionMap::HitFunctionPtr collisionFunction, bool symmetric = true)
    {
        CollisionMap::theCollisionMap().addEntry(type1, type2, 
                    collisionFunction, symmetric);
    }
};

RegisterCollisionFunction cf1("SpaceShip", "Asteroid", &shipAsteroid);
RegisterCollisionFunction cf2("SpaceShip", "SpaceStation", &shipStation);
RegisterCollisionFunction cf3("Asteroid", "SpaceStation", &asteroidStation);
...
int main(int argc, char * argv[])
{
    ...
}
//使用全局对象，保证在main函数之前，就已经将所需的条目加入到了map中。
//稍后如果有新的派生类加入，无需改动原有代码，只需新增代码：
class Satellite: public GameObject { ... };

void satelliteShip(GameObject& satellite, GameObject& spaceShip);
void satelliteAsteroid(GameObject& satellite, GameObject& asteroid);

RegisterCollisionFunction cf4("Satellite", "SpaceShip", &satelliteShip);
RegisterCollisionFunction cf5("Satellite", "Asteroid", &satelliteAsteroid);
```

# 17.在未来时态下发展程序。

程序设计永远在变,仅仅几年前,STL可能还是一个新鲜的概念,而现在则成了C++的必备素质.这一条款的主要目的是为了提醒读者,多为未来做一些额外的考量.而事实证明证明这些努力必定不会白费.
       首先,提供完整的class,即使一些部分目前还用不上.因为对基类的扩展,往往会给派生类带来巨大的影响,所以应当努力将工作提前做到.
       其次,设计接口,使共同的操作易于进行,而阻止共同的错误.C++提供了有力的语言级约束,而提前做了这些约束,必定大有好处.我们不能假定使用者都是合法调用,在这个地方,我们可以极为恶意的揣度使用者的目的.
       尽量将代码泛化,避免雷同的实现.这涉及到修改的问题,避免同时修改多处.

# 18.将非尾端类设计成为抽象类

假设程序有一个Chicken class,Lizard class,Animal class,其中Chicken class和Lizard class继承自Animal class,整个继承体系像这样:

```c++
class Animal {
public:
    Animal& operator=(const Animal& rhs);
    ...
};
class Lizard: public Animal {
public:
    Lizard& operator=(const Lizard& rhs);
    ...
};
class Chicken: public Animal {
public:
    Chicken& operator=(const Chicken& rhs);
    ...
};
//则以下是允许的
Lizard liz1,liz2;
Animal* pAnimal1=&liz1;
Animal* pAnimal2=&liz2;
*pAnimal1=*pAnimal2;
```

最后的调用调用的是Animal class 的copy赋值操作符，即使涉及的对象是Lizard。于是只有派生类的基类部分会被修改。（部分赋值） 最直接的方法是将operator=设为virtual,像这样:

```c++
class Animal {
public:
    virtual Animal& operator=(const Animal& rhs);
    ...
};
class Lizard: public Animal {
public:
    virtual Lizard& operator=(const Animal& rhs);
    ...
};
class Chicken: public Animal {
public:
    virtual Chicken& operator=(const Animal& rhs);
    ...
};                            
```

尽管我们可以定制copy赋值操作符使得返回一个reference代表正确的class，但是这一规则但是这一规则强迫我们在每一个class中为此虚函数声明完全相同的参数类型。意味着派生类的copy赋值必须接受“任何种类的Animal对象出现在赋值操作的右边”，

```c++
Lizard liz;
Chicken chick;
Animal* p1=&liz;
Animal* p2=&chick;
*p1=*p2;//一只鸡赋值给一只蜥蜴（一种异型赋值）
```

希望通过指针进行同型赋值但又希望禁止通过那样的指针进行异型赋值。一种在运行期实现的方法：

```c++
Lizard& Lizard::operator=(const Animal& rhs)
{
    //将rhs转为const Lizard&,失败则抛出异常
    const Lizard& rhs_liz = dynamic_cast<const Lizard&>(rhs);
    //proceed with a normal assignment of rhs_liz to *this;
}
```

  由于同型赋值实际上不需要dynamic_cast,因此opeator=的实现可以升级成这样:.

```c++
class Lizard: public Animal {
public:
    virtual Lizard& operator=(const Animal& rhs);
    Lizard& operator=(const Lizard& rhs); //新增的non-virtual函数
    ...
};
Lizard& Lizard::operator=(const Animal& rhs)
{
    return operator=(dynamic_cast<const Lizard&>(rhs));
}

Lizard liz1,liz2;
liz1=liz2;//同型赋值，调用 Lizard& operator=(const Lizard& rhs)
Animal *p1=&liz1;
Animal *p2=&liz2;
*p1=*p2;//异型赋值，调用 virtual Lizard& operator=(const Animal& rhs)
```

以上策略仍有缺点，应该想办法阻止愚蠢的赋值情况。

 阻止类似于*pAnimal1=*pAnimal2的行为的最直接办法就是将Animal的operator=设为private,但这样一来Animal的自身赋值也不允许,而且Lizard和Chicken的operator=也无法实现,因为它们默认调用Animal的operator=,即使将Animal的operator=设为protected,前一个问题仍然存在.最简单的办法就是消除Animal对象相互赋值的需要,也就是将Animal设为抽象基类,但又由于Animal可能必须作为一个具体类,因此另一个策略就是使Lizard和Chicken不再继承自Animal,并使得Lizard,Chicken,Animal继承自一个更痛的抽象基类AbstactAnimal,整个继承体系像这样:

```c++
class AbstractAnimal {
protected:
    AbstractAnimal& operator=(const AbstractAnimal& rhs);
public:
    virtual ~AbstractAnimal() = 0; // 虽然AbstractAnimal的析构函数被设为pure virtual,但仍然需要提供定义
    ...
};
class Animal: public AbstractAnimal {
public:
    Animal& operator=(const Animal& rhs);
    ...
};
class Lizard: public AbstractAnimal {
public:
    Lizard& operator=(const Lizard& rhs);
    ...
}
class Chicken: public AbstractAnimal {
public:
    Chicken& operator=(const Chicken& rhs);
    ...
};
```

部分赋值和异型赋值禁止，派生类的赋值操作可以调用基类的赋值操作。

**此条款提出一种鉴定抽象类的时机：当你发现需要让一个具体类继承自另一个具体类时。eg:如果有两个具体类c1,c2,希望c2以public方式继承C1,将双类继承体系改为三类集成体系，产生一个抽象类A，令C1,C2以public方式继承自A。**

# 19.在同一个程序中结合C和C++

C++和C混合使用的前提之一就是编译器产生兼容的目标文件(.lib和.dll等).所谓"兼容",指的是编译器在"预编译器相依的特性上"一致,如int和double大小,参数压栈机制等,只有在这个基础上才能讨论结合使用C++和C模块的问题.

## 1.Name Mangling(名称重整)

Name mangling是C++用于支持函数重载的机制,它对重载的函数名称进行一定改变,使得每个函数具有独一无二的名称,由于C不支持函数重载,因此也就不需要name mangling.

如果C++要调用C函数库的某个函数,如果不抑制name mangling,经由name mangling后的函数名称可能与C函数库的原函数名称不匹配,因此就会导致链接失败.解决方法就是使用extern C阻止name mangling,像这样:

```c++
extern "C" 
void simulate(int iterations);
// 这样C++编译器在编译C++文件时就会避免对simulate进行name mangling,转而采用和C相同的命名方式.extern "C"只对接下来的一个函数有效,如果要同时为多个函数使用extern "C",可以使用中括号:
extern "C" { 
    void drawLine(int x1, int y1, int x2, int y2);
    void twiddleBits(unsigned char bits);
    void simulate(int iterations);
    ...
}
//有时代码既有可能被C++使用也有可能被C使用:当用于C++时,可能需要使用extern "C",而用于C时不需要,可以利用C++预定义的的__cplusplus宏使得文件可以"通用于数种语言",像这样:
#ifdef __cplusplus
extern "C"{
#endif
    void drawLine(int x1, int y1, int x2, int y2);
    void twiddleBits(unsigned char bits);
    void simulate(int iterations);
    ...
#ifdef __cplusplus
}
#endif
//如果要结合使用C,assembler,Fortran,LISP,Forth等的函数库,也需要抑制name mangling而使用extern "C".并没有像extern "Fortran"这样的用法
```

## 2.statics的初始化

程序的入口实际上并不是main,而是编译器提供的特殊函数(如 mainCRTStartup(void)等,在mainCRTStartup主要任务之一就是全局对象(包括static class对象,全局对象,namespace作用域内的对象以及文件作用域内的对象)的初始化工作,由于C++支持动态初始化(如全局变量int b=a;)而C仅支持静态初始化，因此全局对象的动态初始化涉及到动态初始化就应该在C++中撰写main,而将C main重命名为realMain,然后让C++ main调用realMain,像这样:

```c++
extern "C" // 以C语言完成此函数
int realMain(int argc, char *argv[]); // C程序库的函数,用于完成主函数功能
int main(int argc, char *argv[]) // C++代码
{
return realMain(argc, argv);
}
```

## 3.动态内存分配

 C动态内存分配和释放使用malloc(及其变种)和free函数,只涉及内存的分配和释放;而C++使用new和delete关键字,并自动调用对象的构造函数.malloc(及其变种)/free搭配使用,new/delete搭配使用,不能混淆.

## 4.数据结构兼容

尽量将两个语言的“数据结构传递”限制于C所能了解的形式；c++ struct如果含有非虚函数，另讨论。

C和C++对于struct和内置类型变量是兼容的,因为C/C++对于struct的内存布局相同,但如果C++为struct加上非虚成员函数,由于struct内存布局不改变,其对象仍兼容于C，但如果加上虚函数,由于vtbl和vptr的存在,struct内存布局便发生改变,也就不再兼容于C.

  也就是说,在C和C++之间对数据结构做双向交流是安全的——前提是结构的定义式在C和C++中都可编译;如果为C++ struct加上非虚函数,虽然不再兼容于C,但可能不影响兼容性;其他改变如虚函数和继承等则会产生影响.

```c++
Lizard liz;
Chicken chick;
Animal* p1=&liz;
Animal* p2=&chick;
*p1=*p2;//一只鸡赋值给一只蜥蜴（一种异型赋值）
```
















































































































































































































































































































































































































































































































































































