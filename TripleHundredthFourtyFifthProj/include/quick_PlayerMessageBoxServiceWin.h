
#ifndef __PLAYER_MESSAGEBOX_SERVICE_WIN_H_
#define __PLAYER_MESSAGEBOX_SERVICE_WIN_H_

#include <string>

#include "quick_PlayerStdafx.h"
#include "cocos_cocos2d.h"
#include "quick_PlayerMessageBoxServiceProtocol.h"

PLAYER_NS_BEGIN

class PlayerMessageBoxServiceWin : public PlayerMessageBoxServiceProtocol
{
public:
    PlayerMessageBoxServiceWin(HWND hwnd);

    virtual int showMessageBox(const std::string &title,
                               const std::string &message,
                               int buttonsType = BUTTONS_OK);

protected:
    HWND _hwnd;
};

PLAYER_NS_END

#endif // __PLAYER_MESSAGEBOX_SERVICE_WIN_H_
