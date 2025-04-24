#pragma once

#include "cocos2d.h"

#include "libsimulator_PlayerMacros.h"
#include "libsimulator_PlayerProtocol.h"
#include "libsimulator_PlayerMenuServiceWin.h"
#include "libsimulator_PlayerMessageBoxServiceWin.h"
#include "libsimulator_PlayerFileDialogServiceWin.h"
#include "libsimulator_PlayerEditBoxServiceWin.h"
#include "libsimulator_PlayerTaskServiceWin.h"
#include "libsimulator_SimulatorExport.h"

PLAYER_NS_BEGIN

class CC_LIBSIM_DLL PlayerWin : public PlayerProtocol, public cocos2d::Ref
{
public:
    static PlayerWin *createWithHwnd(HWND hWnd);
    virtual ~PlayerWin();

    virtual PlayerFileDialogServiceProtocol *getFileDialogService();
    virtual PlayerMessageBoxServiceProtocol *getMessageBoxService();
    virtual PlayerMenuServiceProtocol *getMenuService();
    virtual PlayerEditBoxServiceProtocol *getEditBoxService();
    virtual PlayerTaskServiceProtocol *getTaskService();

protected:
    PlayerWin();
    
    // services
    void initServices();

    PlayerMenuServiceWin *_menuService;
    PlayerMessageBoxServiceWin *_messageBoxService;
    PlayerFileDialogServiceWin *_fileDialogService;
    PlayerEditBoxServiceWin *_editboxService;
    PlayerTaskServiceWin *_taskService;

    HWND _hwnd;
};


PLAYER_NS_END
