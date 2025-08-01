#include <gtest.h>
#include "euluna.hpp"
#include <iostream>

typedef unsigned int uint; 

EulunaBinder& g_binder = EulunaBinder::instance();
EulunaEngine& g_lua = EulunaEngine::instance();

TEST(DemanglerTest, BasicTypes) {
    EXPECT_EQ("int", euluna_tools::demangle_type<int>());
    EXPECT_EQ("bool", euluna_tools::demangle_type<bool>());
}

TEST(DemanglerTest, Classes) {
    //EXPECT_EQ("EulunaSharedObject", EulunaSharedObject().euluna_getCppClassName());
    //EXPECT_EQ("std::string", euluna_tools::demangle_type<std::string>());
    EXPECT_EQ(EulunaRuntimeError().name(), "EulunaRuntimeError");
    EulunaRuntimeError derivedException;
    EulunaException& e = derivedException;
    EXPECT_EQ(e.name(), "EulunaRuntimeError");
    EXPECT_EQ(euluna_tools::demangle_type(e), "EulunaRuntimeError");
}

TEST(EulunaBinder, RegisterBindings)
{
    g_binder.registerBindings(&g_lua);
    EXPECT_EQ(g_lua.stackSize(), 0);
}

////////////////////
void gnop() {
    // do nothing
}

std::string gconcat(const std::string& a, int b) {
    return a+std::to_string(b);
}

int lua_gmul(EulunaInterface* lua) {
    double a = lua->popNumber();
    double b = lua->popNumber();
    lua->pushNumber(a*b);
    return 1;
}

EULUNA_BEGIN_GLOBAL_FUNCTIONS(myglobals)
EULUNA_FUNC(gconcat)
EULUNA_FUNC(gnop)
EULUNA_FUNC_NAMED("gdiv", [](float a, float b) { return a/b; })
EULUNA_FUNC_NAMED("gmul", lua_gmul)
EULUNA_END()

TEST(EulunaBinder, Globals) {
    EXPECT_EQ(g_lua.runBuffer<std::string>("return gconcat(1,2)"), std::string("12"));
    EXPECT_EQ(g_lua.runBuffer<float>("return gdiv(10,5)"), 2.0f);
    EXPECT_EQ(g_lua.runBuffer<double>("return gmul(3,50)"), 150.0);
    EXPECT_EQ(g_lua.stackSize(), 0);
}

////////////////////
EULUNA_BEGIN_SINGLETON(test)
EULUNA_FUNC(gconcat)
EULUNA_FUNC(gnop)
EULUNA_FUNC_NAMED("div", [](float a, float b) { return a/b; })
EULUNA_FUNC_NAMED("mul", lua_gmul)
EULUNA_END()

class Test {
public:
    static std::string concat(const std::string& a, int b) { return a+std::to_string(b); }
    static int lua_mul(EulunaInterface* lua) {
        double a = lua->popNumber();
        double b = lua->popNumber();
        lua->pushNumber(a*b);
        return 1;
    }
    static void nop() { }
    void mynop() { }
    uint setFoo(const std::string& foo) { m_foo = foo; return 0xdeadbeaf; }
    std::string getMyFoo() const { return m_foo; }
private:
    std::string m_foo;
};
Test g_test;

EULUNA_BEGIN_SINGLETON_CLASS_NAMED("g_test", Test, &g_test)
EULUNA_CLASS_STATIC(Test, concat)
EULUNA_CLASS_STATIC(Test, nop)
EULUNA_CLASS_STATIC_NAMED("mul", Test, lua_mul)
EULUNA_CLASS_MEMBER(Test, mynop)
EULUNA_CLASS_MEMBER(Test, setFoo)
EULUNA_CLASS_MEMBER_NAMED("getFoo", Test, getMyFoo)
EULUNA_END()

TEST(EulunaBinder, Singletons) {
    EXPECT_EQ(g_lua.runBuffer<std::string>("return test.gconcat(1,2)"), "12");
    EXPECT_EQ(g_lua.runBuffer<float>("return test.div(10,5)"), 2.0f);
    EXPECT_EQ(g_lua.runBuffer<double>("return test.mul(3,50)"), 150.0);
    EXPECT_EQ(g_lua.runBuffer<std::string>("return g_test.concat(1,2)"), "12");
    EXPECT_EQ(g_lua.runBuffer<double>("return g_test.mul(3,50)"), 150.0);
    EXPECT_EQ(g_lua.runBuffer<uint>("return g_test.setFoo('boo')"), 0xdeadbeaf);
    EXPECT_EQ(g_lua.runBuffer<std::string>("return g_test.getFoo()"), "boo");
    EXPECT_EQ(g_lua.stackSize(), 0);
}


/////////
TEST(EulunaBinder, StdTypes) {
    EXPECT_EQ(g_lua.runBuffer<std::vector<int>>("return {1,2,3}"), std::vector<int>({1,2,3}));
    EXPECT_EQ(g_lua.runBuffer<std::deque<int>>("return {1,2,3}"), std::deque<int>({1,2,3}));
    EXPECT_EQ(g_lua.runBuffer<std::list<int>>("return {1,2,3}"), std::list<int>({1,2,3}));
    EXPECT_EQ(g_lua.runBuffer<std::set<int>>("return {1,2,3}"), std::set<int>({1,2,3}));
    EXPECT_EQ(g_lua.runBuffer<std::unordered_set<int>>("return {1,2,3}"), std::unordered_set<int>({1,2,3}));

    std::map<std::string,int> map;
    map["a"] = 1;
    map["b"] = 2;
    map["c"] = 3;
    EXPECT_EQ(g_lua.runBuffer<decltype(map)>("return {a=1,b=2,c=3}"), map);

    std::unordered_map<std::string,int> umap;
    umap["a"] = 1;
    umap["b"] = 2;
    umap["c"] = 3;
    EXPECT_EQ(g_lua.runBuffer<decltype(umap)>("return {a=1,b=2,c=3}"), umap);

    std::tuple<std::string,int> tuple("lala",2);
    EXPECT_EQ(g_lua.runBuffer<decltype(tuple)>("return {'lala',2}"), tuple);

    std::function<int(int,int)> sum;
    sum = g_lua.runBuffer<decltype(sum)>("return function(a,b) return a+b end");
    EXPECT_EQ(sum(1,2), 3);
}

///////////////////////////
class Dummy {
public:
    Dummy() { m_dummyCounter++; }
    ~Dummy() { m_dummyCounter--; }

    Dummy(const Dummy&) = delete;
    Dummy& operator=(const Dummy&) = delete;

    void setBoo(const std::string& boo) { m_boo = boo; }
    std::string getBoo() const { return m_boo; }

    static Dummy* create() { return new Dummy; }
    void destroy() {
        g_lua.releaseObject(this);
        delete this;
    }

    static int getDummyCounter() { return m_dummyCounter; }

private:
    std::string m_boo;
    static int m_dummyCounter;
};
int Dummy::m_dummyCounter = 0;

void __handleDummyUse(EulunaInterface* lua, Dummy *dummy) {
    // nothing to do
}
void __handleDummyRelease(EulunaInterface* lua, Dummy *dummy) {
    dummy->destroy();
}

EULUNA_BEGIN_MANAGED_CLASS(Dummy)
EULUNA_CLASS_REFERENCE_HANDLERS(__handleDummyUse, __handleDummyRelease)
EULUNA_CLASS_STATIC(Dummy, create)
EULUNA_CLASS_MEMBER(Dummy, destroy)
EULUNA_CLASS_STATIC_NAMED_EX("new", []{ return new Dummy; })
EULUNA_CLASS_STATIC_NAMED_EX("delete", [](Dummy *dummy) { dummy->destroy(); })
EULUNA_CLASS_MEMBER(Dummy, setBoo)
EULUNA_CLASS_MEMBER(Dummy, getBoo)
EULUNA_END()

TEST(Euluna, ManagedClass) {
    Dummy *dummy = new Dummy;
    g_lua.pushObject(dummy);
    g_lua.pop();
    g_lua.collect();
    EXPECT_EQ(g_lua.stackSize(), 0);
    EXPECT_EQ(Dummy::getDummyCounter(), 0);

    std::string script = R"(
        local res = ''
        local dummy

        dummy = Dummy.create()
        dummy:setBoo("hello")
        res = res .. dummy:getBoo()
        dummy:destroy()
        dummy = nil

        local dummy = Dummy.new()
        dummy:setBoo(" world")
        res = res .. dummy:getBoo()
        Dummy.delete(dummy)
        dummy = nil

        dummy = Dummy.new()
        dummy:setBoo("!")
        res = res .. dummy:getBoo()
        dummy.asd = "!!"
        local b = dummy
        res = res .. b.asd
        b = nil
        dummy = nil

        Dummy.new()
        Dummy.create()

        return res
    )";
    EXPECT_EQ(g_lua.runBuffer<std::string>(script), "hello world!!!");
    g_lua.collect();
    EXPECT_EQ(g_lua.stackSize(), 0);
    EXPECT_EQ(Dummy::getDummyCounter(), 0);
}
///////////////////////////
class DummyBase {
public:
    DummyBase() { m_dummyBaseCounter++; }
    virtual ~DummyBase() { m_dummyBaseCounter--; }

    DummyBase(const Dummy&) = delete;
    DummyBase& operator=(const DummyBase&) = delete;

    void setFoo(const std::string& foo) { m_foo = foo; }
    std::string getFoo() const { return m_foo; }

    static DummyBase* create() { return new DummyBase; }
    void destroy() {
        g_lua.releaseObject(this);
        delete this;
    }

    static int getBaseCounter() { return m_dummyBaseCounter; }

private:
    std::string m_foo;
    static int m_dummyBaseCounter;
};
int DummyBase::m_dummyBaseCounter = 0;

void __handleDummyBaseUse(EulunaInterface* lua, DummyBase *dummy) {
    // nothing to do
}
void __handleDummyBaseRelease(EulunaInterface* lua, DummyBase *dummy) {
    dummy->destroy();
}
EULUNA_BEGIN_MANAGED_CLASS(DummyBase)
EULUNA_CLASS_REFERENCE_HANDLERS(__handleDummyBaseUse, __handleDummyBaseRelease)
EULUNA_CLASS_STATIC(DummyBase, create)
EULUNA_CLASS_MEMBER(DummyBase, destroy)
EULUNA_CLASS_STATIC_NAMED_EX("new", []{ return new DummyBase; })
EULUNA_CLASS_STATIC_NAMED_EX("delete", [](DummyBase *dummy) { dummy->destroy(); })
EULUNA_CLASS_MEMBER(DummyBase, setFoo)
EULUNA_CLASS_MEMBER(DummyBase, getFoo)
EULUNA_END()

class DummyDerived : public DummyBase {
public:
    DummyDerived() { m_dummyCounter++; }
    ~DummyDerived() { m_dummyCounter--; }

    void setBoo(const std::string& boo) { m_boo = boo; }
    std::string getBoo() const { return m_boo; }

    static DummyDerived* create() { return new DummyDerived; }
    static int getDerivedCounter() { return m_dummyCounter; }

private:
    std::string m_boo;
    static int m_dummyCounter;
};
int DummyDerived::m_dummyCounter = 0;

void __handleDummyDerivedUse(EulunaInterface* lua, DummyDerived *dummy) {
    // nothing to do
}
void __handleDummyDerivedRelease(EulunaInterface* lua, DummyDerived *dummy) {
    dummy->destroy();
}
EULUNA_BEGIN_MANAGED_DERIVED_CLASS(DummyDerived, "DummyBase")
EULUNA_CLASS_REFERENCE_HANDLERS(__handleDummyDerivedUse, __handleDummyDerivedRelease)
EULUNA_CLASS_STATIC(DummyDerived, create)
EULUNA_CLASS_STATIC_NAMED_EX("new", []{ return new DummyDerived; })
EULUNA_CLASS_STATIC_NAMED_EX("delete", [](DummyDerived *dummy) { dummy->destroy(); })
EULUNA_CLASS_MEMBER(DummyDerived, setBoo)
EULUNA_CLASS_MEMBER(DummyDerived, getBoo)
EULUNA_END()

TEST(Euluna, ManagedDerivedClass) {
    std::string script = R"(
        local res = ''
        local dummy

        dummy = DummyDerived.create()
        dummy:setBoo("hello")
        res = res .. dummy:getBoo()
        dummy:destroy()
        dummy = nil

        local dummy = DummyBase.new()
        dummy:setFoo(" world")
        res = res .. dummy:getFoo()
        DummyBase.delete(dummy)
        dummy = nil

        dummy = DummyDerived.new()
        dummy:setFoo("!")
        res = res .. dummy:getFoo()
        dummy.asd = "!!"
        local b = dummy
        res = res .. b.asd
        b = nil
        dummy = nil

        DummyDerived.new()
        DummyDerived.create()
        DummyBase.new()
        DummyBase.create()

        return res
    )";
    EXPECT_EQ(g_lua.runBuffer<std::string>(script), "hello world!!!");
    g_lua.collect();
    EXPECT_EQ(g_lua.stackSize(), 0);
    EXPECT_EQ(DummyBase::getBaseCounter(), 0);
    EXPECT_EQ(DummyDerived::getDerivedCounter(), 0);
}

//////////////////////
double mathex_lerp(double a, double b, double t) {
    return a + (b-a)*t;
}

std::string string_concat(const std::string& a, const std::string& b) {
    return a + b;
}

namespace stringutil {
std::string concat(const std::string& a, const std::string& b) {
    return a + b;
}
}

EULUNA_BEGIN_SINGLETON(mathex)
EULUNA_FUNC_NAMED("lerp", mathex_lerp)
EULUNA_END()

EULUNA_BEGIN_GLOBAL_FUNCTIONS(myglobals2)
EULUNA_FUNC_NAMED("concat", string_concat)
EULUNA_END()

EULUNA_BEGIN_SINGLETON(stringutil)
EULUNA_FUNC_NAMED("concat", stringutil::concat);
EULUNA_END()

class Foo {
public:
    static Foo* instance() {
        static Foo instance;
        return &instance;
    }

    void setBoo(const std::string& boo) { m_boo = boo; }
    std::string getBoo() const { return m_boo; }

private:
    std::string m_boo;
};

EULUNA_BEGIN_SINGLETON_CLASS_NAMED("foo", Foo, Foo::instance())
EULUNA_CLASS_MEMBER(Foo, setBoo)
EULUNA_CLASS_MEMBER(Foo, getBoo)
EULUNA_END()

class Polygon {
public:
    virtual ~Polygon() { }
    virtual float getArea() = 0;
    void setValues(float a, float b) { m_width = a; m_height = b; }
protected:
    float m_width = 0, m_height = 0;
};

class Rectangle : public Polygon {
public:
    virtual float getArea() { return m_width * m_height; }
};

class Triangle : public Polygon {
public:
    virtual float getArea() { return (m_width * m_height) / 2.0f; }
};

EULUNA_BEGIN_MANAGED_CLASS(Polygon)
EULUNA_CLASS_MEMBER(Polygon, getArea)
EULUNA_CLASS_MEMBER(Polygon, setValues)
EULUNA_END()

EULUNA_BEGIN_MANAGED_DERIVED_CLASS(Rectangle, "Polygon")
EULUNA_CLASS_STATIC_NAMED_EX("new", []{ return new Rectangle; })
EULUNA_CLASS_GENERIC_REFERENCE_HANDLERS(Rectangle)
EULUNA_CLASS_MEMBER(Rectangle, getArea)
EULUNA_END()

EULUNA_BEGIN_MANAGED_DERIVED_CLASS(Triangle, "Polygon")
EULUNA_CLASS_STATIC_NAMED_EX("new", []{ return new Triangle; })
EULUNA_CLASS_GENERIC_REFERENCE_HANDLERS(Triangle)
EULUNA_CLASS_MEMBER(Triangle, getArea)
EULUNA_END()

TEST(Euluna, Examples) {
    // create a new lua state
    auto L = luaL_newstate();
    // initialize euluna engine using the supplied state
    EulunaEngine euluna(L);
    // register C++ bindings in the supplied lua state
    EulunaBinder::registerGlobalBindings(&euluna);
    // example calling a C++ function from lua then return its value back to C++
    EXPECT_EQ(euluna.runBuffer<double>("return mathex.lerp(0,10,0.5)"), 5.0);
    EXPECT_EQ(euluna.runBuffer<std::string>("return concat('hello',' world!')"), "hello world!");
    EXPECT_EQ(euluna.runBuffer<std::string>("return stringutil.concat('hello',' world!')"), "hello world!");
    EXPECT_EQ(euluna.runBuffer<std::string>("foo.setBoo('hello world!') return foo.getBoo()"), "hello world!");
    EXPECT_EQ(euluna.runBuffer<float>("local a = Rectangle.new(); a:setValues(2,3); return a:getArea()"), 6.0f);
    EXPECT_EQ(euluna.runBuffer<float>("local a = Triangle.new(); a:setValues(2,3); return a:getArea()"), 3.0f);
    euluna.runBuffer(R"(
        function helloWorld(str)
          return 'hello ' .. str
        end
        myfuncs = { hello = helloWorld })"
    );
    EXPECT_EQ(euluna.callGlobal<std::string>("helloWorld", "world!"), "hello world!");
    EXPECT_EQ(euluna.callGlobalField<std::string>("myfuncs", "hello", "world!"), "hello world!");
    EXPECT_EQ(g_lua.stackSize(), 0);
}

TEST(Euluna, CallLuaFunctions) {
    g_lua.runBuffer("function helloWorld() return 'hello world!!!' end");
    g_lua.runBuffer("globals = {}; function globals.helloWorld() return 'hello world!!!' end");
    EXPECT_EQ(g_lua.callGlobal<std::string>("helloWorld"), "hello world!!!");
    EXPECT_EQ(g_lua.callGlobalField<std::string>("globals", "helloWorld"), "hello world!!!");
    EXPECT_EQ(g_lua.stackSize(), 0);
}

//////////////////////

