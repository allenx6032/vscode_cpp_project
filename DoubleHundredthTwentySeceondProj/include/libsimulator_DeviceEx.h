#pragma once

#include <string>
#include "libsimulator_PlayerMacros.h"
#include "libsimulator_SimulatorExport.h"

PLAYER_NS_BEGIN

class CC_LIBSIM_DLL DeviceEx
{
public:
    static DeviceEx *getInstance();

    std::string getCurrentUILangName();
    std::string getUserGUID();

private:
    DeviceEx();
    void init();
    void makeUILangName();
    std::string makeUserGUID();

    std::string _uiLangName;
    std::string _userGUID;
};

PLAYER_NS_END