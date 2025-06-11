#include <stdio.h>
#include <stdlib.h>
#include "euluna.hpp"
#include <iostream>

// double mathex_lerp(double a, double b, double t) {
//     return a + (b-a)*t;
// }
// EULUNA_BEGIN_SINGLETON("mathex")
// EULUNA_FUNC_NAMED("lerp", mathex_lerp)
// EULUNA_END()


// std::string string_concat(const std::string& a, const std::string& b) {
//     return a + b;
// }
// EULUNA_BEGIN_GLOBAL_FUNCTIONS(myglobals)
// EULUNA_FUNC_NAMED("concat", string_concat)
// EULUNA_END()

// namespace stringutil {
// std::string concat(const std::string& a, const std::string& b) {
//     return a + b;
// }
// }
// EULUNA_BEGIN_SINGLETON(stringutil)
// EULUNA_SINGLETON_FUNC_NAMED("concat", stringutil::concat);
// EULUNA_END()

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

// class Dummy {
// public:
//     void setBoo(const std::string& boo) { m_boo = boo; }
//     std::string getBoo() const { return m_boo; }
//
// private:
//     std::string m_boo;
// };
// called just once when lua starts to hold a reference to this object
// void __handleDummyUse(EulunaInterface* lua, Dummy *dummy) {
//     // nothing to do, object will be deleted on the release handler
// }
// called just once when the lua holds no reference to this object anymore
// void __handleDummyRelease(EulunaInterface* lua, Dummy *dummy) {
//     // release object lua table, must always be called before deleting
//     lua->releaseObject(dummy);
//     // delete object memory
//     delete dummy;
// }
// EULUNA_BEGIN_MANAGED_CLASS(Dummy)
// EULUNA_CLASS_REFERENCE_HANDLERS(__handleDummyUse, __handleDummyRelease)
// EULUNA_CLASS_STATIC_NAMED("new", []{ return new Dummy; })
// EULUNA_CLASS_MEMBER(Dummy, setBoo)
// EULUNA_CLASS_MEMBER(Dummy, getBoo)
// EULUNA_END()


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


int main()
{
    // Initialize euluna engine using a new lua state
    EulunaEngine euluna(luaL_newstate());
    // Register C++ bindings to lua
    EulunaBinder::registerGlobalBindings(&euluna);
    // Example of calling a C++ function from lua and returning its result back to C++
    double res = euluna.runBuffer<double>("foo.setBoo('hello world!') \nprint(foo.getBoo()) -- hello world!\nlocal t = Triangle.new()\nt:setValues(2, 3)\nprint(t:getArea()) -- 3\nlocal r = Rectangle.new()\nr:setValues(2, 3)\nprint(r:getArea()) -- 6");
    // Outputs 5
    std::cout << res << std::endl;
    return 0;
}