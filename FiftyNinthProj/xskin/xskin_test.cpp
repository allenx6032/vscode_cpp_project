#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "xskin_App.h"

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MyApp app;
	printf("xskin_test\n");
	app.SetResZip(TEXT("skin.zip"));
	printf("xskin_test\n");
	MainWindow mainWnd;
	printf("xskin_test\n");
	mainWnd.Create();
	printf("xskin_test\n");
	UIRect rc(0, 0, 1024, 800);
	printf("xskin_test\n");
	mainWnd.SetBounds(rc);
	printf("xskin_test\n");
	mainWnd.CenterWindow();
	printf("xskin_test\n");
	mainWnd.SetFrameVisible(TRUE);
	printf("xskin_test\n");
	mainWnd.Show();
	return app.Run();
}

