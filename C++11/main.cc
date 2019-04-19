#include <iostream>
#include <string>

using namespace std;

//完美转发
void Fun(int& x)
{ 
	cout << "lvalue ref" << endl;
}
void Fun(int&& x) 
{
	cout << "rvalue ref" << endl; 
}
void Fun(const int& x)
{
	cout << "const lvalue ref" << endl; 
}
void Fun(const int&& x) 
{
	cout << "const rvalue ref" << endl; 
}

template <class T>
void PerfectForward(T &&n)
{
	Fun(std::forward<T>(n));
}


struct Goods
{
	string _name;
	double _price;
};

#include <algorithm>
#include <cstdio>

class Rate
{
public:
	Rate(double rate)
		: _rate(rate)
	{}
	double operator()(double money, int year)
	{
		return money * _rate* year;
	}
private:
	double _rate;
};
int main()
{
	//Goods gds[] = { { "苹果", 2.1 }, { "香蕉", 3.1 }, { "橙子", 2.2 }, {"菠萝", 1.5} };

	//lambda表达式
	//sort(gds, gds + sizeof(gds) / sizeof(gds[0]), [](const Goods & g1, const Goods & g2)
	//	{
	//		return g1._price > g2._price;
	//	});

	int a = 20;
	int b = 20;
	//lambda表达式
	[] {}; //最简单的表达式  没有意义
	//auto fun1 = [=] {return 1 + 2; };	
	// 省略了返回值类型，无返回值类型
	auto fun1 = [&](int c) {b = a + c; };
	fun1(10);
	cout << a << " " << b << endl;

	//各部分完善的表达式
	auto fun2=[=, &b](int c)->int {return b += a + c; }; 
	cout << fun2(10) << endl;

	// 复制捕捉x
	int x = 10;
	auto add_x = [x](int a) mutable { x *= 2; return a + x; };
	cout << add_x(10) << endl;

	//函数对象和lambda表达式
	double d = 3.14;
	Rate r1(d);
	//仿函数
	r1(1000, 2);
	
	auto fun = [=](int n, int day)-> double {return d * n* day; };
	cout << fun(2,10) << endl;

	return 0;
}

#if 0
int main()
{
	int a = 10;
	//int&& rr = a;  无法将左值绑定到右值引用
	const int& lr2 = a;

	int&& rr1 = 10;
	int&& rr2 = move(a);  //move实现左值能够被右值引用

	const int lr3 = rr2;  //const类型的左值引用能够将引用右值

	//cout << rr2 << " " << lr3 << endl;

	const int b = 20;
	PerfectForward(10);
	PerfectForward(rr1);
	PerfectForward(lr2);
	PerfectForward(move(b));

	return 0;
}
#endif
