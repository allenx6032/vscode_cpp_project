
#ifndef __PLAYER_MAC_H_
#define __PLAYER_MAC_H_

#include "libsimulator_PlayerProtocol.h"

#include "libsimulator_PlayerEditBoxServiceMac.h"
#include "libsimulator_PlayerFileDialogServiceMac.h"
#include "libsimulator_PlayerMenuServiceMac.h"
#include "libsimulator_PlayerMessageBoxServiceMac.h"
#include "libsimulator_PlayerTaskServiceMac.h"

#include "libsimulator_PlayerUtils.h"

#include "libsimulator_ProjectConfig.h"
PLAYER_NS_BEGIN

class PlayerMac : public PlayerProtocol
{
public:
    static PlayerMac *create();
    virtual ~PlayerMac();

    virtual PlayerFileDialogServiceProtocol *getFileDialogService();
    virtual PlayerMessageBoxServiceProtocol *getMessageBoxService();
    virtual PlayerMenuServiceProtocol *getMenuService();
    virtual PlayerEditBoxServiceProtocol *getEditBoxService();
    virtual PlayerTaskServiceProtocol *getTaskService();

protected:
    PlayerMac();

    PlayerMenuServiceMac       *_menuService;
    PlayerMessageBoxServiceMac *_messageBoxService;
    PlayerFileDialogServiceMac *_fileDialogService;
    PlayerEditBoxServiceMac    *_editBoxService;
    PlayerTaskServiceMac       *_taskService;
    id _appController;
};

PLAYER_NS_END

#endif // __PLAYER_MAC_H_
