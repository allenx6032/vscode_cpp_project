// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#ifndef ME_DEBUGIMPL_HPP
#define ME_DEBUGIMPL_HPP

#include "FallingSand_core.hpp"
#include "FallingSand_macros.hpp"
#include "FallingSand_imgui_impl.hpp"

namespace ME {

struct AppMetaData {
    std::string platform;
    std::string compiler;
    std::string compiler_version;
    std::string cpp;
};

int ME_buildnum(void);
AppMetaData ME_metadata(void);

}  // namespace ME

#endif