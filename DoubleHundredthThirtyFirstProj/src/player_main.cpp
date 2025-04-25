#include "player_main.h"

#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <Commdlg.h>
#include <Shlobj.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <shellapi.h>
#include <windows.h>

#include <string>
#include <vector>

#include "cocos2d.h"
#include "player_AppDelegate.h"
#include "player_ProjectConfig.h"
#include "player_SimulatorConfig.h"

#include "player_resource.h"

using namespace std;
USING_NS_CC;

WNDPROC g_oldProc = NULL;
bool g_landscape  = false;
bool g_windTop    = false;
CCSize g_screenSize;
HACCEL hAccelTable;
#define USE_WIN32_CONSOLE 0

void createSimulator(const std::string& viewName, int width, int height, float frameZoomFactor = 1.0f);

// #pragma comment(lib, "comctl32.lib")
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
// int APIENTRY _tWinMain(HINSTANCE hInstance,
//                        HINSTANCE hPrevInstance,
//                        LPTSTR    lpCmdLine,
//                        int       nCmdShow)
// int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ProjectConfig project;
    HWND hwndConsole;

    // load project config from command line args
    // vector<string> args;
    // for (int i = 0; i < __argc; ++i)
    // {
    //     wstring ws(__wargv[i]);
    //     string s;
    //     s.assign(ws.begin(), ws.end());
    //     args.push_back(s);
    // }
    // project.parseCommandLine(args);

#if USE_WIN32_CONSOLE <= 0
    project.setShowConsole(false);
#endif

    // create console window
    if (project.isShowConsole())
    {
        AllocConsole();
        hwndConsole = GetConsoleWindow();
        if (hwndConsole != NULL)
        {
            ShowWindow(hwndConsole, SW_SHOW);
            BringWindowToTop(hwndConsole);
            freopen("CONOUT$", "wt", stdout);
            freopen("CONOUT$", "wt", stderr);

            HMENU hmenu = GetSystemMenu(hwndConsole, FALSE);
            if (hmenu != NULL) DeleteMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
        }
    }
    project.dump();

    // create the application instance
    AppDelegate app;
    int ret = Application::getInstance()->run();

#if USE_WIN32_CONSOLE
    if (!ret)
    {
        system("pause");
    }
    FreeConsole();
#endif

    return ret;
}


static INT_PTR CALLBACK AboutDialogCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static void createViewMenu()
{
    auto glview    = Director::getInstance()->getOpenGLView();
    HMENU hSysMenu = GetSystemMenu(glview->getWin32Window(), FALSE);
    HMENU viewMenu = GetSubMenu(hSysMenu, 8);
    int count      = SimulatorConfig::getInstance()->getScreenSizeCount();
    for (int i = count - 1; i >= 0; --i)
    {
        SimulatorScreenSize size = SimulatorConfig::getInstance()->getScreenSize(i);
        wstring menuName;
        menuName.assign(size.title.begin(), size.title.end());

        MENUITEMINFO item;
        ZeroMemory(&item, sizeof(item));
        item.cbSize     = sizeof(item);
        item.fMask      = MIIM_FTYPE | MIIM_ID | MIIM_STRING;
        item.fType      = MFT_STRING;
        item.wID        = ID_VIEW_SIZE + i;
        item.dwTypeData = (LPTSTR)menuName.c_str();
        item.cch        = menuName.length();

        InsertMenuItem(viewMenu, 0, TRUE, &item);
    }
}

static void updateMenu()
{
    auto glview       = Director::getInstance()->getOpenGLView();
    HMENU hSysMenu    = GetSystemMenu(glview->getWin32Window(), FALSE);
    HMENU viewMenu    = GetSubMenu(hSysMenu, 8);
    HMENU viewControl = GetSubMenu(hSysMenu, 9);

    // landscape
    if (g_landscape)
    {
        CheckMenuItem(viewMenu, ID_VIEW_PORTRAIT, MF_BYCOMMAND | MF_UNCHECKED);
        CheckMenuItem(viewMenu, ID_VIEW_LANDSCAPE, MF_BYCOMMAND | MF_CHECKED);
    }
    else
    {
        CheckMenuItem(viewMenu, ID_VIEW_PORTRAIT, MF_BYCOMMAND | MF_CHECKED);
        CheckMenuItem(viewMenu, ID_VIEW_LANDSCAPE, MF_BYCOMMAND | MF_UNCHECKED);
    }

    // top
    if (g_windTop)
    {
        ::SetWindowPos(glview->getWin32Window(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        CheckMenuItem(viewControl, ID_CONTROL_TOP, MF_BYCOMMAND | MF_CHECKED);
    }
    else
    {
        ::SetWindowPos(glview->getWin32Window(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        CheckMenuItem(viewControl, ID_CONTROL_TOP, MF_BYCOMMAND | MF_UNCHECKED);
    }

    int count = SimulatorConfig::getInstance()->getScreenSizeCount();
    for (int i = 0; i < count; ++i)
    {
        bool bSel = false;

        SimulatorScreenSize size = SimulatorConfig::getInstance()->getScreenSize(i);
        if ((size.width == g_screenSize.width && size.height == g_screenSize.height) || (size.width == g_screenSize.height && size.height == g_screenSize.width))
        {
            bSel = true;
        }
        CheckMenuItem(viewMenu, i, MF_BYPOSITION | (bSel ? MF_CHECKED : MF_UNCHECKED));
    }

    int scale = glview->getFrameZoomFactor() * 100;
    CheckMenuItem(viewMenu, ID_VIEW_ZOOMOUT100, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(viewMenu, ID_VIEW_ZOOMOUT75, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(viewMenu, ID_VIEW_ZOOMOUT50, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(viewMenu, ID_VIEW_ZOOMOUT25, MF_BYCOMMAND | MF_UNCHECKED);
    switch (scale)
    {
    case 100:
        CheckMenuItem(viewMenu, ID_VIEW_ZOOMOUT100, MF_BYCOMMAND | MF_CHECKED);
        break;
    case 75:
        CheckMenuItem(viewMenu, ID_VIEW_ZOOMOUT75, MF_BYCOMMAND | MF_CHECKED);
        break;
    case 50:
        CheckMenuItem(viewMenu, ID_VIEW_ZOOMOUT50, MF_BYCOMMAND | MF_CHECKED);
        break;
    case 25:
        CheckMenuItem(viewMenu, ID_VIEW_ZOOMOUT25, MF_BYCOMMAND | MF_CHECKED);
        break;
    default:
        break;
    }
}

/*@brief updateView*/
static void updateView()
{
    auto glview = Director::getInstance()->getOpenGLView();
    auto policy = glview->getResolutionPolicy();
    auto designSize = glview->getDesignResolutionSize();

    if (g_landscape)
    {
        glview->setFrameSize(g_screenSize.width, g_screenSize.height);
    }
    else
    {
        glview->setFrameSize(g_screenSize.height, g_screenSize.width);
    }

    glview->setDesignResolutionSize(designSize.width, designSize.height, policy);

    updateMenu();
}

static void onViewChangeOrientation(int viewMenuID)
{
    if (viewMenuID == ID_VIEW_PORTRAIT && g_landscape)
    {
        g_landscape = false;
        updateView();
    }
    else if (viewMenuID == ID_VIEW_LANDSCAPE && !g_landscape)
    {
        g_landscape = true;
        updateView();
    }
}

static void onViewZoomOut(int viewMenuID)
{
    auto glview = Director::getInstance()->getOpenGLView();
    float scale = 1.0;
    switch (viewMenuID)
    {
    case ID_VIEW_ZOOMOUT100:
        scale = 1.0;
        break;
    case ID_VIEW_ZOOMOUT75:
        scale = 0.75;
        break;
    case ID_VIEW_ZOOMOUT50:
        scale = 0.50;
        break;
    case ID_VIEW_ZOOMOUT25:
        scale = 0.25;
        break;
    default:
        break;
    }
    glview->setFrameZoomFactor(scale);
    updateView();
}

static void onViewChangeFrameSize(int viewMenuID)
{
    int index = viewMenuID - ID_VIEW_SIZE;
    int count = SimulatorConfig::getInstance()->getScreenSizeCount();
    if (index >= 0 && index < count)
    {
        SimulatorScreenSize size = SimulatorConfig::getInstance()->getScreenSize(index);
        g_screenSize.width = size.width;
        g_screenSize.height = size.height;
        updateView();
    }
}

static void onHelpAbout()
{
    auto glview = Director::getInstance()->getOpenGLView();
    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_ABOUT), glview->getWin32Window(), AboutDialogCallback);
}

static void shutDownApp()
{
    auto glview = Director::getInstance()->getOpenGLView();
    HWND hWnd = glview->getWin32Window();
    ::SendMessage(hWnd, WM_CLOSE, NULL, NULL);
}

static void onRestart()
{
    PROCESS_INFORMATION info;
    STARTUPINFO startup;
    TCHAR szPath[128] = { 0 };
    TCHAR *szCmdLine = NULL;
    GetModuleFileName(NULL, szPath, sizeof(szPath));
    szCmdLine = GetCommandLine();
    GetStartupInfo(&startup);
    BOOL bSucc = CreateProcess(szPath, szCmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startup, &info);
    if (bSucc)
    {
        ExitProcess(-1);
    }
}

static LRESULT CALLBACK SNewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    switch (message)
    {
    case WM_KEYDOWN:
        if (wParam == VK_F5)
        {
            PostMessage(hWnd, WM_SYSCOMMAND, ID_CONTROL_RELOAD, lParam);
            break;
        }
    case WM_SYSCOMMAND:
    {
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case ID_CONTROL_TOP:
            g_windTop = !g_windTop;
            updateView();
            break;
        case ID_FILE_EXIT:
            shutDownApp();
            break;

        case ID_VIEW_PORTRAIT:
        case ID_VIEW_LANDSCAPE:
            onViewChangeOrientation(wmId);
            break;

        case ID_VIEW_ZOOMOUT100:
        case ID_VIEW_ZOOMOUT75:
        case ID_VIEW_ZOOMOUT50:
        case ID_VIEW_ZOOMOUT25:
            onViewZoomOut(wmId);
            break;

        case ID_CONTROL_RELOAD:
            onRestart();
            break;

        case ID_HELP_ABOUT:
            onHelpAbout();
            break;
        default:
            if (wmId >= ID_VIEW_SIZE && wmId <= ID_VIEW_SIZE + SimulatorConfig::getInstance()->getScreenSizeCount() - 1)
            {
                onViewChangeFrameSize(wmId);
            }
            break;
        }
    }
    break;
    }
    return g_oldProc(hWnd, message, wParam, lParam);
}

static INT_PTR CALLBACK AboutDialogCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void createSimulator(const std::string& viewName, int width, int height, float frameZoomFactor)
{
    auto glview = Director::getInstance()->getOpenGLView();
    if (glview)
    {
        return;
    }

    g_landscape = width > height;
    g_screenSize.width  = width;
    g_screenSize.height = height;

    glview = GLViewImpl::createWithRect(viewName, Rect(0, 0, width, height), frameZoomFactor);
    auto director = Director::getInstance();
    director->setOpenGLView(glview);

    HWND hWnd          = glview->getWin32Window();
    HMENU hMenu        = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU_COCOS));
    HMENU hSysMenu     = GetSystemMenu(hWnd, FALSE);
    HMENU hviewMenu    = GetSubMenu(hMenu, 1);
    HMENU hcontrolMenu = GetSubMenu(hMenu, 2);
    HMENU haboutMenu   = GetSubMenu(hMenu, 3);

    AppendMenu(hSysMenu, MF_SEPARATOR, 0, NULL);
    if (hSysMenu != INVALID_HANDLE_VALUE && hMenu != INVALID_HANDLE_VALUE)
    {
        AppendMenu(hSysMenu, MF_POPUP, (UINT)(size_t)hviewMenu, TEXT("View"));
        AppendMenu(hSysMenu, MF_POPUP, (UINT)(size_t)hcontrolMenu, TEXT("Control"));
        AppendMenu(hSysMenu, MF_POPUP, (UINT)(size_t)haboutMenu, TEXT("About"));
    }
    
    createViewMenu();
    updateMenu();

    g_oldProc = (WNDPROC)SetWindowLong(hWnd, GWLP_WNDPROC, (LONG)(size_t)SNewWndProc);
    if (g_oldProc == 0)
    {
        CCLOG("SetWindowLong NewWndProc Error:%d\n", GetLastError());
    }
}
