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

#include <Box2D.h>
///-----includes_start-----
#include "btBulletDynamicsCommon.h"

#include <stdio.h>
#include <iostream>

#include "b2Imgui.h"
#include "b2RenderGL3.h"
#include "b2DebugDraw.h"
#include "b2Test.h"

#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#else
#include <glew/glew.h>
#endif
#include <glfw/glfw3.h>
#include <stdio.h>
#ifdef _MSC_VER
#define snprintf _snprintf
#endif
struct UIState
{
	bool showMenu;
	int scroll;
	int scrollarea1;
	bool mouseOverMenu;
	bool chooseTest;
};
namespace
{
	GLFWwindow* mainWindow = NULL;
	UIState ui;

	int32 testIndex = 0;
	int32 testSelection = 0;
	int32 testCount = 0;
	TestEntry* entry;
	Test* test;
	Settings settings;
	bool rightMouseDown;
	b2Vec2 lastp;
}
static void sCreateUI()
{
	ui.showMenu = true;
	ui.scroll = 0;
	ui.scrollarea1 = 0;
	ui.chooseTest = false;
	ui.mouseOverMenu = false;
	// Init UI
    const char* fontPath = "DroidSans.ttf";
	if (RenderGLInit(fontPath) == false)
	{
		fprintf(stderr, "Could not init GUI renderer.\n");
		// assert(false);
		return;
	}
}
static void sResizeWindow(GLFWwindow*, int width, int height)
{
	g_camera.m_width = width;
	g_camera.m_height = height;
}
static void sKeyCallback(GLFWwindow*, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Quit
			glfwSetWindowShouldClose(mainWindow, GL_TRUE);
			break;
		case GLFW_KEY_LEFT:
			// Pan left
			if (mods == GLFW_MOD_CONTROL)
			{
				b2Vec2 newOrigin(2.0f, 0.0f);
				test->ShiftOrigin(newOrigin);
			}
			else
			{
				g_camera.m_center.x -= 0.5f;
			}
			break;
		case GLFW_KEY_RIGHT:
			// Pan right
			if (mods == GLFW_MOD_CONTROL)
			{
				b2Vec2 newOrigin(-2.0f, 0.0f);
				test->ShiftOrigin(newOrigin);
			}
			else
			{
				g_camera.m_center.x += 0.5f;
			}
			break;
		case GLFW_KEY_DOWN:
			// Pan down
			if (mods == GLFW_MOD_CONTROL)
			{
				b2Vec2 newOrigin(0.0f, 2.0f);
				test->ShiftOrigin(newOrigin);
			}
			else
			{
				g_camera.m_center.y -= 0.5f;
			}
			break;
		case GLFW_KEY_UP:
			// Pan up
			if (mods == GLFW_MOD_CONTROL)
			{
				b2Vec2 newOrigin(0.0f, -2.0f);
				test->ShiftOrigin(newOrigin);
			}
			else
			{
				g_camera.m_center.y += 0.5f;
			}
			break;
		case GLFW_KEY_HOME:
			// Reset view
			g_camera.m_zoom = 1.0f;
			g_camera.m_center.Set(0.0f, 20.0f);
			break;
		case GLFW_KEY_Z:
			// Zoom out
			g_camera.m_zoom = b2Min(1.1f * g_camera.m_zoom, 20.0f);
			break;
		case GLFW_KEY_X:
			// Zoom in
			g_camera.m_zoom = b2Max(0.9f * g_camera.m_zoom, 0.02f);
			break;
		case GLFW_KEY_R:
			// Reset test
			delete test;
			test = entry->createFcn();
			break;
		case GLFW_KEY_SPACE:
			// Launch a bomb.
			if (test)
			{
				test->LaunchBomb();
			}
			break;
		case GLFW_KEY_P:
			// Pause
			settings.pause = !settings.pause;
			break;
		case GLFW_KEY_LEFT_BRACKET:
			// Switch to previous test
			--testSelection;
			if (testSelection < 0)
			{
				testSelection = testCount - 1;
			}
			break;
		case GLFW_KEY_RIGHT_BRACKET:
			// Switch to next test
			++testSelection;
			if (testSelection == testCount)
			{
				testSelection = 0;
			}
			break;
		case GLFW_KEY_TAB:
			ui.showMenu = !ui.showMenu;
		default:
			if (test)
			{
				test->Keyboard(key);
			}
		}
	}
	else if (action == GLFW_RELEASE)
	{
		test->KeyboardUp(key);
	}
}
static void sMouseButton(GLFWwindow*, int32 button, int32 action, int32 mods)
{
	double xd, yd;
	glfwGetCursorPos(mainWindow, &xd, &yd);
	b2Vec2 ps((float32)xd, (float32)yd);
	// Use the mouse to move things around.
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		b2Vec2 pw = g_camera.ConvertScreenToWorld(ps);
		if (action == GLFW_PRESS)
		{
			if (mods == GLFW_MOD_SHIFT)
			{
				test->ShiftMouseDown(pw);
			}
			else
			{
				test->MouseDown(pw);
			}
		}
		if (action == GLFW_RELEASE)
		{
			test->MouseUp(pw);
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_2)
	{
		if (action == GLFW_PRESS)
		{	
			lastp = g_camera.ConvertScreenToWorld(ps);
			rightMouseDown = true;
		}
		if (action == GLFW_RELEASE)
		{
			rightMouseDown = false;
		}
	}
}
static void sMouseMotion(GLFWwindow*, double xd, double yd)
{
	b2Vec2 ps((float)xd, (float)yd);
	b2Vec2 pw = g_camera.ConvertScreenToWorld(ps);
	test->MouseMove(pw);
	if (rightMouseDown)
	{
		b2Vec2 diff = pw - lastp;
		g_camera.m_center.x -= diff.x;
		g_camera.m_center.y -= diff.y;
		lastp = g_camera.ConvertScreenToWorld(ps);
	}
}
static void sScrollCallback(GLFWwindow*, double, double dy)
{
	if (ui.mouseOverMenu)
	{
		ui.scroll = -int(dy);
	}
	else
	{
		if (dy > 0)
		{
			g_camera.m_zoom /= 1.1f;
		}
		else
		{
			g_camera.m_zoom *= 1.1f;
		}
	}
}
static void sRestart()
{
	delete test;
	entry = g_testEntries + testIndex;
	test = entry->createFcn();
}
static void sSimulate()
{
	glEnable(GL_DEPTH_TEST);
	test->Step(&settings);
	test->DrawTitle(entry->name);
	glDisable(GL_DEPTH_TEST);
	if (testSelection != testIndex)
	{
		testIndex = testSelection;
		delete test;
		entry = g_testEntries + testIndex;
		test = entry->createFcn();
		g_camera.m_zoom = 1.0f;
		g_camera.m_center.Set(0.0f, 20.0f);
	}
}
static void sInterface()
{
	int menuWidth = 200;
	ui.mouseOverMenu = false;
	if (ui.showMenu)
	{
		bool over = imguiBeginScrollArea("Testbed Controls", g_camera.m_width - menuWidth - 10, 10, menuWidth, g_camera.m_height - 20, &ui.scrollarea1);
		if (over) ui.mouseOverMenu = true;
		imguiSeparatorLine();
		imguiLabel("Test");
		if (imguiButton(entry->name, true))
		{
			ui.chooseTest = !ui.chooseTest;
		}
		imguiSeparatorLine();
		imguiSlider("Vel Iters", &settings.velocityIterations, 0, 50, 1, true);
		imguiSlider("Pos Iters", &settings.positionIterations, 0, 50, 1, true);
		imguiSlider("Hertz", &settings.hz, 5.0f, 120.0f, 5.0f, true);
		if (imguiCheck("Sleep", settings.enableSleep, true))
			settings.enableSleep = !settings.enableSleep;
		if (imguiCheck("Warm Starting", settings.enableWarmStarting, true))
			settings.enableWarmStarting = !settings.enableWarmStarting;
		if (imguiCheck("Time of Impact", settings.enableContinuous, true))
			settings.enableContinuous = !settings.enableContinuous;
		if (imguiCheck("Sub-Stepping", settings.enableSubStepping, true))
			settings.enableSubStepping = !settings.enableSubStepping;
		imguiSeparatorLine();
		if (imguiCheck("Shapes", settings.drawShapes, true))
			settings.drawShapes = !settings.drawShapes;
		if (imguiCheck("Joints", settings.drawJoints, true))
			settings.drawJoints = !settings.drawJoints;
		if (imguiCheck("AABBs", settings.drawAABBs, true))
			settings.drawAABBs = !settings.drawAABBs;
		if (imguiCheck("Contact Points", settings.drawContactPoints, true))
			settings.drawContactPoints = !settings.drawContactPoints;
		if (imguiCheck("Contact Normals", settings.drawContactNormals, true))
			settings.drawContactNormals = !settings.drawContactNormals;
		if (imguiCheck("Contact Impulses", settings.drawContactImpulse, true))
			settings.drawContactImpulse = !settings.drawContactImpulse;
		if (imguiCheck("Friction Impulses", settings.drawFrictionImpulse, true))
			settings.drawFrictionImpulse = !settings.drawFrictionImpulse;
		if (imguiCheck("Center of Masses", settings.drawCOMs, true))
			settings.drawCOMs = !settings.drawCOMs;
		if (imguiCheck("Statistics", settings.drawStats, true))
			settings.drawStats = !settings.drawStats;
		if (imguiCheck("Profile", settings.drawProfile, true))
			settings.drawProfile = !settings.drawProfile;
		if (imguiButton("Pause", true))
			settings.pause = !settings.pause;
		if (imguiButton("Single Step", true))
			settings.singleStep = !settings.singleStep;
		if (imguiButton("Restart", true))
			sRestart();
		if (imguiButton("Quit", true))
			glfwSetWindowShouldClose(mainWindow, GL_TRUE);
		imguiEndScrollArea();
	}
	int testMenuWidth = 200;
	if (ui.chooseTest)
	{
		static int testScroll = 0;
		bool over = imguiBeginScrollArea("Choose Sample", g_camera.m_width - menuWidth - testMenuWidth - 20, 10, testMenuWidth, g_camera.m_height - 20, &testScroll);
		if (over) ui.mouseOverMenu = true;
		for (int i = 0; i < testCount; ++i)
		{
			if (imguiItem(g_testEntries[i].name, true))
			{
				delete test;
				entry = g_testEntries + i;
				test = entry->createFcn();
				ui.chooseTest = false;
			}
		}
		imguiEndScrollArea();
	}
	imguiEndFrame();
}



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
// This is a simple example of building and running a simulation
// using Box2D. Here we create a large ground box and a small dynamic
// box.
// There are no graphics for this example. Box2D is meant to be used
// with your rendering engine in your game engine.
int main(int argc, char** argv)
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


	B2_NOT_USED(argc);
	B2_NOT_USED(argv);
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -10.0f);
	// Construct a world object, which will hold and simulate the rigid bodies.
	b2World world(gravity);
	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);
	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	b2Body* groundBody = world.CreateBody(&groundBodyDef);
	// Define the ground box shape.
	b2PolygonShape groundBox;
	// The extents are the half-widths of the box.
	groundBox.SetAsBox(50.0f, 10.0f);
	// Add the ground fixture to the ground body.
	groundBody->CreateFixture(&groundBox, 0.0f);
	// Define the dynamic body. We set its position and call the body factory.
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 4.0f);
	b2Body* body = world.CreateBody(&bodyDef);
	// Define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);
	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	// Set the box density to be non-zero, so it will be dynamic.
	fixtureDef.density = 1.0f;
	// Override the default friction.
	fixtureDef.friction = 0.3f;
	// Add the shape to the body.
	body->CreateFixture(&fixtureDef);
	// Prepare for simulation. Typically we use a time step of 1/60 of a
	// second (60Hz) and 10 iterations. This provides a high quality simulation
	// in most game scenarios.
	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;
	// This is our little game loop.
	for (int32 i = 0; i < 60; ++i)
	{
		// Instruct the world to perform a single step of simulation.
		// It is generally best to keep the time step and iterations fixed.
		world.Step(timeStep, velocityIterations, positionIterations);
		// Now print the position and angle of the body.
		b2Vec2 position = body->GetPosition();
		float32 angle = body->GetAngle();
		printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
	}
	// When the world destructor is called, all bodies and joints are freed. This can
	// create orphaned pointers, so be careful about your world management.
	getchar();

	///-----includes_end-----
	int i;
	///-----initialization_start-----
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,-10,0));
	///-----initialization_end-----
	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	///create a few basic rigid bodies
	//the ground is a cube of side 100 at position y = -56.
	//the sphere will hit it at y = -6, with center at -5
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));
		collisionShapes.push_back(groundShape);
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0,-56,0));
		btScalar mass(0.);
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);
		btVector3 localInertia(0,0,0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass,localInertia);
		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}
	{
		//create a dynamic rigidbody
		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(colShape);
		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();
		btScalar	mass(1.f);
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);
		btVector3 localInertia(0,0,0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass,localInertia);
			startTransform.setOrigin(btVector3(2,10,0));
			//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
			btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
			btRigidBody* body = new btRigidBody(rbInfo);
			dynamicsWorld->addRigidBody(body);
	}
/// Do some simulation
	///-----stepsimulation_start-----
	for (i=0;i<150;i++)
	{
		dynamicsWorld->stepSimulation(1.f/60.f,10);
		//print positions of all objects
		for (int j=dynamicsWorld->getNumCollisionObjects()-1; j>=0 ;j--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
			btRigidBody* body = btRigidBody::upcast(obj);
			btTransform trans;
			if (body && body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trans);
			} else
			{
				trans = obj->getWorldTransform();
			}
			printf("world pos object %d = %f,%f,%f\n",j,float(trans.getOrigin().getX()),float(trans.getOrigin().getY()),float(trans.getOrigin().getZ()));
		}
	}
	///-----stepsimulation_end-----
	//cleanup in the reverse order of creation/initialization
	///-----cleanup_start-----
	//remove the rigidbodies from the dynamics world and delete them
	for (i=dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}
	//delete collision shapes
	for (int j=0;j<collisionShapes.size();j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}
	//delete dynamics world
	delete dynamicsWorld;
	//delete solver
	delete solver;
	//delete broadphase
	delete overlappingPairCache;
	//delete dispatcher
	delete dispatcher;
	delete collisionConfiguration;
	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();
	getchar();

// #if defined(_WIN32)
// 	// Enable memory-leak reports
// 	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
// #endif
    g_camera.m_width = 1024;
    g_camera.m_height = 640;
	if (glfwInit() == 0)
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}
	getchar();
	char ctitle[64];
	sprintf(ctitle, "Box2D Testbed Version %d.%d.%d", b2_version.major, b2_version.minor, b2_version.revision);
#if defined(__APPLE__)
	// Not sure why, but these settings cause glewInit below to crash.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    mainWindow = glfwCreateWindow(g_camera.m_width, g_camera.m_height, ctitle, NULL, NULL);
	if (mainWindow == NULL)
	{
		fprintf(stderr, "Failed to open GLFW mainWindow.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(mainWindow);
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
	glfwSetScrollCallback(mainWindow, sScrollCallback);
	glfwSetWindowSizeCallback(mainWindow, sResizeWindow);
	glfwSetKeyCallback(mainWindow, sKeyCallback);
	glfwSetMouseButtonCallback(mainWindow, sMouseButton);
	glfwSetCursorPosCallback(mainWindow, sMouseMotion);
	glfwSetScrollCallback(mainWindow, sScrollCallback);
#if defined(__APPLE__) == FALSE
	//glewExperimental = GL_TRUE;
    GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}
#endif
	g_debugDraw.Create();
	sCreateUI();
	testCount = 0;
	while (g_testEntries[testCount].createFcn != NULL)
	{
		++testCount;
	}
	testIndex = b2Clamp(testIndex, 0, testCount - 1);
	testSelection = testIndex;
	entry = g_testEntries + testIndex;
	test = entry->createFcn();
	// Control the frame rate. One draw per monitor refresh.
	glfwSwapInterval(1);
    double time1 = glfwGetTime();
    double frameTime = 0.0;
    glClearColor(0.3f, 0.3f, 0.3f, 1.f);
 	while (!glfwWindowShouldClose(mainWindow))
	{
 		glfwGetWindowSize(mainWindow, &g_camera.m_width, &g_camera.m_height);
		glViewport(0, 0, g_camera.m_width, g_camera.m_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		unsigned char mousebutton = 0;
		int mscroll = ui.scroll;
		ui.scroll = 0;
		double xd, yd;
		glfwGetCursorPos(mainWindow, &xd, &yd);
		int mousex = int(xd);
		int mousey = int(yd);
		mousey = g_camera.m_height - mousey;
		int leftButton = glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_LEFT);
		if (leftButton == GLFW_PRESS)
			mousebutton |= IMGUI_MBUT_LEFT;
		imguiBeginFrame(mousex, mousey, mousebutton, mscroll);
		sSimulate();
		sInterface();
        // Measure speed
        double time2 = glfwGetTime();
        double alpha = 0.9f;
        frameTime = alpha * frameTime + (1.0 - alpha) * (time2 - time1);
        time1 = time2;
        char buffer[32];
        snprintf(buffer, 32, "%.1f ms", 1000.0 * frameTime);
        AddGfxCmdText(5, 5, TEXT_ALIGN_LEFT, buffer, WHITE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		RenderGLFlush(g_camera.m_width, g_camera.m_height);
		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}
	g_debugDraw.Destroy();
	RenderGLDestroy();
	glfwTerminate();

	return 0;
}
