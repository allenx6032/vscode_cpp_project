// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#ifndef BLEND2D_CODEC_QOICODEC_P_H_INCLUDED
#define BLEND2D_CODEC_QOICODEC_P_H_INCLUDED

#include "blend2d-api-internal_p.h"
#include "blend2d-image_p.h"
#include "blend2d-imagecodec.h"
#include "blend2d-imagedecoder.h"
#include "blend2d-imageencoder.h"
#include "blend2d-runtime_p.h"

//! \cond INTERNAL
//! \addtogroup blend2d_codec_impl
//! \{

namespace bl {
namespace Qoi {

BL_HIDDEN void qoiCodecOnInit(BLRuntimeContext* rt, BLArray<BLImageCodec>* codecs) noexcept;

} // {Qoi}
} // {bl}

struct BLQoiDecoderImpl : public BLImageDecoderImpl {
  //! Decoder image information.
  BLImageInfo imageInfo;
};

struct BLQoiEncoderImpl : public BLImageEncoderImpl {
};

struct BLQoiCodecImpl : public BLImageCodecImpl {
};

//! \}
//! \endcond

#endif // BLEND2D_CODEC_QOICODEC_P_H_INCLUDED
