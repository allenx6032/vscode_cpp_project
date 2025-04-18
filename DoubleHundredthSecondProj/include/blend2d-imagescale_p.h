// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#ifndef BLEND2D_IMAGESCALE_P_H_INCLUDED
#define BLEND2D_IMAGESCALE_P_H_INCLUDED

#include "blend2d-geometry.h"
#include "blend2d-image.h"

//! \cond INTERNAL
//! \addtogroup blend2d_internal
//! \{

namespace bl {

//! Low-level image scaling context.
class ImageScaleContext {
public:
  enum Dir : uint32_t {
    kDirHorz = 0,
    kDirVert = 1
  };

  struct Record {
    uint32_t pos;
    uint32_t count;
  };

  struct Data {
    int dstSize[2];
    int srcSize[2];
    int kernelSize[2];
    int isUnbound[2];

    double scale[2];
    double factor[2];
    double radius[2];

    int32_t* weightList[2];
    Record* recordList[2];
  };

  Data* data;

  BL_INLINE ImageScaleContext() noexcept
    : data(nullptr) {}

  BL_INLINE ~ImageScaleContext() noexcept { reset(); }

  BL_INLINE bool isInitialized() const noexcept { return data != nullptr; }

  BL_INLINE int dstWidth() const noexcept { return data->dstSize[kDirHorz]; }
  BL_INLINE int dstHeight() const noexcept { return data->dstSize[kDirVert]; }

  BL_INLINE int srcWidth() const noexcept { return data->srcSize[kDirHorz]; }
  BL_INLINE int srcHeight() const noexcept { return data->srcSize[kDirVert]; }

  BL_HIDDEN BLResult reset() noexcept;
  BL_HIDDEN BLResult create(const BLSizeI& to, const BLSizeI& from, uint32_t filter) noexcept;

  BL_HIDDEN BLResult processHorzData(uint8_t* dstLine, intptr_t dstStride, const uint8_t* srcLine, intptr_t srcStride, uint32_t format) const noexcept;
  BL_HIDDEN BLResult processVertData(uint8_t* dstLine, intptr_t dstStride, const uint8_t* srcLine, intptr_t srcStride, uint32_t format) const noexcept;
};

} // {bl}

//! \}
//! \endcond

#endif // BLEND2D_IMAGESCALE_P_H_INCLUDED
