
#ifndef __PLAYER_PROTOCOL_H_
#define __PLAYER_PROTOCOL_H_

#include "quick_PlayerMacros.h"
#include "quick_PlayerSettings.h"

#include "quick_PlayerFileDialogServiceProtocol.h"
#include "quick_PlayerMessageBoxServiceProtocol.h"
#include "quick_PlayerMenuServiceProtocol.h"
#include "quick_PlayerTaskServiceProtocol.h"

#include "quick_ProjectConfig.h"

PLAYER_NS_BEGIN

class PlayerProtocol
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
    virtual PlayerTaskServiceProtocol *getTaskService() = 0;

    // player function

    virtual void quit() = 0;
    virtual void relaunch() = 0;
    virtual void openNewPlayer() = 0;
    virtual void openNewPlayerWithProjectConfig(const ProjectConfig &config) = 0;
    virtual void openProjectWithProjectConfig(const ProjectConfig &config) = 0;
    
    // window info
    virtual int getPositionX() = 0;
    virtual int getPositionY() = 0;
    
    // register event
    void registerGLFWEvents();
    
protected:
    PlayerProtocol(); // avoid create instance from outside

    PlayerSettings _settings;

private:
    static PlayerProtocol *_instance;
};

PLAYER_NS_END

#endif // __PLAYER_PROTOCOL_H_
