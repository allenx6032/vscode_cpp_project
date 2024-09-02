/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include <cstdio>
#include <stdio.h>
#include <iostream>

#include <stdio.h>
#ifdef _MSC_VER
#define snprintf _snprintf
#endif


#include "fflua.h"
using namespace ff;
class base_t
{
public:
    base_t():v(789){}
	void dump()
	{
		printf("in %s a:%d\n", __FUNCTION__, v);
	}
	int v;
};
class foo_t: public base_t
{
public:
	foo_t(int b):a(b)
	{
		printf("in %s b:%d this=%p\n", __FUNCTION__, b, this);
	}
	~foo_t()
	{
		printf("in %s\n", __FUNCTION__);
	}
	void print(int64_t a, base_t* p) const
	{
		printf("in foo_t::print a:%ld p:%p\n", (long)a, p);
	}
	static void dumy()
	{
		printf("in %s\n", __FUNCTION__);
	}
	int a;
};
//! lua talbe 可以自动转换为stl 对象
void dumy(map<string, string> ret, vector<int> a, list<string> b, set<int64_t> c)
{
    printf("in %s begin ------------\n", __FUNCTION__);
	for (map<string, string>::iterator it =  ret.begin(); it != ret.end(); ++it)
	{
		printf("map:%s, val:%s:\n", it->first.c_str(), it->second.c_str());
	}
	printf("in %s end ------------\n", __FUNCTION__);
}
class clazz{
public:
    static void static_func(){
        printf("in clazz::%s end ------------\n", __FUNCTION__);
    }
};
static void lua_reg(lua_State* ls)
{
    //! 注册基类函数, ctor() 为构造函数的类型
	fflua_register_t<base_t, ctor()>(ls, "base_t")  //! 注册构造函数
					.def(&base_t::dump, "dump")     //! 注册基类的函数
					.def(&base_t::v, "v");          //! 注册基类的属性
    //! 注册子类，ctor(int) 为构造函数， foo_t为类型名称， base_t为继承的基类名称
	fflua_register_t<foo_t, ctor(int)>(ls, "foo_t", "base_t")
				.def(&foo_t::print, "print")        //! 子类的函数
				.def(&foo_t::a, "a");               //! 子类的字段
	fflua_register_t<>(ls)
				.def(&dumy, "dumy");                //! 注册静态函数
    fflua_register_t<clazz, ctor()>(ls, "clazz")
				.def(&clazz::static_func, "static_func"); 
}

#include "ELuna.h"
//define a class
class CPPClass
{
public:
	CPPClass(const char* name): m_name(name){
		printf("%s %p Constructor!\n", name, this);
	}
	~CPPClass(){
		printf("%s %p Destructor!\n", m_name, this);
	}
	//define method
	void cppPrint(const char* word) {
		printf("%s: %s\n", m_name, word);
	}
	int cppSum(int a, int b){
		return a + b;
	}
	void print() {
		printf("%s: %p\n", m_name, this);
	}
	CPPClass& createRef(CPPClass& p) {
		printf("%s %s %p %p createRef!\n", m_name, p.m_name, &p, this);
		p.m_name = "ref";
		return p;
	} 
private:
	const char* m_name;
};
//define function
void cppPrint(char* str) {
	printf("cppPrint: %s\n", str);
}
int cppSum(int a, int b){
	return a + b;
}
CPPClass* testnil(CPPClass* c)
{
    printf("CPPClass c: %p\n", c);
    return nullptr;
}
void testCPP1(lua_State* L) {
	//register a class and it's constructor. indicate all constructor's param type
	ELuna::registerClass<CPPClass>(L, "CPPClass", ELuna::constructor<CPPClass, const char* >);
	//register a method
	ELuna::registerMethod<CPPClass>(L, "cppPrint", &CPPClass::cppPrint);
	ELuna::registerMethod<CPPClass>(L, "cppSum", &CPPClass::cppSum);
	ELuna::registerMethod<CPPClass, CPPClass&>(L, "createRef", &CPPClass::createRef);
	ELuna::registerMethod<CPPClass>(L, "print", &CPPClass::print);
	//register a function
	ELuna::registerFunction(L, "cppPrint", cppPrint);
	ELuna::registerFunction(L, "cppSum", cppSum);
    ELuna::registerFunction(L, "testnil", testnil);
}
void testLua1(lua_State* L) {
	//new a LuaFunction object to bind lua function. indicate return value type
	ELuna::LuaFunction<void> luaPrint(L, "luaPrint");
	ELuna::LuaFunction<int> luaSum(L, "luaSum");
	//run Luafunction's () to call lua function
	luaPrint("hello world");
	printf("luaSum: %d\n", luaSum(1,2));
	//register a lua table
	ELuna::LuaTable luaTable(L, "luaTable");
	//set table's key and value
	luaTable.set(2, "world");
	printf("LuaTable: %s %s\n", luaTable.get<int, char*>(1), luaTable.get<int, char*>(2));
}
const char *fileName = "sample.lua";
struct TestObj
{
	TestObj(const char* name): m_name(name){};
	~TestObj(){};
	void print(){printf("hello world: %s [%p]\n", m_name, this);};
	void changeName(const char* name) {m_name = name;};
	const char* m_name;
};
class Tester
{
public:
	Tester() { 
		printf(" Tester0 Constructor!\n");
	}
	~Tester() { printf(" Tester0 Destructor!\n");}
	void foo0() {
		printf("foo0: \n");
	}
	void foo1(int p1) {
		printf("foo1: %d\n", p1);
	}
	void foo2(int p1, int p2) {
		printf("foo2: %d %d\n", p1, p2);
	}
	void foo3(int p1, int p2, int p3) {
		printf("foo3: %d %d %d\n", p1, p2, p3);
	}
	void foo4(int p1, int p2, int p3, int p4) {
		printf("foo4: %d %d %d %d\n", p1, p2, p3, p4);
	}
	void foo5(int p1, int p2, int p3, int p4, int p5) {
		printf("foo5: %d %d %d %d %d\n", p1, p2, p3, p4, p5);
	}
	void foo6(int p1, int p2, int p3, int p4, int p5, int p6) {
		printf("foo6: %d %d %d %d %d %d\n", p1, p2, p3, p4, p5, p6);
	}
	void foo7(int p1, int p2, int p3, int p4, int p5, int p6, int p7) {
		printf("foo7: %d %d %d %d %d %d %d\n", p1, p2, p3, p4, p5, p6, p7);
	}
	void foo8(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8) {
		printf("foo8: %d %d %d %d %d %d %d %d\n", p1, p2, p3, p4, p5, p6, p7, p8);
	}
	void foo9(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9) {
		printf("foo9: %d %d %d %d %d %d %d %d %d\n", p1, p2, p3, p4, p5, p6, p7, p8, p9);
	}
	int retFoo0() {
		return 0;
	}
	int retFoo1(int p1) {
		return p1;
	}
	int retFoo2(int p1, int p2) {
		return p1 + p2;
	}
	int retFoo3(int p1, int p2, int p3) {
		return p1 + p2 + p3;
	}
	int retFoo4(int p1, int p2, int p3, int p4) {
		return p1 + p2 + p3 + p4;
	}
	int retFoo5(int p1, int p2, int p3, int p4, int p5) {
		return p1 + p2 + p3 + p4 + p5;
	}
	int retFoo6(int p1, int p2, int p3, int p4, int p5, int p6) {
		return p1 + p2 + p3 + p4 + p5 + p6;
	}
	int retFoo7(int p1, int p2, int p3, int p4, int p5, int p6, int p7) {
		return p1 + p2 + p3 + p4 + p5 + p6 + p7;
	}
	int retFoo8(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8) {
		return p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8;
	}
	int retFoo9(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9) {
		return p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
	}
	bool testBool(bool p) {
		return p;
	}
	char* testPChar(char* p) {
		return p;
	}
	const char* testPConstChar(const char* p) {
		return p;
	}
	char testChar(char p) {
		return p;
	}
	unsigned char testUChar(unsigned char p) {
		return p;
	}
	short testShort(short p) {
		return p;
	}
	unsigned short testUShort(unsigned short p) {
		return p;
	}
	long testLong(long p) {
		return p;
	}
	unsigned long testULong(unsigned long p) {
		return p;
	}
	unsigned int testUInt(unsigned int p) {
		return p;
	}
	long long testLongLong(long long p) {
		return p;
	}
	unsigned long long testULongLong(unsigned long long p) {
		return p;
	}
	float testFloat(float p) {
		return p;
	}
	double testDouble(double p) {
		return p;
	}
	ELuna::LuaString testLuaString(ELuna::LuaString p) {
		return p;
	}
	std::string testStdString(std::string p) {
		return p;
	}
	ELuna::LuaTable testLuaTable(ELuna::LuaTable p) {
		p.set(2, "world");
		printf("has key: %d\n", p.has("world"));
		printf("has key: %d\n", p.has(1));
		printf("has key: %d\n", p.has(2));
		printf("has key: %d\n", p.has(100));
		printf("get value: %s\n", p.get<int, const char*>(1));
		printf("get value: %s\n", p.get<int, const char*>(2));

		return p;
	}
	TestObj testObj(TestObj p) {
		p.m_name = "TestObj Change";
		return p;
	}
	TestObj& testObjRef(TestObj& p) {
		p.m_name = "TestObjRef Changed";
		return p;
	}
	void testObjPointer(TestObj* p) {
		p->m_name = "TestObjPointer Changed";
	}
	TestObj* testObjPointer1(TestObj* p) {
		p->m_name = "TestObjPointer Changed";
		p = new TestObj("fuck pointer");  // will memory leak, need delete. c new, c delete
		return p;
	}
	TestObj* testObjPointer2(TestObj* p) {
		p->m_name = "TestObjPointer Changed";
		return p;
	}
	const TestObj* testObjPointer3(TestObj* p) {
		return p;
	}
	const TestObj* testObjPointer4(const TestObj* p) {
		return p;
	}
private:
};
class Test1
{
public:
	Test1(int p1) { 
		printf(" Test1 Constructor! %d\n", p1);
	}
	~Test1() { printf(" Test1 Destructor!\n");}
};
class Test2
{
public:
	Test2(int p1, int p2) { 
		printf(" Test2 Constructor! %d %d\n", p1, p2);
	}
	~Test2() { printf(" Test2 Destructor!\n");}
};
class Test3
{
public:
	Test3(int p1, int p2, int p3) { 
		printf(" Test3 Constructor! %d %d %d\n", p1, p2, p3);
	}
	~Test3() { printf(" Test3 Destructor!\n");}
};
class Test4
{
public:
	Test4(int p1, int p2, int p3, int p4) { 
		printf(" Test4 Constructor! %d %d %d %d\n", p1, p2, p3, p4);
	}
	~Test4() { printf(" Test4 Destructor!\n");}
};
class Test5
{
public:
	Test5(int p1, int p2, int p3, int p4, int p5) { 
		printf(" Test5 Constructor! %d %d %d %d %d\n", p1, p2, p3, p4, p5);
	}
	~Test5() { printf(" Test5 Destructor!\n");}
};
class Test6
{
public:
	Test6(int p1, int p2, int p3, int p4, int p5, int p6) { 
		printf(" Test6 Constructor! %d %d %d %d %d %d\n", p1, p2, p3, p4, p5, p6);
	}
	~Test6() { printf(" Test6 Destructor!\n");}
};
class Test7
{
public:
	Test7(int p1, int p2, int p3, int p4, int p5, int p6, int p7) { 
		printf(" Test7 Constructor! %d %d %d %d %d %d %d\n", p1, p2, p3, p4, p5, p6, p7);
	}
	~Test7() { printf(" Test7 Destructor!\n");}
};
class Test8
{
public:
	Test8(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8) { 
		printf(" Test8 Constructor! %d %d %d %d %d %d %d %d\n", p1, p2, p3, p4, p5, p6, p7, p8);
	}
	~Test8() { printf(" Test8 Destructor!\n");}
};
class Test9
{
public:
	Test9(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9) { 
		printf(" Test9 Constructor! %d %d %d %d %d %d %d %d %d\n", p1, p2, p3, p4, p5, p6, p7, p8, p9);
	}
	~Test9() { printf(" Test9 Destructor!\n");}
};
void foo0() {
	printf("foo0: \n");
}
void foo1(int p1) {
	printf("foo1: %d\n", p1);
}
void foo2(int p1, int p2) {
	printf("foo2: %d %d\n", p1, p2);
}
void foo3(int p1, int p2, int p3) {
	printf("foo3: %d %d %d\n", p1, p2, p3);
}
void foo4(int p1, int p2, int p3, int p4) {
	printf("foo4: %d %d %d %d\n", p1, p2, p3, p4);
}
void foo5(int p1, int p2, int p3, int p4, int p5) {
	printf("foo5: %d %d %d %d %d\n", p1, p2, p3, p4, p5);
}
void foo6(int p1, int p2, int p3, int p4, int p5, int p6) {
	printf("foo6: %d %d %d %d %d %d\n", p1, p2, p3, p4, p5, p6);
}
void foo7(int p1, int p2, int p3, int p4, int p5, int p6, int p7) {
	printf("foo7: %d %d %d %d %d %d %d\n", p1, p2, p3, p4, p5, p6, p7);
}
void foo8(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8) {
	printf("foo8: %d %d %d %d %d %d %d %d\n", p1, p2, p3, p4, p5, p6, p7, p8);
}
void foo9(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9) {
	printf("foo9: %d %d %d %d %d %d %d %d %d\n", p1, p2, p3, p4, p5, p6, p7, p8, p9);
}
int retFoo0() {
	return 0;
}
int retFoo1(int p1) {
	return p1;
}
int retFoo2(int p1, int p2) {
	return p1 + p2;
}
int retFoo3(int p1, int p2, int p3) {
	return p1 + p2 + p3;
}
int retFoo4(int p1, int p2, int p3, int p4) {
	return p1 + p2 + p3 + p4;
}
int retFoo5(int p1, int p2, int p3, int p4, int p5) {
	return p1 + p2 + p3 + p4 + p5;
}
int retFoo6(int p1, int p2, int p3, int p4, int p5, int p6) {
	return p1 + p2 + p3 + p4 + p5 + p6;
}
int retFoo7(int p1, int p2, int p3, int p4, int p5, int p6, int p7) {
	return p1 + p2 + p3 + p4 + p5 + p6 + p7;
}
int retFoo8(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8) {
	return p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8;
}
int retFoo9(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9) {
	return p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
}
TestObj retObj(TestObj p) {
	p.m_name = "RetObj Change";
	return p;
}
TestObj& retObjRef(TestObj& p) {
	p.m_name = "RetObjRef Changed";
	return p;
}
TestObj* retObjPointer1(TestObj* p) {
	p->m_name = "RetObjPointer Changed";
	p = new TestObj("fuck pointer");  // will memory leak, need delete. c new, c delete
	return p;
}
TestObj* retObjPointer(TestObj* p) {
	p->m_name = "RetObjPointer Changed";
	return p;
}
void testCPP2(lua_State* L) {
	//register a class and it's constructor. indicate all constructor's param type
	ELuna::registerClass<Tester>(L, "Tester0", ELuna::constructor<Tester>);
	ELuna::registerClass<TestObj>(L, "TestObj", ELuna::constructor<TestObj, const char*>);
	//register a method
	ELuna::registerMethod<Tester>(L, "foo0", &Tester::foo0);
	ELuna::registerMethod<Tester>(L, "foo1", &Tester::foo1);
	ELuna::registerMethod<Tester>(L, "foo2", &Tester::foo2);
	ELuna::registerMethod<Tester>(L, "foo3", &Tester::foo3);
	ELuna::registerMethod<Tester>(L, "foo4", &Tester::foo4);
	ELuna::registerMethod<Tester>(L, "foo5", &Tester::foo5);
	ELuna::registerMethod<Tester>(L, "foo6", &Tester::foo6);
	ELuna::registerMethod<Tester>(L, "foo7", &Tester::foo7);
	ELuna::registerMethod<Tester>(L, "foo8", &Tester::foo8);
	ELuna::registerMethod<Tester>(L, "foo9", &Tester::foo9);
	ELuna::registerMethod<Tester>(L, "retFoo0", &Tester::retFoo0);
	ELuna::registerMethod<Tester>(L, "retFoo1", &Tester::retFoo1);
	ELuna::registerMethod<Tester>(L, "retFoo2", &Tester::retFoo2);
	ELuna::registerMethod<Tester>(L, "retFoo3", &Tester::retFoo3);
	ELuna::registerMethod<Tester>(L, "retFoo4", &Tester::retFoo4);
	ELuna::registerMethod<Tester>(L, "retFoo5", &Tester::retFoo5);
	ELuna::registerMethod<Tester>(L, "retFoo6", &Tester::retFoo6);
	ELuna::registerMethod<Tester>(L, "retFoo7", &Tester::retFoo7);
	ELuna::registerMethod<Tester>(L, "retFoo8", &Tester::retFoo8);
	ELuna::registerMethod<Tester>(L, "retFoo9", &Tester::retFoo9);
	ELuna::registerMethod<TestObj>(L, "print", &TestObj::print);
	ELuna::registerMethod<TestObj>(L, "changeName", &TestObj::changeName);
	//Tester read2cpp and push2lua
	ELuna::registerMethod<Tester>(L, "testBool", &Tester::testBool);
	ELuna::registerMethod<Tester>(L, "testChar", &Tester::testChar);
	ELuna::registerMethod<Tester>(L, "testDouble", &Tester::testDouble);
	ELuna::registerMethod<Tester>(L, "testFloat", &Tester::testFloat);
	ELuna::registerMethod<Tester>(L, "testLong", &Tester::testLong);
	ELuna::registerMethod<Tester>(L, "testLongLong", &Tester::testLongLong);
	ELuna::registerMethod<Tester>(L, "testLuaString", &Tester::testLuaString);
	ELuna::registerMethod<Tester>(L, "testStdString", &Tester::testStdString);
	ELuna::registerMethod<Tester>(L, "testPChar", &Tester::testPChar);
	ELuna::registerMethod<Tester>(L, "testPConstChar", &Tester::testPConstChar);
	ELuna::registerMethod<Tester>(L, "testShort", &Tester::testShort);
	ELuna::registerMethod<Tester>(L, "testUChar", &Tester::testUChar);
	ELuna::registerMethod<Tester>(L, "testUInt", &Tester::testUInt);
	ELuna::registerMethod<Tester>(L, "testULong", &Tester::testULong);
	ELuna::registerMethod<Tester>(L, "testULongLong", &Tester::testULongLong);
	ELuna::registerMethod<Tester>(L, "testUShort", &Tester::testUShort);
	ELuna::registerMethod<Tester>(L, "testLuaTable", &Tester::testLuaTable);
	ELuna::registerMethod<Tester>(L, "testObjPointer", &Tester::testObjPointer);
	ELuna::registerMethod<Tester>(L, "testObjPointer1", &Tester::testObjPointer1);
	ELuna::registerMethod<Tester>(L, "testObjPointer2", &Tester::testObjPointer2);
	ELuna::registerMethod<Tester>(L, "testObjPointer3", &Tester::testObjPointer3);
	ELuna::registerMethod<Tester>(L, "testObjPointer4", &Tester::testObjPointer4);
	ELuna::registerMethod<Tester>(L, "testObj", &Tester::testObj);
	ELuna::registerMethod<Tester>(L, "testObjRef", &Tester::testObjRef);
	//register a function
	ELuna::registerFunction(L, "cppFoo0", &foo0);
	ELuna::registerFunction(L, "cppFoo1", &foo1);
	ELuna::registerFunction(L, "cppFoo2", &foo2);
	ELuna::registerFunction(L, "cppFoo3", &foo3);
	ELuna::registerFunction(L, "cppFoo4", &foo4);
	ELuna::registerFunction(L, "cppFoo5", &foo5);
	ELuna::registerFunction(L, "cppFoo6", &foo6);
	ELuna::registerFunction(L, "cppFoo7", &foo7);
	ELuna::registerFunction(L, "cppFoo8", &foo8);
	ELuna::registerFunction(L, "cppFoo9", &foo9);
	ELuna::registerFunction(L, "cppRetFoo0", &retFoo0);
	ELuna::registerFunction(L, "cppRetFoo1", &retFoo1);
	ELuna::registerFunction(L, "cppRetFoo2", &retFoo2);
	ELuna::registerFunction(L, "cppRetFoo3", &retFoo3);
	ELuna::registerFunction(L, "cppRetFoo4", &retFoo4);
	ELuna::registerFunction(L, "cppRetFoo5", &retFoo5);
	ELuna::registerFunction(L, "cppRetFoo6", &retFoo6);
	ELuna::registerFunction(L, "cppRetFoo7", &retFoo7);
	ELuna::registerFunction(L, "cppRetFoo8", &retFoo8);
	ELuna::registerFunction(L, "cppRetFoo9", &retFoo9);
	ELuna::registerFunction(L, "cppRetObj", &retObj);
	ELuna::registerFunction(L, "cppRetObjRef", &retObjRef);
	ELuna::registerFunction(L, "cppRetObjPointer", &retObjPointer);
	ELuna::registerFunction(L, "cppRetObjPointer1", &retObjPointer1);
	//test constructor
	ELuna::registerClass<Test1>(L, "Test1", ELuna::constructor<Test1, int>);
	ELuna::registerClass<Test2>(L, "Test2", ELuna::constructor<Test2, int, int>);
	ELuna::registerClass<Test3>(L, "Test3", ELuna::constructor<Test3, int, int, int>);
	ELuna::registerClass<Test4>(L, "Test4", ELuna::constructor<Test4, int, int, int, int>);
	ELuna::registerClass<Test5>(L, "Test5", ELuna::constructor<Test5, int, int, int, int, int>);
	ELuna::registerClass<Test6>(L, "Test6", ELuna::constructor<Test6, int, int, int, int, int, int>);
	ELuna::registerClass<Test7>(L, "Test7", ELuna::constructor<Test7, int, int, int, int, int, int, int>);
	ELuna::registerClass<Test8>(L, "Test8", ELuna::constructor<Test8, int, int, int, int, int, int, int, int>);
	ELuna::registerClass<Test9>(L, "Test9", ELuna::constructor<Test9, int, int, int, int, int, int, int, int, int>);
}
void testLua2(lua_State* L) {
	printf("------------testLua------------\n");
	ELuna::LuaFunction<void> foo0(L, "foo0");
	ELuna::LuaFunction<void> foo1(L, "foo1");
	ELuna::LuaFunction<void> foo2(L, "foo2");
	ELuna::LuaFunction<void> foo3(L, "foo3");
	ELuna::LuaFunction<void> foo4(L, "foo4");
	ELuna::LuaFunction<void> foo5(L, "foo5");
	ELuna::LuaFunction<void> foo6(L, "foo6");
	ELuna::LuaFunction<void> foo7(L, "foo7");
	ELuna::LuaFunction<void> foo8(L, "foo8");
	ELuna::LuaFunction<void> foo9(L, "foo9");
	ELuna::LuaFunction<int> retFoo0(L, "retFoo0");
	ELuna::LuaFunction<int> retFoo1(L, "retFoo1");
	ELuna::LuaFunction<int> retFoo2(L, "retFoo2");
	ELuna::LuaFunction<int> retFoo3(L, "retFoo3");
	ELuna::LuaFunction<int> retFoo4(L, "retFoo4");
	ELuna::LuaFunction<int> retFoo5(L, "retFoo5");
	ELuna::LuaFunction<int> retFoo6(L, "retFoo6");
	ELuna::LuaFunction<int> retFoo7(L, "retFoo7");
	ELuna::LuaFunction<int> retFoo8(L, "retFoo8");
	ELuna::LuaFunction<int> retFoo9(L, "retFoo9");
	ELuna::LuaFunction<TestObj*> luaTestObjPointer(L, "luaTestObjPointer");
	ELuna::LuaFunction<TestObj&> luaTestObjRef(L, "luaTestObjRef");
	ELuna::LuaFunction<TestObj> luaTestObj(L, "luaTestObj");
	foo0();
	foo1(1);
	foo2(1,2);
	foo3(1,2,3);
	foo4(1,2,3,4);
	foo5(1,2,3,4,5);
	foo6(1,2,3,4,5,6);
	foo7(1,2,3,4,5,6,7);
	foo8(1,2,3,4,5,6,7,8);
	foo9(1,2,3,4,5,6,7,8,9);
	printf("retFoo0: %d\n", retFoo0());
	printf("retFoo1: %d\n", retFoo1(1));
	printf("retFoo2: %d\n", retFoo2(1,2));
	printf("retFoo3: %d\n", retFoo3(1,2,3));
	printf("retFoo4: %d\n", retFoo4(1,2,3,4));
	printf("retFoo5: %d\n", retFoo5(1,2,3,4,5));
	printf("retFoo6: %d\n", retFoo6(1,2,3,4,5,6));
	printf("retFoo7: %d\n", retFoo7(1,2,3,4,5,6,7));
	printf("retFoo8: %d\n", retFoo8(1,2,3,4,5,6,7,8));
	printf("retFoo9: %d\n", retFoo9(1,2,3,4,5,6,7,8,9));
	printf("luaTestObjPointer: \n");
	TestObj pObj = TestObj("TestObjPointer");
	TestObj* retPObj = luaTestObjPointer(&pObj);
	pObj.print();
	retPObj->print();
	printf("luaTestObjRef1: \n");
	TestObj objRef1("TestObjRef1");
	TestObj& retObjRef1 = luaTestObjRef(&objRef1);
	objRef1.print();
	retObjRef1.print();
	printf("luaTestObjRef2: \n");
	TestObj objRef2("TestObjRef2");
	TestObj& retObjRef2 = luaTestObjRef(objRef2);
	objRef2.print();
	retObjRef2.print();
	printf("luaTestObj: \n");
	TestObj obj("TestObj");
	TestObj retObj = luaTestObj(obj);
	obj.print();
	retObj.print();
	printf("pass refrence to luaFunction the same as object!\n");
}
#ifndef MINI_LUA_H
#include "MiniLua.hpp"
#endif
/**
 * This C++-function can be called from Lua
 */
static int test_function(lua_State *L) {
    if (not L)
        return 0;
    double number{lua_tonumber(L, 1)};
    number *= 2;
    lua_pushnumber(L, number);
    return 1;
}

#include <luaMagic.hpp>
#include <cstring>
#include <iostream>

// #include "SDL.h"
#include "hellorogue.c"
#include "SDL_main.h"


int luaMagic_load(lua_State* L);
double add(double x, double y);
void upper(const char* str);
void hello(const char* str, int x, double y);
void commandLine(lua_State *L);

#if __WIN32
int SDL_main(int argc, char *argv[])
#else
int main(int argc, char** argv)
#endif
{

fflua_t fflua;
    try 
    {
        fflua.setModFuncFlag(true);
        //! 注册C++ 对象到lua中
        fflua.reg(lua_reg);
        //! 载入lua文件
        fflua.add_package_path("./");
#ifdef _WIN32
        fflua.load_file("test.lua");
#else
        fflua.load_file("test.lua");
#endif
        //! 获取全局变量
        int var = 0;
        assert(0 == fflua.get_global_variable("test_var", var));
        //! 设置全局变量
        assert(0 == fflua.set_global_variable("test_var", ++var));
        //! 执行lua 语句
        fflua.run_string("print(\"exe run_string!!\")");
        //! 调用lua函数, 基本类型作为参数
        int32_t arg1 = 1;
        float   arg2 = 2;
        double  arg3 = 3;
        string  arg4 = "4";
		fflua.call<int32_t>("test_func", arg1, arg2, arg3, arg4);
        fflua.call<bool>("Mod:funcTest1", arg1, arg2);
        //! 调用lua函数，stl类型作为参数， 自动转换为lua talbe
        vector<int> vec;        vec.push_back(100);
        list<float> lt;         lt.push_back((float)99.99);
        set<string> st;         st.insert("OhNIce");
        map<string, int> mp;    mp["key"] = 200;
        fflua.call<string>("test_stl", vec, lt, st,  mp);
        //! 调用lua 函数返回 talbe，自动转换为stl结构
        vec = fflua.call<vector<int> >("test_return_stl_vector");
        lt  = fflua.call<list<float> >("test_return_stl_list");
        st  = fflua.call<set<string> >("test_return_stl_set");
        mp  = fflua.call<map<string, int> >("test_return_stl_map");
        //! 调用lua函数，c++ 对象作为参数, foo_t 必须被注册过
        foo_t* foo_ptr = new foo_t(456);
        fflua.call<void>("test_object", foo_ptr);
        //! 调用lua函数，c++ 对象作为返回值, foo_t 必须被注册过 
        assert(foo_ptr == fflua.call<foo_t*>("test_ret_object", foo_ptr));
        //! 调用lua函数，c++ 对象作为返回值, 自动转换为基类
        base_t* base_ptr = fflua.call<base_t*>("test_ret_base_object", foo_ptr);
        assert(base_ptr == foo_ptr);
    }
    catch (exception& e)
    {
        printf("exception:%s\n", e.what());
    }
#ifdef _WIN32
    system("pause");
#endif

	lua_State *L = ELuna::openLua();
	testCPP1(L);
	testCPP2(L);
	ELuna::doFile(L, fileName);
	getchar();
	testLua1(L);
	getchar();
	testLua2(L);
	getchar();
	ELuna::closeLua(L);
	getchar();



	MiniLua lua{};
    lua.Push(test_function, "test_function");
    lua.Load("testMinilua.lua");
    // get int
    int width{0};
        lua.Get("width", width);
    std::cout << "width: " << width << std::endl;
    // get double
    double number{0};
    if(lua.Get("number", number))
        std::cout << "number: " << number << std::endl;
    // get float
    float flt{0.0};
    if(lua.Get("float", flt))
        std::cout << "float: " << flt << std::endl;
    // get string
    std::string title{"empty"};
    if(lua.Get("title", title))
        std::cout << "title: " << title << std::endl;
    // get boolean
    bool boolean{false};
    if(lua.Get("fullscreen", boolean))
        std::cout << "fullscreen: " << (boolean ? "true" : "false") << std::endl;
    // three function arguments
    int result{0};
    int args[] = {1,2,3};
    if (lua.Call("Sum", 3, args, result))
        std::cout << "1+2+3 = " << result << std::endl;
    // null function arguments, no return value
    lua.Call("Print_string");
    // one argument, no return value
    lua.Call("Power", 2);
    std::cout << "Lua top: " << lua_gettop(lua.GetState()) << std::endl;
	getchar();

   	lua_State *_l = ELuna::openLua();
	luaopen_rogue_core(_l);
	ELuna::doFile(L, "test_sdl.lua");
	ELuna::closeLua(L);
	getchar();

 	/* 创建lua_State */
    lua_State* ls = luaL_newstate();
    /* 创建失败，退出 */
    if(ls == NULL)
    {
        fprintf(stderr,"%s : Cannot create state.\n",argv[0]);
        return 1;
    }
    /* 打开所有Lua标准库 */
    luaL_openlibs(ls);
	/* 绑定函数 */
	luaMagic_load(ls);
    /* 运行交互式命令行 */
    commandLine(ls);
	getchar();
	return 0;
}


int luaMagic_load(lua_State* L)
{
	luaMagic::bind(L, "add", add);
	luaMagic::bind(L, "upper", upper);
	luaMagic::bind(L, "hello", hello);
}

double add(double x, double y)
{
	return x+y;
}

void upper(const char* str)
{
	for(int i = 0 ; i < strlen(str); i++)
	{
		putchar(str[i] & ~(1<<5));
	}
	putchar('\n');
}

void hello(const char* str, int x, double y)
{
	printf("%s %d %lf\n", str, x, y);
}

void commandLine(lua_State *L)
{
	while (1) 
	{
	char cmd[256];
	printf(">> ");
	std::cin.get(cmd,256);
	getchar();
	luaL_dostring(L, cmd);
	}
}