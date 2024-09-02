#include <iostream>
#include <string>
#include <assert.h>
using namespace std;

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



#include <iostream>
#include "lua_tinker.h"

static int cpp_int = 100;

int cpp_func(int arg1, int arg2)
{
	return arg1 + arg2;
}

struct A
{
	A(int v) : value(v) {}
	int value;
};

struct base
{
	base() {}
	const char* is_base(){ return "this is base"; }
};

class test : public base
{
public:
	test(int val) : _test(val) {}
	~test() {}
	const char* is_test(){ return "this is test"; }
	void ret_void() {}
	int ret_int()				{ return _test;			}
	int ret_mul(int m) const	{ return _test * m;		}
	A get()						{ return A(_test);		}
	void set(A a)				{ _test = a.value;		}
	int _test;
};

test g_test(11);

// 窃荐 屈怕啊 int(*)(lua_State*) 肚绰 int(*)(lua_State*,T1) 老 版快父 lua_yield() 甫 镜 荐 乐促.
// 窃荐 牢磊啊 歹 鞘夸且 版快 lua_tinker.h 狼 "functor (non-managed)" 内膏飘 何盒阑 曼绊秦辑 流立 眠啊且 巴
int TestFunc(lua_State* L)
{
	printf("# TestFunc 角青吝\n");
	return lua_yield(L, 0);
}

int TestFunc2(lua_State* L, float a)
{
	printf("# TestFunc2(L,%f) 角青吝\n", a);
	return lua_yield(L, 0);
}

class TestClass
{
public:

	// 窃荐 屈怕啊 int(T::*)(lua_State*) 肚绰 int(T::*)(lua_State*,T1) 老 版快父 lua_yield() 甫 镜 荐 乐促.
	// 窃荐 牢磊啊 歹 鞘夸且 版快 lua_tinker.h 狼 "class member functor (non-managed)" 内膏飘 何盒阑 曼绊秦辑 流立 眠啊且 巴
	int TestFunc(lua_State* L)
	{
		printf("# TestClass::TestFunc 角青吝\n");
		return lua_yield(L, 0);
	}
	int TestFunc2(lua_State* L, float a)
	{
		printf("# TestClass::TestFunc2(L,%f) 角青吝\n", a);
		return lua_yield(L, 0);
	}
};

void show_error(const char* error)
{
	printf("_ALERT -> %s\n", error);
}

int main(int argc, char* argv[])
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

    // Lua 甫 檬扁拳 茄促.
	lua_State* L = lua_open();
	// Lua 扁夯 窃荐甸阑 肺靛茄促.- print() 荤侩
	luaopen_base(L);
	// LuaTinker 甫 捞侩秦辑 窃荐甫 殿废茄促.
	lua_tinker::def(L, "cpp_func", cpp_func);
	// sample1.lua 颇老阑 肺靛/角青茄促.
	lua_tinker::dofile(L, "sample1.lua");
	// sample1.lua 狼 窃荐甫 龋免茄促.
	int result = lua_tinker::call<int>(L, "lua_func", 3, 4);
	// lua_func(3,4) 狼 搬苞拱 免仿
	printf("lua_func(3,4) = %d\n", result);
	// 橇肺弊伐 辆丰
	lua_close(L);
    getchar();

    L = lua_open();
	// Lua 扁夯 窃荐甸阑 肺靛茄促.- print() 荤侩
	luaopen_base(L);
	// LuaTinker 甫 捞侩秦辑 cpp_int 甫 Lua俊 傈崔
	lua_tinker::set(L, "cpp_int", cpp_int);
	// sample1.lua 颇老阑 肺靛/角青茄促.
	lua_tinker::dofile(L, "sample2.lua");
	// sample1.lua 狼 窃荐甫 龋免茄促.
	int lua_int = lua_tinker::get<int>(L, "lua_int");
	// lua_int 蔼 免仿
	printf("lua_int = %d\n", lua_int);
	// 橇肺弊伐 辆丰
	lua_close(L);
    getchar();

    L = lua_open();
	// Lua 扁夯 窃荐甸阑 肺靛茄促.- print() 荤侩
	luaopen_base(L);
	// Lua 巩磊凯 窃荐甸阑 肺靛茄促.- string 荤侩
	luaopen_string(L);
	// base 努贰胶甫 Lua 俊 眠啊茄促.
	lua_tinker::class_add<base>(L, "base");
	// base 狼 窃荐甫 殿废茄促.
	lua_tinker::class_def<base>(L, "is_base", &base::is_base);
	// test 努贰胶甫 Lua 俊 眠啊茄促.
	lua_tinker::class_add<test>(L, "test");
	// test 啊 base 俊 惑加 罐疽澜阑 舅妨霖促.
	lua_tinker::class_inh<test, base>(L);
	// test 努贰胶 积己磊甫 殿废茄促.
	lua_tinker::class_con<test>(L, lua_tinker::constructor<test,int>);
	// test 狼 窃荐甸阑 殿废茄促.
	lua_tinker::class_def<test>(L, "is_test", &test::is_test);
	lua_tinker::class_def<test>(L, "ret_void", &test::ret_void);
	lua_tinker::class_def<test>(L, "ret_int", &test::ret_int);
	lua_tinker::class_def<test>(L, "ret_mul", &test::ret_mul);
	lua_tinker::class_def<test>(L, "get", &test::get);
	lua_tinker::class_def<test>(L, "set", &test::set);
	lua_tinker::class_mem<test>(L, "_test", &test::_test);
	// Lua 傈开 函荐龋 g_test 狼 器牢磐甫 殿废茄促.
	lua_tinker::set(L, "g_test", &g_test);
	// sample3.lua 颇老阑 肺靛/角青茄促.
	lua_tinker::dofile(L, "sample3.lua");
	// 橇肺弊伐 辆丰
	lua_close(L);
    getchar();

    L = lua_open();
	// Lua 扁夯 窃荐甸阑 肺靛茄促.- print() 荤侩
	luaopen_base(L);
	// Lua 抛捞喉阑 积己窍绊 胶琶俊 仟浆茄促.
	lua_tinker::table haha(L, "haha");
	// haha.value = 1 蔼阑 持绰促.
	haha.set("value", 1);
	// table 郴俊 table阑 父甸绢 持绰促.
	haha.set("inside", lua_tinker::table(L));
	// haha.inside 狼 器牢磐甫 胶琶困肺 汗荤茄促.
	lua_tinker::table inside = haha.get<lua_tinker::table>("inside");
	// inside.value = 2 蔼阑 持绰促.
	inside.set("value", 2);
	// sample4.lua 颇老阑 肺靛/角青茄促.
	lua_tinker::dofile(L, "sample4.lua");
	// Lua 俊辑 积己茄 haha.test 蔼阑 佬绰促.
	const char* test = haha.get<const char*>("test");
	printf("haha.test = %s\n", test);
	// 傈开俊 殿废窍瘤 臼绊 Lua 胶琶俊 后 抛捞喉阑 积己茄促.(瘤开函荐)
	lua_tinker::table temp(L);
	// 后 抛捞喉.name 俊 蔼阑 持绰促.
	temp.set("name", "local table !!");
	// table阑 狼 牢磊肺 荤侩窍咯 print_table 阑 龋免茄促.
	lua_tinker::call<void>(L, "print_table", temp);
	// 窃荐啊 府畔窍绰 table阑 罐绰促.
	lua_tinker::table ret = lua_tinker::call<lua_tinker::table>(L, "return_table", "give me a table !!");
	printf("ret.name =\t%s\n", ret.get<const char*>("name"));
	// 橇肺弊伐 辆丰
	lua_close(L);
    getchar();

    L = lua_open();
	// Lua 扁夯 窃荐甸阑 肺靛茄促.- print() 荤侩
	luaopen_base(L);
	// lua_State* 啊 爱绊乐绰 泅犁狼 胶琶阑 焊咯霖促.
	printf("%s\n","-------------------------- current stack");
	lua_tinker::enum_stack(L);
	// 胶琶俊 1阑 剐绢持绰促.
	lua_pushnumber(L, 1);
	// 泅犁 胶琶狼 郴侩阑 促矫 免仿茄促.
	printf("%s\n","-------------------------- stack after push '1'");
	lua_tinker::enum_stack(L);
	// sample5.lua 颇老阑 肺靛/角青茄促.
	lua_tinker::dofile(L, "sample5.lua");
	// test_error() 窃荐甫 龋免茄促.
	// test_error() 绰 角青吝 test_error_3() 龋免阑 矫档窍促 俊矾甫 惯积矫挪促.
	// 窃荐 龋免吝 惯积茄 俊矾绰 printf()甫 烹秦辑 免仿等促.
	printf("%s\n","-------------------------- calling test_error()");
	lua_tinker::call<void>(L, "test_error");
	// test_error_3()绰 粮犁窍瘤 臼绰 窃荐捞促. 龋免 磊眉啊 角菩茄促.
	printf("%s\n","-------------------------- calling test_error_3()");
	lua_tinker::call<void>(L, "test_error_3");
	// printf() 措脚 蜡历啊 力傍窍绰 俊矾 免仿 风凭阑 荤侩且 荐 乐促.
	// 捞 俊矾贸府 窃荐绰1俺狼 风酒 巩磊凯肺 惯积茄 俊矾甫 傈崔窍霸 等促.
	// C++ 俊辑 殿废且 版快 void function(const char*) 屈怕啊 利钦窍促.
	lua_tinker::def(L, "_ALERT", show_error);
	lua_tinker::call<void>(L, "_ALERT", "test !!!");
	// test_error() 窃荐甫 龋免茄促.
	// 窃荐 龋免吝 惯积茄 俊矾绰 Lua俊 殿废等 _ALERT()甫 烹秦辑 免仿等促.
	printf("%s\n","-------------------------- calling test_error()");
	lua_tinker::call<void>(L, "test_error");
	// 橇肺弊伐 辆丰
	lua_close(L);
    getchar();

    L = lua_open();
	// Lua 扁夯 窃荐甸阑 肺靛茄促.- print() 荤侩
	luaopen_base(L);
	// Lua 巩磊凯 窃荐甸阑 肺靛茄促.- string 荤侩
	luaopen_string(L);
	// TestFunc 窃荐甫 Lua 俊 殿废茄促.
	lua_tinker::def(L, "TestFunc", &TestFunc);
	lua_tinker::def(L, "TestFunc2", &TestFunc2);
	// TestClass 努贰胶甫 Lua 俊 眠啊茄促.
	lua_tinker::class_add<TestClass>(L, "TestClass");
	// TestClass 狼 窃荐甫 殿废茄促.
	lua_tinker::class_def<TestClass>(L, "TestFunc", &TestClass::TestFunc);
	lua_tinker::class_def<TestClass>(L, "TestFunc2", &TestClass::TestFunc2);
	// TestClass 甫 傈开 函荐肺 急攫茄促.
	TestClass g_test;
	lua_tinker::set(L, "g_test", &g_test);
	// sample3.lua 颇老阑 肺靛茄促.
	lua_tinker::dofile(L, "sample6.lua");
	// Thread 甫 矫累茄促.
	lua_newthread(L);
	lua_pushstring(L, "ThreadTest");
	lua_gettable(L, LUA_GLOBALSINDEX);
	// Thread 甫 矫累茄促.
	printf("* lua_resume() 龋免\n");
	lua_resume(L, 0);
	// Thread 甫 促矫 矫累茄促.
	printf("* lua_resume() 龋免\n");
	lua_resume(L, 0);
	// Thread 甫 促矫 矫累茄促.
	printf("* lua_resume() 龋免\n");
	lua_resume(L, 0);
	// Thread 甫 促矫 矫累茄促.
	printf("* lua_resume() 龋免\n");
	lua_resume(L, 0);
	// Thread 甫 促矫 矫累茄促.
	printf("* lua_resume() 龋免\n");
	lua_resume(L, 0);
	// 橇肺弊伐 辆丰
	lua_close(L);
    getchar();
    return 0;
}
