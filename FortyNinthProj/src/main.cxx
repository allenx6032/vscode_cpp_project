#include "PainterEngine.h"

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



PX_ANN ann;
px_int epoch;

typedef struct
{
	px_point2D initv;
	px_float prediction;
	px_float truely;
}PX_Object_AnnBall;

PX_Object* PX_AnnBallCreate();

px_void PX_AnnTrainOnEpoch()
{
	px_double x=0, y=0;
	px_double vx = PX_randRange(50, 200);
	px_double vy = PX_randRange(50, 200);
	px_double initvx = vx;
	px_double initvy = vy;
	px_double input[2];
	px_double exp;
	px_double result;
	while (PX_TRUE)
	{
		vy -= 98 * 20.f / 1000.f;
		x += vx * 20.f / 1000.f;
		y += vy * 20.f / 1000.f;
		if (y<0)
		{
			break;
		}
	}
	exp = x / 1000.0;
	input[0] = initvx / 200.f;
	input[1] = initvy / 200.f;
	PX_ANNTrain(&ann, input, &exp);
	PX_ANNForward(&ann, input);
	PX_ANNGetOutput(&ann, &result);
}

px_int update_time=0;
px_float t;
PX_OBJECT_UPDATE_FUNCTION(AnnBallUpdate)
{
	PX_Object_AnnBall *pBall=PX_ObjectGetDesc(PX_Object_AnnBall,pObject);
	update_time+=elapsed*5;
	if (update_time>=20)
	{
		update_time -= 20;
		pObject->vy -= 98 * 20.f / 1000.f;
		pObject->x+=pObject->vx*20.f/1000.f;
		pObject->y+=pObject->vy*20.f/1000.f;
	}

	if (pObject->y < -100)
	{
		for (px_int i = 0; i < 1000; i++)
		{
			PX_AnnTrainOnEpoch();	
		}
		epoch += 1;
		PX_ObjectDelayDelete(pObject);
		PX_AnnBallCreate();
	}
}

PX_OBJECT_RENDER_FUNCTION(AnnBallRender)
{
	PX_Object_AnnBall* pBall = PX_ObjectGetDesc(PX_Object_AnnBall, pObject);
	PX_GeoDrawBall(psurface, 32+pObject->x, 500-pObject->y-16, 16, PX_COLOR(255, 0, 0, 0));
	do
	{
		px_char content[32] = {0};
		PX_sprintf1(content, 32, "epoch:%1k", PX_STRINGFORMAT_INT(epoch));
		PX_FontDrawText(psurface, 32 + pObject->x, 500 - pObject->y - 16-36,PX_ALIGN_MIDBOTTOM, content, PX_COLOR(255, 0, 0, 0));
	} while (0);

	PX_GeoDrawBall(psurface,32+pBall->prediction * 1000.0, 500-16, 16, PX_COLOR(255, 255, 0, 0));
	PX_FontDrawText(psurface, 32 + pBall->prediction * 1000.0, 500 - 16, PX_ALIGN_MIDBOTTOM, "prediction", PX_COLOR(255, 0, 0, 0));
	PX_GeoDrawBall(psurface, 32+pBall->truely * 1000.0, 500-16, 16, PX_COLOR(64, 0, 0, 255));
}

PX_Object* PX_AnnBallCreate()
{
	PX_Object* pObject = PX_ObjectCreateEx(mp, root, 0, 0, 0, 0, 0, 0, 0, AnnBallUpdate, AnnBallRender, 0,0,sizeof(PX_Object_AnnBall));
	// PX_Object_AnnBall *pBall = PX_ObjectGetDesc(PX_Object_AnnBall, pObject);
	px_double io[2];
	px_double result;
	px_float initvx = PX_randRange(50, 200);
	px_float initvy = PX_randRange(50, 200);

	
	// pBall->initv.x = initvx;
	// pBall->initv.y = initvy;
	pObject->vx= initvx;
	pObject->vy= initvy;

	io[0] = initvx / 200.f;
	io[1] = initvy / 200.f;
	PX_ANNForward(&ann, io);
	PX_ANNGetOutput(&ann, &result);
	// pBall->prediction = result;

	do
	{
		px_double x = 0, y = 0;
		px_double vx = initvx;
		px_double vy = initvy;
		while (PX_TRUE)
		{
			vy -= 98 * 20.f / 1000.f;
			x += vx * 20.f / 1000.f;
			y += vy * 20.f / 1000.f;
			if (y < 0)
			{
				break;
			}
		}
		// pBall->truely = x / 1000.0;
	} while (0);

	return pObject;
}

PX_OBJECT_RENDER_FUNCTION(grounddraw)
{
	PX_GeoDrawLine(psurface, 0, 500-16, 800, 500-16,3, PX_COLOR(128, 255, 0, 0));
}


px_int main()
{
	PainterEngine_Initialize(800, 600);
	PainterEngine_SetBackgroundColor(PX_COLOR_WHITE);
	PX_AnnBallCreate();
	PX_ObjectCreateFunction(mp, root, 0, grounddraw, 0);
	PX_ANNInitialize(mp,&ann,0.005,PX_ANN_REGULARZATION_NONE,0);
	PX_ANNAddLayer(&ann,2,0,PX_ANN_ACTIVATION_FUNCTION_LINEAR,PX_ANN_LAYER_WEIGHT_INITMODE_GAUSSRAND,0.5);
	PX_ANNAddLayer(&ann,64,1,PX_ANN_ACTIVATION_FUNCTION_SIGMOID, PX_ANN_LAYER_WEIGHT_INITMODE_GAUSSRAND,0.5);
	PX_ANNAddLayer(&ann,128,1, PX_ANN_ACTIVATION_FUNCTION_SIGMOID, PX_ANN_LAYER_WEIGHT_INITMODE_GAUSSRAND,0.5);
	PX_ANNAddLayer(&ann,1,0, PX_ANN_ACTIVATION_FUNCTION_LINEAR, PX_ANN_LAYER_WEIGHT_INITMODE_GAUSSRAND,0.5);

    return 0;
}