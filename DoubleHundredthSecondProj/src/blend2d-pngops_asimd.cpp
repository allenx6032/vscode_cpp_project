// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include "blend2d-api-build_p.h"
#if defined(BL_TARGET_OPT_ASIMD)

#include "blend2d-pngopssimdimpl_p.h"

namespace bl {
namespace Png {
namespace Ops {

void initFuncTable_ASIMD(FunctionTable& ft) noexcept {
  initSimdFunctions(ft);
}

} // {Ops}
} // {Png}
} // {bl}

#endif // BL_TARGET_OPT_ASIMD
