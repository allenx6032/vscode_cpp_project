// Copyright 2022 Google LLC
// SPDX-License-Identifier: BSD-2-Clause

#include "avif_internal.h"

#if defined(AVIF_LIBSHARPYUV_ENABLED)
#include <sharpyuv/sharpyuv.h>
#include <sharpyuv/sharpyuv_csp.h>

avifResult avifImageRGBToYUVLibSharpYUV(avifImage * image, const avifRGBImage * rgb, const avifReformatState * state)
{
    const SharpYuvColorSpace colorSpace = {
        state->kr, state->kb, image->depth, (state->yuvRange == AVIF_RANGE_LIMITED) ? kSharpYuvRangeLimited : kSharpYuvRangeFull
    };

    SharpYuvConversionMatrix matrix;
    // Fills in 'matrix' for the given YUVColorSpace.
    SharpYuvComputeConversionMatrix(&colorSpace, &matrix);
#if SHARPYUV_VERSION >= SHARPYUV_MAKE_VERSION(0, 4, 0)
    SharpYuvOptions options;
    SharpYuvOptionsInit(&matrix, &options);
    if (image->transferCharacteristics == AVIF_TRANSFER_CHARACTERISTICS_UNSPECIFIED) {
        // Set to sRGB for backward compatibility.
        options.transfer_type = kSharpYuvTransferFunctionSrgb;
    } else {
        options.transfer_type = (SharpYuvTransferFunctionType)image->transferCharacteristics;
    }
    const int sharpyuvRes = SharpYuvConvertWithOptions(&rgb->pixels[state->rgbOffsetBytesR],
                                                       &rgb->pixels[state->rgbOffsetBytesG],
                                                       &rgb->pixels[state->rgbOffsetBytesB],
                                                       state->rgbPixelBytes,
                                                       rgb->rowBytes,
                                                       rgb->depth,
                                                       image->yuvPlanes[AVIF_CHAN_Y],
                                                       image->yuvRowBytes[AVIF_CHAN_Y],
                                                       image->yuvPlanes[AVIF_CHAN_U],
                                                       image->yuvRowBytes[AVIF_CHAN_U],
                                                       image->yuvPlanes[AVIF_CHAN_V],
                                                       image->yuvRowBytes[AVIF_CHAN_V],
                                                       image->depth,
                                                       rgb->width,
                                                       rgb->height,
                                                       &options);
#else
    const int sharpyuvRes = SharpYuvConvert(&rgb->pixels[state->rgbOffsetBytesR],
                                            &rgb->pixels[state->rgbOffsetBytesG],
                                            &rgb->pixels[state->rgbOffsetBytesB],
                                            state->rgbPixelBytes,
                                            rgb->rowBytes,
                                            rgb->depth,
                                            image->yuvPlanes[AVIF_CHAN_Y],
                                            image->yuvRowBytes[AVIF_CHAN_Y],
                                            image->yuvPlanes[AVIF_CHAN_U],
                                            image->yuvRowBytes[AVIF_CHAN_U],
                                            image->yuvPlanes[AVIF_CHAN_V],
                                            image->yuvRowBytes[AVIF_CHAN_V],
                                            image->depth,
                                            rgb->width,
                                            rgb->height,
                                            &matrix);
#endif // SHARPYUV_VERSION >= SHARPYUV_MAKE_VERSION(0, 4, 0)
    if (!sharpyuvRes) {
        return AVIF_RESULT_REFORMAT_FAILED;
    }

    return AVIF_RESULT_OK;
}

#else

avifResult avifImageRGBToYUVLibSharpYUV(avifImage * image, const avifRGBImage * rgb, const avifReformatState * state)
{
    (void)image;
    (void)rgb;
    (void)state;
    return AVIF_RESULT_NOT_IMPLEMENTED;
}
#endif // defined(AVIF_LIBSHARPYUV_ENABLED)
