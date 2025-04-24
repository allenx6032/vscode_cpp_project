
#ifndef __PLAYER_PROTOCOL_H_
#define __PLAYER_PROTOCOL_H_

#include "libsimulator_PlayerMacros.h"
#include "libsimulator_PlayerSettings.h"

#include "libsimulator_PlayerFileDialogServiceProtocol.h"
#include "libsimulator_PlayerMessageBoxServiceProtocol.h"
#include "libsimulator_PlayerMenuServiceProtocol.h"
#include "libsimulator_PlayerEditBoxServiceProtocol.h"
#include "libsimulator_PlayerTaskServiceProtocol.h"

#include "libsimulator_ProjectConfig.h"
#include "libsimulator_SimulatorExport.h"

PLAYER_NS_BEGIN

class CC_LIBSIM_DLL PlayerProtocol
{
public:
    virtual ~PlayerProtocol();

    static PlayerProtocol *getInstance();
    static void purgeInstance();

    void setPlayerSettings(const PlayerSettings &settings);
    PlayerSettings getPlayerSettings() const;

    virtual PlayerFileDialogServiceProtocol *getFileDialogService() = 0; // implemented in platform related source files
    virtual PlayerMessageBoxServiceProtocol *getMessageBoxService() = 0;
    virtual PlayerMenuServiceProtocol *getMenuService() = 0;
    virtual PlayerEditBoxServiceProtocol *getEditBoxService() = 0;
    virtual PlayerTaskServiceProtocol *getTaskService() = 0;
    
protected:
    PlayerProtocol(); // avoid create instance from outside

    PlayerSettings _settings;

private:
    static PlayerProtocol *_instance;
};

PLAYER_NS_END

#endif // __PLAYER_PROTOCOL_H_
