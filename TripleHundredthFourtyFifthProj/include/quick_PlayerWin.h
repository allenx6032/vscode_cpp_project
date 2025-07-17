#pragma once

#include "quick_PlayerStdafx.h"
#include "Resource.h"
#include "cocos_cocos2d.h"
#include "quick_PlayerAppDelegate.h"
#include "quick_ProjectConfig.h"

#include "quick_PlayerMacros.h"
#include "quick_PlayerProtocol.h"
#include "quick_PlayerMenuServiceWin.h"
#include "quick_PlayerMessageBoxServiceWin.h"
#include "quick_PlayerFileDialogServiceWin.h"
#include "quick_PlayerTaskServiceWin.h"

PLAYER_NS_BEGIN

class PlayerWin : public PlayerProtocol, public cocos2d::Ref
{
public:
    static PlayerWin *getInstance();
    virtual ~PlayerWin();
    int run();

    virtual PlayerFileDialogServiceProtocol *getFileDialogService();
    virtual PlayerMessageBoxServiceProtocol *getMessageBoxService();
    virtual PlayerMenuServiceProtocol *getMenuService();
    virtual PlayerTaskServiceProtocol *getTaskService();

    virtual void quit();
    virtual void relaunch();
    virtual void openNewPlayer();
    virtual void openNewPlayerWithProjectConfig(const ProjectConfig &config);
    virtual void openProjectWithProjectConfig(const ProjectConfig &config);

    virtual int getPositionX();
    virtual int getPositionY();
protected:
    PlayerWin();

    static PlayerWin *_instance;
    ProjectConfig _project;
    HWND _hwnd;
    HWND _hwndConsole;
    AppDelegate *_app;
    FILE *_writeDebugLogFile;

    PlayerMenuServiceWin *_menuService;
    PlayerMessageBoxServiceWin *_messageBoxService;
    PlayerFileDialogServiceWin *_fileDialogService;
    PlayerTaskServiceWin *_taskService;

    // services
    void initServices();

    // event handlers
    void onWindowClose(cocos2d::EventCustom* event);
    void onWindowResize(cocos2d::EventCustom* event);
    void onWindowScale(cocos2d::EventCustom* event);

    // 
    void loadLuaConfig();
    void registerKeyboardEvent();
    void setZoom(float frameScale);

    // debug log
    void writeDebugLog(const char *log);

    // helper
    std::string convertPathFormatToUnixStyle(const std::string& path);
    std::string getUserDocumentPath();
    std::string getApplicationExePath();
    std::string removeFilename(const std::string& path);
    std::string getUserGUID();
	char*	    convertTCharToANSI(const TCHAR* src);

    std::string _userGUID;
    static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};


PLAYER_NS_END
