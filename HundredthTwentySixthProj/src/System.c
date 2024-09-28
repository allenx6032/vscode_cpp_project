#include "System.h"

int Start()
{
	int res = Init();

	if (res != 1)
	{
		printf("Failed init with, %i \n", res);
		return res;
	}
	Run();

	return 1;
}

int Init()
{
	InitWindow(720, 1080, "C OpenGL test");
	glfwSetScrollCallback(wnd, ScrollCallback);
	glfwSetWindowSizeCallback(wnd, WindowResizeCallback);
	glfwSetKeyCallback(wnd, KeyCallback);
	glfwSetCursorPosCallback(wnd, CursorPosCallback);
	glfwSetMouseButtonCallback(wnd, MouseButtonCallback);
	SetupCamera();

	return InitOpenGL();
	return 1;
}
int Run()
{
	Frame();

	Exit();
	return 1;
}
void Frame()
{
	printf("First frame\n");
	while (!glfwWindowShouldClose(wnd))
	{

		ResetKeyUp();
		ResetMouseUp();
		Render();
		glfwSwapBuffers(wnd);
		glfwPollEvents();
		MouseInputProcessing();
		ScrollProcessing();
		UpdateCamera();
		if (glfwGetKey(wnd, GLFW_KEY_R))
			SetupMaterials();
		// Should've added this A LONG time ago
		if (glfwGetKey(wnd, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(wnd, GLFW_KEY_C))
			glfwSetWindowShouldClose(wnd, 1);
		LightMovement();
	}
}

void Exit()
{
	Cleanup();
}

void WindowResizeCallback(GLFWwindow *window, int x, int y)
{
	glViewport(0, 0, x, y);
}