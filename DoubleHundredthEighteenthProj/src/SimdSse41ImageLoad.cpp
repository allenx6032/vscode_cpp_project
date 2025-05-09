/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2025 Yermalayeu Ihar,
*               2022-2022 Souriya Trinh.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "SimdMemory.h"
#include "SimdImageLoad.h"
#include "SimdSse41.h"

#include <memory>

namespace Simd
{
#ifdef SIMD_SSE41_ENABLE
    namespace Sse41
    {
        ImagePgmTxtLoader::ImagePgmTxtLoader(const ImageLoaderParam& param)
            : Base::ImagePgmTxtLoader(param)
        {
        }

        void ImagePgmTxtLoader::SetConverters()
        {
            Base::ImagePgmTxtLoader::SetConverters();
            if (_image.width >= A)
            {
                switch (_param.format)
                {
                case SimdPixelFormatBgr24: _toAny = Sse41::GrayToBgr; break;
                case SimdPixelFormatBgra32: _toBgra = Sse41::GrayToBgra; break;
                case SimdPixelFormatRgb24: _toAny = Sse41::GrayToBgr; break;
                case SimdPixelFormatRgba32: _toBgra = Sse41::GrayToBgra; break;
                default: break;
                }
            }
        }

        //-------------------------------------------------------------------------------------------------

        ImagePgmBinLoader::ImagePgmBinLoader(const ImageLoaderParam& param)
            : Base::ImagePgmBinLoader(param)
        {
        }

        void ImagePgmBinLoader::SetConverters()
        {
            Base::ImagePgmBinLoader::SetConverters();
            if (_image.width >= A)
            {
                switch (_param.format)
                {
                case SimdPixelFormatBgr24: _toAny = Sse41::GrayToBgr; break;
                case SimdPixelFormatBgra32: _toBgra = Sse41::GrayToBgra; break;
                case SimdPixelFormatRgb24: _toAny = Sse41::GrayToBgr; break;
                case SimdPixelFormatRgba32: _toBgra = Sse41::GrayToBgra; break;
                default: break;
                }
            }
        }

        //-------------------------------------------------------------------------------------------------

        ImagePpmTxtLoader::ImagePpmTxtLoader(const ImageLoaderParam& param)
            : Base::ImagePpmTxtLoader(param)
        {
        }

        void ImagePpmTxtLoader::SetConverters()
        {
            Base::ImagePpmTxtLoader::SetConverters();
            if (_image.width >= A)
            {
                switch (_param.format)
                {
                case SimdPixelFormatGray8: _toAny = Sse41::RgbToGray; break;
                case SimdPixelFormatBgr24: _toAny = Sse41::BgrToRgb; break;
                case SimdPixelFormatBgra32: _toBgra = Sse41::RgbToBgra; break;
                case SimdPixelFormatRgba32: _toBgra = Sse41::BgrToBgra; break;
                default: break;
                }
            }
        }

        //-------------------------------------------------------------------------------------------------

        ImagePpmBinLoader::ImagePpmBinLoader(const ImageLoaderParam& param)
            : Base::ImagePpmBinLoader(param)
        {
        }

        void ImagePpmBinLoader::SetConverters()
        {
            Base::ImagePpmBinLoader::SetConverters();
            if (_image.width >= A)
            {
                switch (_param.format)
                {
                case SimdPixelFormatGray8: _toAny = Sse41::RgbToGray; break;
                case SimdPixelFormatBgr24: _toAny = Sse41::BgrToRgb; break;
                case SimdPixelFormatBgra32: _toBgra = Sse41::RgbToBgra; break;
                case SimdPixelFormatRgba32: _toBgra = Sse41::BgrToBgra; break;
                default: break;
                }
            }
        }

        //-------------------------------------------------------------------------------------------------

        ImageBmpLoader::ImageBmpLoader(const ImageLoaderParam& param)
            : Base::ImageBmpLoader(param)
        {
        }

        void ImageBmpLoader::SetConverters()
        {
            switch (_param.format)
            {
            case SimdPixelFormatGray8: _toAny = (_bpp == 32 ? Sse41::BgraToGray : Sse41::BgrToGray); break;
            case SimdPixelFormatBgr24: _toAny = (_bpp == 32 ? (ToAnyPtr)Sse41::BgraToBgr : NULL); break;
            case SimdPixelFormatRgb24: _toAny = (_bpp == 32 ? Sse41::BgraToRgb : Sse41::BgrToRgb); break;
            case SimdPixelFormatBgra32: _toBgra = (_bpp == 32 ? NULL : (ToBgraPtr)Sse41::BgrToBgra); break;
            case SimdPixelFormatRgba32:
                if (_bpp == 32)
                    _toAny = Sse41::BgraToRgba;
                else
                    _toBgra = (ToBgraPtr)Sse41::RgbToBgra;
                break;
            default: break;
            }
        }

        //-------------------------------------------------------------------------------------------------

        ImageLoader* CreateImageLoader(const ImageLoaderParam& param)
        {
            switch (param.file)
            {
            case SimdImageFilePgmTxt: return new ImagePgmTxtLoader(param);
            case SimdImageFilePgmBin: return new ImagePgmBinLoader(param);
            case SimdImageFilePpmTxt: return new ImagePpmTxtLoader(param);
            case SimdImageFilePpmBin: return new ImagePpmBinLoader(param);
            case SimdImageFilePng: return new ImagePngLoader(param);
            case SimdImageFileJpeg: return new ImageJpegLoader(param);
            case SimdImageFileBmp: return new ImageBmpLoader(param);
            default:
                return NULL;
            }
        }

        uint8_t* ImageLoadFromMemory(const uint8_t* data, size_t size, size_t* stride, size_t* width, size_t* height, SimdPixelFormatType* format)
        {
            ImageLoaderParam param(data, size, *format);
            if (param.Validate())
            {
                Holder<ImageLoader> loader(CreateImageLoader(param));
                if (loader)
                {
                    if (loader->FromStream())
                        return loader->Release(stride, width, height, format);
                }
            }
            return NULL;
        }
    }
#endif
}
