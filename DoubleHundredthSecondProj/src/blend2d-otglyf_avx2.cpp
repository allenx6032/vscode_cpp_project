// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include "blend2d-api-build_p.h"
#ifdef BL_TARGET_OPT_AVX2

#include "blend2d-otglyfsimdimpl_p.h"

namespace bl {
namespace OpenType {
namespace GlyfImpl {

BLResult BL_CDECL getGlyphOutlines_AVX2(
  const BLFontFaceImpl* faceI_,
  BLGlyphId glyphId,
  const BLMatrix2D* transform,
  BLPath* out,
  size_t* contourCountOut,
  ScopedBuffer* tmpBuffer) noexcept {

  return getGlyphOutlinesSimdImpl(faceI_, glyphId, transform, out, contourCountOut, tmpBuffer);
}

} // {GlyfImpl}
} // {OpenType}
} // {bl}

#endif // BL_BUILD_OPT_AVX2
