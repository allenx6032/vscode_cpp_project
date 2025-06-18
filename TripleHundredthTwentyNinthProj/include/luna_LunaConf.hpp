#pragma once
#include "luna_Configuration.hpp"
#include "luna_Defines.hpp"

namespace luna_conf {
    static Configuration main(getLunaResource("luna.cfg"));
    static Configuration lang(getLunaResource("lang/" + main.get("lang") + ".lang"));
}