// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include "blend2d-api-build_p.h"
#if !defined(BL_BUILD_NO_JIT)

#include "blend2d-pipecompiler_p.h"
#include "blend2d-pipepart_p.h"
#include "blend2d-tables_p.h"

namespace bl {
namespace Pipeline {
namespace JIT {

PipePart::PipePart(PipeCompiler* pc, PipePartType partType) noexcept
  : pc(pc),
    cc(pc->cc),
    ct(commonTable),
    _partType(partType) {}

void PipePart::preparePart() noexcept {}

} // {JIT}
} // {Pipeline}
} // {bl}

#endif // !BL_BUILD_NO_JIT
