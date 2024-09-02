// #include "PainterEngine.h"

// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"
#include "lua_includes.h"
#include "LuaBinding.h"
#include "LuaRef.h"

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS 1
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "testDraw.h"
#include "testSettings.h"
#include "test.h"

#include <algorithm>
#include <stdio.h>
#include <thread>
#include <chrono>

#if defined(_WIN32)
#include <crtdbg.h>
#endif

GLFWwindow* g_mainWindow = nullptr;
static int32 s_testSelection = 0;
static Test* s_test = nullptr;
static Settings s_settings;
static bool s_rightMouseDown = false;
static b2Vec2 s_clickPointWS = b2Vec2_zero;
static float s_displayScale = 1.0f;

void glfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "GLFW error occured. Code: %d. Description: %s\n", error, description);
}

static inline bool CompareTests(const TestEntry& a, const TestEntry& b)
{
	int result = strcmp(a.category, b.category);
	if (result == 0)
	{
		result = strcmp(a.name, b.name);
	}
	return result < 0;
}

static void SortTests()
{
	std::sort(g_testEntries, g_testEntries + g_testCount, CompareTests);
}

static void RestartTest()
{
	delete s_test;
	s_test = g_testEntries[s_settings.m_testIndex].createFcn();
}

static void CreateUI(GLFWwindow* window, const char* glslVersion = NULL)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	bool success;
	success = ImGui_ImplGlfw_InitForOpenGL(window, false);
	if (success == false)
	{
		printf("ImGui_ImplGlfw_InitForOpenGL failed\n");
		assert(false);
	}
	success = ImGui_ImplOpenGL3_Init(glslVersion);
	if (success == false)
	{
		printf("ImGui_ImplOpenGL3_Init failed\n");
		assert(false);
	}
	// Search for font file
	const char* fontPath1 = "data/droid_sans.ttf";
	const char* fontPath2 = "../data/droid_sans.ttf";
	const char* fontPath = nullptr;
	FILE* file1 = fopen(fontPath1, "rb");
	FILE* file2 = fopen(fontPath2, "rb");
	if (file1)
	{
		fontPath = fontPath1;
		fclose(file1);
	}
	if (file2)
	{
		fontPath = fontPath2;
		fclose(file2);
	}
	if (fontPath)
	{
		ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath, 14.0f * s_displayScale);
	}
}

static void ResizeWindowCallback(GLFWwindow*, int width, int height)
{
	g_camera.m_width = width;
	g_camera.m_height = height;
	s_settings.m_windowWidth = width;
	s_settings.m_windowHeight = height;
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	if (ImGui::GetIO().WantCaptureKeyboard)
	{
		return;
	}
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Quit
			glfwSetWindowShouldClose(g_mainWindow, GL_TRUE);
			break;
		case GLFW_KEY_LEFT:
			// Pan left
			if (mods == GLFW_MOD_CONTROL)
			{
				b2Vec2 newOrigin(2.0f, 0.0f);
				s_test->ShiftOrigin(newOrigin);
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
				s_test->ShiftOrigin(newOrigin);
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
				s_test->ShiftOrigin(newOrigin);
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
				s_test->ShiftOrigin(newOrigin);
			}
			else
			{
				g_camera.m_center.y += 0.5f;
			}
			break;
		case GLFW_KEY_HOME:
			g_camera.ResetView();
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
			RestartTest();
			break;
		case GLFW_KEY_SPACE:
			// Launch a bomb.
			if (s_test)
			{
				s_test->LaunchBomb();
			}
			break;
		case GLFW_KEY_O:
			s_settings.m_singleStep = true;
			break;
		case GLFW_KEY_P:
			s_settings.m_pause = !s_settings.m_pause;
			break;
		case GLFW_KEY_LEFT_BRACKET:
			// Switch to previous test
			--s_testSelection;
			if (s_testSelection < 0)
			{
				s_testSelection = g_testCount - 1;
			}
			break;
		case GLFW_KEY_RIGHT_BRACKET:
			// Switch to next test
			++s_testSelection;
			if (s_testSelection == g_testCount)
			{
				s_testSelection = 0;
			}
			break;
		case GLFW_KEY_TAB:
			g_debugDraw.m_showUI = !g_debugDraw.m_showUI;
		default:
			if (s_test)
			{
				s_test->Keyboard(key);
			}
		}
	}
	else if (action == GLFW_RELEASE)
	{
		s_test->KeyboardUp(key);
	}
}

static void CharCallback(GLFWwindow* window, unsigned int c)
{
	ImGui_ImplGlfw_CharCallback(window, c);
}

static void MouseButtonCallback(GLFWwindow* window, int32 button, int32 action, int32 mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
	double xd, yd;
	glfwGetCursorPos(g_mainWindow, &xd, &yd);
	b2Vec2 ps((float)xd, (float)yd);
	// Use the mouse to move things around.
	if (button == GLFW_MOUSE_BUTTON_1)
	{
        //<##>
        //ps.Set(0, 0);
		b2Vec2 pw = g_camera.ConvertScreenToWorld(ps);
		if (action == GLFW_PRESS)
		{
			if (mods == GLFW_MOD_SHIFT)
			{
				s_test->ShiftMouseDown(pw);
			}
			else
			{
				s_test->MouseDown(pw);
			}
		}
		if (action == GLFW_RELEASE)
		{
			s_test->MouseUp(pw);
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_2)
	{
		if (action == GLFW_PRESS)
		{	
			s_clickPointWS = g_camera.ConvertScreenToWorld(ps);
			s_rightMouseDown = true;
		}
		if (action == GLFW_RELEASE)
		{
			s_rightMouseDown = false;
		}
	}
}

static void MouseMotionCallback(GLFWwindow*, double xd, double yd)
{
	b2Vec2 ps((float)xd, (float)yd);
	b2Vec2 pw = g_camera.ConvertScreenToWorld(ps);
	s_test->MouseMove(pw);
	if (s_rightMouseDown)
	{
		b2Vec2 diff = pw - s_clickPointWS;
		g_camera.m_center.x -= diff.x;
		g_camera.m_center.y -= diff.y;
		s_clickPointWS = g_camera.ConvertScreenToWorld(ps);
	}
}

static void ScrollCallback(GLFWwindow* window, double dx, double dy)
{
	ImGui_ImplGlfw_ScrollCallback(window, dx, dy);
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}
	if (dy > 0)
	{
		g_camera.m_zoom /= 1.1f;
	}
	else
	{
		g_camera.m_zoom *= 1.1f;
	}
}

static void UpdateUI()
{
	float menuWidth = 180.0f * s_displayScale;
	if (g_debugDraw.m_showUI)
	{
		ImGui::SetNextWindowPos({g_camera.m_width - menuWidth - 10.0f, 10.0f});
		ImGui::SetNextWindowSize({menuWidth, g_camera.m_height - 20.0f});
		ImGui::Begin("Tools", &g_debugDraw.m_showUI, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		if (ImGui::BeginTabBar("ControlTabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Controls"))
			{
				ImGui::SliderInt("Vel Iters", &s_settings.m_velocityIterations, 0, 50);
				ImGui::SliderInt("Pos Iters", &s_settings.m_positionIterations, 0, 50);
				ImGui::SliderFloat("Hertz", &s_settings.m_hertz, 5.0f, 120.0f, "%.0f hz");
				ImGui::Separator();
				ImGui::Checkbox("Sleep", &s_settings.m_enableSleep);
				ImGui::Checkbox("Warm Starting", &s_settings.m_enableWarmStarting);
				ImGui::Checkbox("Time of Impact", &s_settings.m_enableContinuous);
				ImGui::Checkbox("Sub-Stepping", &s_settings.m_enableSubStepping);
				ImGui::Separator();
				ImGui::Checkbox("Shapes", &s_settings.m_drawShapes);
				ImGui::Checkbox("Joints", &s_settings.m_drawJoints);
				ImGui::Checkbox("AABBs", &s_settings.m_drawAABBs);
				ImGui::Checkbox("Contact Points", &s_settings.m_drawContactPoints);
				ImGui::Checkbox("Contact Normals", &s_settings.m_drawContactNormals);
				ImGui::Checkbox("Contact Impulses", &s_settings.m_drawContactImpulse);
				ImGui::Checkbox("Friction Impulses", &s_settings.m_drawFrictionImpulse);
				ImGui::Checkbox("Center of Masses", &s_settings.m_drawCOMs);
				ImGui::Checkbox("Statistics", &s_settings.m_drawStats);
				ImGui::Checkbox("Profile", &s_settings.m_drawProfile);
				ImVec2 button_sz = ImVec2(-1, 0);
				if (ImGui::Button("Pause (P)", button_sz))
				{
					s_settings.m_pause = !s_settings.m_pause;
				}
				if (ImGui::Button("Single Step (O)", button_sz))
				{
					s_settings.m_singleStep = !s_settings.m_singleStep;
				}
				if (ImGui::Button("Restart (R)", button_sz))
				{
					RestartTest();
				}
				if (ImGui::Button("Quit", button_sz))
				{
					glfwSetWindowShouldClose(g_mainWindow, GL_TRUE);
				}
				ImGui::EndTabItem();
			}
			ImGuiTreeNodeFlags leafNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			leafNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			if (ImGui::BeginTabItem("Tests"))
			{
				int categoryIndex = 0;
				const char* category = g_testEntries[categoryIndex].category;
				int i = 0;
				while (i < g_testCount)
				{
					bool categorySelected = strcmp(category, g_testEntries[s_settings.m_testIndex].category) == 0;
					ImGuiTreeNodeFlags nodeSelectionFlags = categorySelected ? ImGuiTreeNodeFlags_Selected : 0;
					bool nodeOpen = ImGui::TreeNodeEx(category, nodeFlags | nodeSelectionFlags);

					if (nodeOpen)
					{
						while (i < g_testCount && strcmp(category, g_testEntries[i].category) == 0)
						{
							ImGuiTreeNodeFlags selectionFlags = 0;
							if (s_settings.m_testIndex == i)
							{
								selectionFlags = ImGuiTreeNodeFlags_Selected;
							}
							ImGui::TreeNodeEx((void*)(intptr_t)i, leafNodeFlags | selectionFlags, "%s", g_testEntries[i].name);
							if (ImGui::IsItemClicked())
							{
								s_testSelection = i;
							}
							++i;
						}
						ImGui::TreePop();
					}
					else
					{
						while (i < g_testCount && strcmp(category, g_testEntries[i].category) == 0)
						{
							++i;
						}
					}
					if (i < g_testCount)
					{
						category = g_testEntries[i].category;
						categoryIndex = i;
					}
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
		s_test->UpdateUI();
	}
}

void run( lua_State* L, const char* code );
using namespace ManualBind;
class Widget
{
    public:
    std::string stringy;
    int numbery;
    void setThings( std::string str, int num )
    {
        stringy = str;
        numbery = num;
    }
};
using WidgetPtr = std::shared_ptr<Widget>;
struct WidgetBinding : public Binding<WidgetBinding,Widget>
{
    static constexpr const char* class_name = "Widget";
    static luaL_Reg* members()
    {
        static luaL_Reg members[] =
        {
            { "setThings", callSetThings },
            { nullptr, nullptr }
        };
        return members;
    }
    static int callSetThings( lua_State* L )
    {
        WidgetPtr p = fromStack( L, 1 );
        std::string str( luaL_checkstring( L, 2 ) );
        int num = luaL_checkinteger( L, 3 );
        p->setThings( str, num );
        return 1;
    }
};
class Extra
{
};
using ExtraPtr = std::shared_ptr<Extra>;
struct ExtraBinding : public Binding<ExtraBinding,Extra>
{
    static constexpr const char* class_name = "Extra";
    static void setExtraMeta( lua_State* L )
    {
        lua_pushliteral( L, "Alara" );
        lua_setfield( L, -2, "DEF_NAME" );
    }
};
class Person
{
    public:
    std::string name;
};
using PersonPtr = std::shared_ptr<Person>;
struct PersonBinding : public Binding<PersonBinding,Person>
{
    static constexpr const char* class_name = "Person";
    static bind_properties* properties()
    {
        static bind_properties properties[] =
        {
            { "name", getName, setName },
            { nullptr, nullptr, nullptr }
        };
        return properties;
    }
    static int getName( lua_State* L )
    {
        PersonPtr p = fromStack( L, 1 );
        lua_pushstring( L, p->name.c_str() );
        return 1;
    }
    static int setName( lua_State* L )
    {
        PersonPtr p = fromStack( L, 1 );
        const char* name = luaL_checkstring( L, 3 );
        p->name = name;
        return 0;
    }
};



// px_texture tex;
// PX_Object *canvas_root;
// PX_CanvasVM canvasvm;
// PX_Object *Canvas, *Layer, *PainterBox, *Menu;
// px_byte data[16 * 1024 * 1024];

// px_void PX_MenuExecuteOnOpenFile(px_void *buffer, px_int size, px_void *userPtr)
// {
//     // 打开并导入文件
//     PX_CanvasVMImport(&canvasvm, (const px_byte *)buffer, size);
//     return;
// }

// px_void PX_MenuExecuteOpenFile(px_void *userPtr)
// {
//     // 打开文件对话框
//     PX_RequestData("open", data, sizeof(data), userPtr, PX_MenuExecuteOnOpenFile);
//     return;
// }

// px_void PX_MenuExecuteExportSaveFile(px_void *userPtr)
// {
//     // 保存文件
//     px_memory data;
//     PX_MemoryInitialize(mp_static, &data);
//     PX_CanvasVMExport(&canvasvm, &data);
//     PX_RequestData("download:PainterEngine.pe", data.buffer, data.usedsize, userPtr, 0);
//     PX_MemoryFree(&data);
//     return;
// }

// px_void PX_MenuExecuteExportPngFile(px_void *userPtr)
// {
//     // 导出为png图片
//     px_memory data;
//     PX_MemoryInitialize(mp_static, &data);
//     PX_CanvasVMExportAsPng(&canvasvm, &data);
//     PX_RequestData("download:image.png", data.buffer, data.usedsize, userPtr, 0);
//     PX_MemoryFree(&data);
//     return;
// }

// px_void PX_MenuExecuteLog(px_void *userPtr)
// {
//     // 不做任何事情
//     printf("Hello!\n");
//     return;
// }

void run( lua_State* L, const char* code )
{
    if( luaL_dostring( L, code ) )
    {
        // WARN(lua_tostring( L, -1 ));
        lua_tostring( L, -1 );
        lua_pop( L, 1 );
    }
}


// int main(int argc, char * argv[])
int main()
{
    lua_State* L = luaL_newstate();
    WidgetBinding::register_class( L );
    WidgetPtr wp = std::make_shared<Widget>();
    WidgetBinding::push( L, wp );
    lua_setglobal( L, "widget" );
    run( L, "widget:setThings( 'Hello World', 42 )" );
    // REQUIRE( wp->stringy == "Hello World" );
    // REQUIRE( wp->numbery == 42 );
    lua_close( L );
    getchar();

    L = luaL_newstate();
    ExtraBinding::register_class( L );
    ExtraPtr ep = std::make_shared<Extra>();
    ExtraBinding::push( L, ep );
    lua_setglobal( L, "person" );
    run( L, "name1 = person.DEF_NAME" );
    lua_getglobal( L, "name1" );
    std::string name1( lua_tostring( L, -1 ) );
    // REQUIRE( name1 == "Alara" );
    lua_close( L );
    getchar();

    L = luaL_newstate();
    ExtraBinding::register_class( L );
    ExtraPtr ep1 = std::make_shared<Extra>();
    ExtraBinding::push( L, ep1 );
    lua_setglobal( L, "ep1" );
    ExtraPtr ep2 = std::make_shared<Extra>();
    ExtraBinding::push( L, ep2 );
    lua_setglobal( L, "ep2" );
    run( L, "ep1.perinstance = 'Nigel'" );
    run( L, "name2 = ep1.perinstance" );
    lua_getglobal( L, "name2" );
    std::string name2( lua_tostring( L, -1 ) );
    // REQUIRE( name2 == "Nigel" );
    run( L, "name2 = ep2.perinstance" );
    lua_getglobal( L, "name2" );
    // REQUIRE( lua_type( L, -1 ) == LUA_TNIL );
    lua_close( L );
    getchar();

    L = luaL_newstate();
    PersonBinding::register_class( L );
    PersonPtr pp = std::make_shared<Person>();
    PersonBinding::push( L, pp );
    lua_setglobal( L, "person" );
    run( L, "person.name ='Bob'" );
    // REQUIRE( pp->name == "Bob" );
    pp->name = "Jane";
    run( L, "name = person.name" );
    lua_getglobal( L, "name" );
    std::string name (luaL_checkstring( L, -1 ));
    // REQUIRE( name == "Jane" );
    lua_close( L );
    getchar();
	 
#if defined(_WIN32)
	// Enable memory-leak reports
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
#endif
	char buffer[128];
	s_settings.Load();
	SortTests();
	glfwSetErrorCallback(glfwErrorCallback);
	g_camera.m_width = s_settings.m_windowWidth;
	g_camera.m_height = s_settings.m_windowHeight;
	if (glfwInit() == 0)
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
#if __APPLE__
    const char* glslVersion = "#version 150";
#else
    const char* glslVersion = NULL;
#endif
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	sprintf(buffer, "Box2D Testbed Version %d.%d.%d", b2_version.major, b2_version.minor, b2_version.revision);
	bool fullscreen = false;
	if (fullscreen)
	{
		g_mainWindow = glfwCreateWindow(800, 600, buffer, glfwGetPrimaryMonitor(), NULL);
	}
	else
	{
		g_mainWindow = glfwCreateWindow(g_camera.m_width, g_camera.m_height, buffer, NULL, NULL);
	}
	if (g_mainWindow == NULL)
	{
		fprintf(stderr, "Failed to open GLFW g_mainWindow.\n");
		glfwTerminate();
		return -1;
	}
	glfwGetWindowContentScale(g_mainWindow, &s_displayScale, &s_displayScale);
	glfwMakeContextCurrent(g_mainWindow);
	// Load OpenGL functions using glad
	int version = gladLoadGL(glfwGetProcAddress);
	printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
	glfwSetWindowSizeCallback(g_mainWindow, ResizeWindowCallback);
	glfwSetKeyCallback(g_mainWindow, KeyCallback);
	glfwSetCharCallback(g_mainWindow, CharCallback);
	glfwSetMouseButtonCallback(g_mainWindow, MouseButtonCallback);
	glfwSetCursorPosCallback(g_mainWindow, MouseMotionCallback);
	glfwSetScrollCallback(g_mainWindow, ScrollCallback);
	g_debugDraw.Create();
	CreateUI(g_mainWindow, glslVersion);
	s_settings.m_testIndex = b2Clamp(s_settings.m_testIndex, 0, g_testCount - 1);
	s_testSelection = s_settings.m_testIndex;
	s_test = g_testEntries[s_settings.m_testIndex].createFcn();
	// Control the frame rate. One draw per monitor refresh.
	//glfwSwapInterval(1);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	std::chrono::duration<double> frameTime(0.0);
	std::chrono::duration<double> sleepAdjust(0.0);
	while (!glfwWindowShouldClose(g_mainWindow))
	{
		std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
		glfwGetWindowSize(g_mainWindow, &g_camera.m_width, &g_camera.m_height);
        int bufferWidth, bufferHeight;
        glfwGetFramebufferSize(g_mainWindow, &bufferWidth, &bufferHeight);
        glViewport(0, 0, bufferWidth, bufferHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		if (g_debugDraw.m_showUI)
		{
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(ImVec2(float(g_camera.m_width), float(g_camera.m_height)));
			ImGui::SetNextWindowBgAlpha(0.0f);
			ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
			ImGui::End();
			const TestEntry& entry = g_testEntries[s_settings.m_testIndex];
			sprintf(buffer, "%s : %s", entry.category, entry.name);
			s_test->DrawTitle(buffer);
		}
		s_test->Step(s_settings);
		UpdateUI();
		// ImGui::ShowDemoWindow();
		if (g_debugDraw.m_showUI)
		{
			sprintf(buffer, "%.1f ms", 1000.0 * frameTime.count());
			g_debugDraw.DrawString(5, g_camera.m_height - 20, buffer);
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(g_mainWindow);
		if (s_testSelection != s_settings.m_testIndex)
		{
			s_settings.m_testIndex = s_testSelection;
			delete s_test;
			s_test = g_testEntries[s_settings.m_testIndex].createFcn();
			g_camera.ResetView();
		}
		glfwPollEvents();
		// Throttle to cap at 60Hz. This adaptive using a sleep adjustment. This could be improved by
		// using mm_pause or equivalent for the last millisecond.
		std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
		std::chrono::duration<double> target(1.0 / 60.0);
		std::chrono::duration<double> timeUsed = t2 - t1;
		std::chrono::duration<double> sleepTime = target - timeUsed + sleepAdjust;
		if (sleepTime > std::chrono::duration<double>(0))
		{
			std::this_thread::sleep_for(sleepTime);
		}
		std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
		frameTime = t3 - t1;
		// Compute the sleep adjustment using a low pass filter
		sleepAdjust = 0.9 * sleepAdjust + 0.1 * (target - frameTime);
	}
	delete s_test;
	s_test = nullptr;
	g_debugDraw.Destroy();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
	s_settings.Save();
    getchar();
	return 0;
}

int px_main()
{

    // lua_State* L = luaL_newstate();
    // BasicPODbinding::register_class( L );
    // Basic* bp = new Basic();
    // // Give Lua a copy.
    // BasicPODbinding::push( L, bp );
    // lua_setglobal( L, "bp" );
    // // Retrieve
    // lua_getglobal( L, "bp" );
    // Basic* fromLua = BasicPODbinding::fromStackThrow( L, 1 );
    // lua_pop( L, 1 );
    // // REQUIRE( bp == fromLua );
    // lua_close( L );
    // delete bp;
   

    // PX_IO_Data data;
    // PainterEngine_Initialize(1200, 600);
    // PainterEngine_SetBackgroundColor(PX_COLOR(255, 224, 224, 224));
    // // Canvas虚拟机
    // PX_CanvasVMInitialize(mp, &canvasvm, 600, 600, App.runtime.surface_width - 224 - 200, App.runtime.surface_height - 128);
    // canvas_root = PX_ObjectCreate(mp, root, 0, 0, 0, 0, 0, 0);
    // // 调色板
    // PainterBox = PX_Object_PainterBoxCreate(mp, canvas_root, 0, 28, &canvasvm);
    // // 图层
    // Layer = PX_Object_LayerBoxCreate(mp, canvas_root, App.runtime.surface_width - 196, 64, 0, &canvasvm);
    // // 画板
    // Canvas = PX_Object_CanvasCreate(mp, canvas_root, 200, 64, &canvasvm);
    // // 菜单
    // Menu = PX_Object_MenuCreate(mp, canvas_root, 0, 0, 128, 0);
    // PX_Object_MenuAddItem(Menu, 0, "Open File", PX_MenuExecuteOpenFile, 0);
    // PX_Object_MenuAddItem(Menu, 0, "Save File", PX_MenuExecuteExportSaveFile, 0);
    // PX_Object_MenuAddItem(Menu, 0, "Export to PNG file", PX_MenuExecuteExportPngFile, 0);
    // PX_Object_MenuAddItem(Menu, 0, "Do nothing", PX_MenuExecuteLog, 0);
    // // 示范:加载绘制的文件
    // data = PX_LoadFileToIOData("assets/PainterEngine.pe");
    // if (data.size)
    //     PX_CanvasVMImport(&canvasvm, data.buffer, data.size);
    // PX_FreeIOData(&data);
    // return 0;
}