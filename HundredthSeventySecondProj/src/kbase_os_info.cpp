/*
 @ 0xCCCCCCCC
*/

#include "kbase_os_info.h"

namespace kbase {

// static
OSInfo* OSInfo::GetInstance()
{
    static OSInfo instance;
    return &instance;
}

}   // namespace kbase
