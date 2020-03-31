#pragma once
#include <iostream>
#include<functional>
using namespace std;
function<int(int n1, int n2)> funAddnum;
int addnum(int n1, int n2)
{
	return n1 + n2;
}

function<int(int n1, int n2)> sumnum = [](int n1, int n2)->int {return n1 - n2; };

class Singleton
{
private:
	Singleton() {};
	Singleton(const Singleton&) {};
	Singleton& operator=(const Singleton&) {}
public:
	static Singleton& getInstance()
	{
		static Singleton instance;
		return instance;
	}
};
unsigned int __stdcall threadDemo(LPVOID) // void *
{
	Singleton::getInstance();
	return 0;
}
class Product1 {
public:
	virtual void process() = 0;
};
class ProductA1 :public Product1 {
	void process() { cout << "drvProductA pro" << endl; }
};
class ProductB1 :public Product1 {
	void process() { cout << "drvProductA1 pro" << endl; }
};
class Product2 {
public:
	virtual void process() = 0;
};
class ProductA2 :public Product2 {
	void process() { cout << "DrvProductB pro" << endl; }
};
class ProductB2 :public Product2 {
	void process() { cout << "DrvProductB1 pro" << endl; }
};
class Factory {
	virtual Product1* Create1() = 0;
	virtual Product2* Create2() = 0;
};
class FactoryA :public Factory {
	Product1* Create1() { return new ProductA1; }
	Product2* Create2() { return new ProductA2; }
};
class FactoryB :public Factory {
	Product1* Create1() { return new ProductB1; }
	Product2* Create2() { return new ProductB2; }
};
