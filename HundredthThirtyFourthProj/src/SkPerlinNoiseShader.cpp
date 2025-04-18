/*
 * Copyright 2013 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkDither.h"
#include "SkPerlinNoiseShader.h"
#include "SkColorFilter.h"
#include "SkReadBuffer.h"
#include "SkWriteBuffer.h"
#include "SkShader.h"
#include "SkUnPreMultiply.h"
#include "SkString.h"

#if SK_SUPPORT_GPU
#include "GrContext.h"
#include "GrCoordTransform.h"
#include "GrGLEffect.h"
#include "GrGLProgramBuilder.h"
#include "GrTBackendEffectFactory.h"
#include "SkGr.h"
#endif

static const int kBlockSize = 256;
static const int kBlockMask = kBlockSize - 1;
static const int kPerlinNoise = 4096;
static const int kRandMaximum = SK_MaxS32; // 2**31 - 1

namespace {

// noiseValue is the color component's value (or color)
// limitValue is the maximum perlin noise array index value allowed
// newValue is the current noise dimension (either width or height)
inline int checkNoise(int noiseValue, int limitValue, int newValue) {
    // If the noise value would bring us out of bounds of the current noise array while we are
    // stiching noise tiles together, wrap the noise around the current dimension of the noise to
    // stay within the array bounds in a continuous fashion (so that tiling lines are not visible)
    if (noiseValue >= limitValue) {
        noiseValue -= newValue;
    }
    return noiseValue;
}

inline SkScalar smoothCurve(SkScalar t) {
    static const SkScalar SK_Scalar3 = 3.0f;

    // returns t * t * (3 - 2 * t)
    return SkScalarMul(SkScalarSquare(t), SK_Scalar3 - 2 * t);
}

bool perlin_noise_type_is_valid(SkPerlinNoiseShader::Type type) {
    return (SkPerlinNoiseShader::kFractalNoise_Type == type) ||
           (SkPerlinNoiseShader::kTurbulence_Type == type);
}

} // end namespace

struct SkPerlinNoiseShader::StitchData {
    StitchData()
      : fWidth(0)
      , fWrapX(0)
      , fHeight(0)
      , fWrapY(0)
    {}

    bool operator==(const StitchData& other) const {
        return fWidth == other.fWidth &&
               fWrapX == other.fWrapX &&
               fHeight == other.fHeight &&
               fWrapY == other.fWrapY;
    }

    int fWidth; // How much to subtract to wrap for stitching.
    int fWrapX; // Minimum value to wrap.
    int fHeight;
    int fWrapY;
};

struct SkPerlinNoiseShader::PaintingData {
    PaintingData(const SkISize& tileSize, SkScalar seed,
                 SkScalar baseFrequencyX, SkScalar baseFrequencyY,
                 const SkMatrix& matrix)
    {
        SkVector wavelength = SkVector::Make(SkScalarInvert(baseFrequencyX),
                                             SkScalarInvert(baseFrequencyY));
        matrix.mapVectors(&wavelength, 1);
        fBaseFrequency.fX = SkScalarInvert(wavelength.fX);
        fBaseFrequency.fY = SkScalarInvert(wavelength.fY);
        SkVector sizeVec = SkVector::Make(SkIntToScalar(tileSize.fWidth),
                                          SkIntToScalar(tileSize.fHeight));
        matrix.mapVectors(&sizeVec, 1);
        fTileSize.fWidth = SkScalarRoundToInt(sizeVec.fX);
        fTileSize.fHeight = SkScalarRoundToInt(sizeVec.fY);
        this->init(seed);
        if (!fTileSize.isEmpty()) {
            this->stitch();
        }

#if SK_SUPPORT_GPU
        fPermutationsBitmap.setInfo(SkImageInfo::MakeA8(kBlockSize, 1));
        fPermutationsBitmap.setPixels(fLatticeSelector);

        fNoiseBitmap.setInfo(SkImageInfo::MakeN32Premul(kBlockSize, 4));
        fNoiseBitmap.setPixels(fNoise[0][0]);
#endif
    }

    int         fSeed;
    uint8_t     fLatticeSelector[kBlockSize];
    uint16_t    fNoise[4][kBlockSize][2];
    SkPoint     fGradient[4][kBlockSize];
    SkISize     fTileSize;
    SkVector    fBaseFrequency;
    StitchData  fStitchDataInit;

private:

#if SK_SUPPORT_GPU
    SkBitmap   fPermutationsBitmap;
    SkBitmap   fNoiseBitmap;
#endif

    inline int random()  {
        static const int gRandAmplitude = 16807; // 7**5; primitive root of m
        static const int gRandQ = 127773; // m / a
        static const int gRandR = 2836; // m % a

        int result = gRandAmplitude * (fSeed % gRandQ) - gRandR * (fSeed / gRandQ);
        if (result <= 0)
            result += kRandMaximum;
        fSeed = result;
        return result;
    }

    // Only called once. Could be part of the constructor.
    void init(SkScalar seed)
    {
        static const SkScalar gInvBlockSizef = SkScalarInvert(SkIntToScalar(kBlockSize));

        // According to the SVG spec, we must truncate (not round) the seed value.
        fSeed = SkScalarTruncToInt(seed);
        // The seed value clamp to the range [1, kRandMaximum - 1].
        if (fSeed <= 0) {
            fSeed = -(fSeed % (kRandMaximum - 1)) + 1;
        }
        if (fSeed > kRandMaximum - 1) {
            fSeed = kRandMaximum - 1;
        }
        for (int channel = 0; channel < 4; ++channel) {
            for (int i = 0; i < kBlockSize; ++i) {
                fLatticeSelector[i] = i;
                fNoise[channel][i][0] = (random() % (2 * kBlockSize));
                fNoise[channel][i][1] = (random() % (2 * kBlockSize));
            }
        }
        for (int i = kBlockSize - 1; i > 0; --i) {
            int k = fLatticeSelector[i];
            int j = random() % kBlockSize;
            SkASSERT(j >= 0);
            SkASSERT(j < kBlockSize);
            fLatticeSelector[i] = fLatticeSelector[j];
            fLatticeSelector[j] = k;
        }

        // Perform the permutations now
        {
            // Copy noise data
            uint16_t noise[4][kBlockSize][2];
            for (int i = 0; i < kBlockSize; ++i) {
                for (int channel = 0; channel < 4; ++channel) {
                    for (int j = 0; j < 2; ++j) {
                        noise[channel][i][j] = fNoise[channel][i][j];
                    }
                }
            }
            // Do permutations on noise data
            for (int i = 0; i < kBlockSize; ++i) {
                for (int channel = 0; channel < 4; ++channel) {
                    for (int j = 0; j < 2; ++j) {
                        fNoise[channel][i][j] = noise[channel][fLatticeSelector[i]][j];
                    }
                }
            }
        }

        // Half of the largest possible value for 16 bit unsigned int
        static const SkScalar gHalfMax16bits = 32767.5f;

        // Compute gradients from permutated noise data
        for (int channel = 0; channel < 4; ++channel) {
            for (int i = 0; i < kBlockSize; ++i) {
                fGradient[channel][i] = SkPoint::Make(
                    SkScalarMul(SkIntToScalar(fNoise[channel][i][0] - kBlockSize),
                                gInvBlockSizef),
                    SkScalarMul(SkIntToScalar(fNoise[channel][i][1] - kBlockSize),
                                gInvBlockSizef));
                fGradient[channel][i].normalize();
                // Put the normalized gradient back into the noise data
                fNoise[channel][i][0] = SkScalarRoundToInt(SkScalarMul(
                    fGradient[channel][i].fX + SK_Scalar1, gHalfMax16bits));
                fNoise[channel][i][1] = SkScalarRoundToInt(SkScalarMul(
                    fGradient[channel][i].fY + SK_Scalar1, gHalfMax16bits));
            }
        }
    }

    // Only called once. Could be part of the constructor.
    void stitch() {
        SkScalar tileWidth  = SkIntToScalar(fTileSize.width());
        SkScalar tileHeight = SkIntToScalar(fTileSize.height());
        SkASSERT(tileWidth > 0 && tileHeight > 0);
        // When stitching tiled turbulence, the frequencies must be adjusted
        // so that the tile borders will be continuous.
        if (fBaseFrequency.fX) {
            SkScalar lowFrequencx =
                SkScalarFloorToScalar(tileWidth * fBaseFrequency.fX) / tileWidth;
            SkScalar highFrequencx =
                SkScalarCeilToScalar(tileWidth * fBaseFrequency.fX) / tileWidth;
            // BaseFrequency should be non-negative according to the standard.
            if (SkScalarDiv(fBaseFrequency.fX, lowFrequencx) <
                SkScalarDiv(highFrequencx, fBaseFrequency.fX)) {
                fBaseFrequency.fX = lowFrequencx;
            } else {
                fBaseFrequency.fX = highFrequencx;
            }
        }
        if (fBaseFrequency.fY) {
            SkScalar lowFrequency =
                SkScalarFloorToScalar(tileHeight * fBaseFrequency.fY) / tileHeight;
            SkScalar highFrequency =
                SkScalarCeilToScalar(tileHeight * fBaseFrequency.fY) / tileHeight;
            if (SkScalarDiv(fBaseFrequency.fY, lowFrequency) <
                SkScalarDiv(highFrequency, fBaseFrequency.fY)) {
                fBaseFrequency.fY = lowFrequency;
            } else {
                fBaseFrequency.fY = highFrequency;
            }
        }
        // Set up TurbulenceInitial stitch values.
        fStitchDataInit.fWidth  =
            SkScalarRoundToInt(tileWidth * fBaseFrequency.fX);
        fStitchDataInit.fWrapX  = kPerlinNoise + fStitchDataInit.fWidth;
        fStitchDataInit.fHeight =
            SkScalarRoundToInt(tileHeight * fBaseFrequency.fY);
        fStitchDataInit.fWrapY  = kPerlinNoise + fStitchDataInit.fHeight;
    }

public:

#if SK_SUPPORT_GPU
    const SkBitmap& getPermutationsBitmap() const { return fPermutationsBitmap; }

    const SkBitmap& getNoiseBitmap() const { return fNoiseBitmap; }
#endif
};

SkShader* SkPerlinNoiseShader::CreateFractalNoise(SkScalar baseFrequencyX, SkScalar baseFrequencyY,
                                                  int numOctaves, SkScalar seed,
                                                  const SkISize* tileSize) {
    return SkNEW_ARGS(SkPerlinNoiseShader, (kFractalNoise_Type, baseFrequencyX, baseFrequencyY,
                                            numOctaves, seed, tileSize));
}

SkShader* SkPerlinNoiseShader::CreateTurbulence(SkScalar baseFrequencyX, SkScalar baseFrequencyY,
                                              int numOctaves, SkScalar seed,
                                              const SkISize* tileSize) {
    return SkNEW_ARGS(SkPerlinNoiseShader, (kTurbulence_Type, baseFrequencyX, baseFrequencyY,
                                            numOctaves, seed, tileSize));
}

SkPerlinNoiseShader::SkPerlinNoiseShader(SkPerlinNoiseShader::Type type,
                                         SkScalar baseFrequencyX,
                                         SkScalar baseFrequencyY,
                                         int numOctaves,
                                         SkScalar seed,
                                         const SkISize* tileSize)
  : fType(type)
  , fBaseFrequencyX(baseFrequencyX)
  , fBaseFrequencyY(baseFrequencyY)
  , fNumOctaves(numOctaves > 255 ? 255 : numOctaves/*[0,255] octaves allowed*/)
  , fSeed(seed)
  , fTileSize(NULL == tileSize ? SkISize::Make(0, 0) : *tileSize)
  , fStitchTiles(!fTileSize.isEmpty())
{
    SkASSERT(numOctaves >= 0 && numOctaves < 256);
}

#ifdef SK_SUPPORT_LEGACY_DEEPFLATTENING
SkPerlinNoiseShader::SkPerlinNoiseShader(SkReadBuffer& buffer) : INHERITED(buffer) {
    fType           = (SkPerlinNoiseShader::Type) buffer.readInt();
    fBaseFrequencyX = buffer.readScalar();
    fBaseFrequencyY = buffer.readScalar();
    fNumOctaves     = buffer.readInt();
    fSeed           = buffer.readScalar();
    fStitchTiles    = buffer.readBool();
    fTileSize.fWidth  = buffer.readInt();
    fTileSize.fHeight = buffer.readInt();
    buffer.validate(perlin_noise_type_is_valid(fType) &&
                    (fNumOctaves >= 0) && (fNumOctaves <= 255) &&
                    (fStitchTiles != fTileSize.isEmpty()));
}
#endif

SkPerlinNoiseShader::~SkPerlinNoiseShader() {
}

SkFlattenable* SkPerlinNoiseShader::CreateProc(SkReadBuffer& buffer) {
    Type type = (Type)buffer.readInt();
    SkScalar freqX = buffer.readScalar();
    SkScalar freqY = buffer.readScalar();
    int octaves = buffer.readInt();
    SkScalar seed = buffer.readScalar();
    SkISize tileSize;
    tileSize.fWidth = buffer.readInt();
    tileSize.fHeight = buffer.readInt();

    switch (type) {
        case kFractalNoise_Type:
            return SkPerlinNoiseShader::CreateFractalNoise(freqX, freqY, octaves, seed, &tileSize);
        case kTurbulence_Type:
            return SkPerlinNoiseShader::CreateTubulence(freqX, freqY, octaves, seed, &tileSize);
        default:
            return NULL;
    }
}

void SkPerlinNoiseShader::flatten(SkWriteBuffer& buffer) const {
    buffer.writeInt((int) fType);
    buffer.writeScalar(fBaseFrequencyX);
    buffer.writeScalar(fBaseFrequencyY);
    buffer.writeInt(fNumOctaves);
    buffer.writeScalar(fSeed);
    buffer.writeInt(fTileSize.fWidth);
    buffer.writeInt(fTileSize.fHeight);
}

SkScalar SkPerlinNoiseShader::PerlinNoiseShaderContext::noise2D(
        int channel, const StitchData& stitchData, const SkPoint& noiseVector) const {
    struct Noise {
        int noisePositionIntegerValue;
        int nextNoisePositionIntegerValue;
        SkScalar noisePositionFractionValue;
        Noise(SkScalar component)
        {
            SkScalar position = component + kPerlinNoise;
            noisePositionIntegerValue = SkScalarFloorToInt(position);
            noisePositionFractionValue = position - SkIntToScalar(noisePositionIntegerValue);
            nextNoisePositionIntegerValue = noisePositionIntegerValue + 1;
        }
    };
    Noise noiseX(noiseVector.x());
    Noise noiseY(noiseVector.y());
    SkScalar u, v;
    const SkPerlinNoiseShader& perlinNoiseShader = static_cast<const SkPerlinNoiseShader&>(fShader);
    // If stitching, adjust lattice points accordingly.
    if (perlinNoiseShader.fStitchTiles) {
        noiseX.noisePositionIntegerValue =
            checkNoise(noiseX.noisePositionIntegerValue, stitchData.fWrapX, stitchData.fWidth);
        noiseY.noisePositionIntegerValue =
            checkNoise(noiseY.noisePositionIntegerValue, stitchData.fWrapY, stitchData.fHeight);
        noiseX.nextNoisePositionIntegerValue =
            checkNoise(noiseX.nextNoisePositionIntegerValue, stitchData.fWrapX, stitchData.fWidth);
        noiseY.nextNoisePositionIntegerValue =
            checkNoise(noiseY.nextNoisePositionIntegerValue, stitchData.fWrapY, stitchData.fHeight);
    }
    noiseX.noisePositionIntegerValue &= kBlockMask;
    noiseY.noisePositionIntegerValue &= kBlockMask;
    noiseX.nextNoisePositionIntegerValue &= kBlockMask;
    noiseY.nextNoisePositionIntegerValue &= kBlockMask;
    int i =
        fPaintingData->fLatticeSelector[noiseX.noisePositionIntegerValue];
    int j =
        fPaintingData->fLatticeSelector[noiseX.nextNoisePositionIntegerValue];
    int b00 = (i + noiseY.noisePositionIntegerValue) & kBlockMask;
    int b10 = (j + noiseY.noisePositionIntegerValue) & kBlockMask;
    int b01 = (i + noiseY.nextNoisePositionIntegerValue) & kBlockMask;
    int b11 = (j + noiseY.nextNoisePositionIntegerValue) & kBlockMask;
    SkScalar sx = smoothCurve(noiseX.noisePositionFractionValue);
    SkScalar sy = smoothCurve(noiseY.noisePositionFractionValue);
    // This is taken 1:1 from SVG spec: http://www.w3.org/TR/SVG11/filters.html#feTurbulenceElement
    SkPoint fractionValue = SkPoint::Make(noiseX.noisePositionFractionValue,
                                          noiseY.noisePositionFractionValue); // Offset (0,0)
    u = fPaintingData->fGradient[channel][b00].dot(fractionValue);
    fractionValue.fX -= SK_Scalar1; // Offset (-1,0)
    v = fPaintingData->fGradient[channel][b10].dot(fractionValue);
    SkScalar a = SkScalarInterp(u, v, sx);
    fractionValue.fY -= SK_Scalar1; // Offset (-1,-1)
    v = fPaintingData->fGradient[channel][b11].dot(fractionValue);
    fractionValue.fX = noiseX.noisePositionFractionValue; // Offset (0,-1)
    u = fPaintingData->fGradient[channel][b01].dot(fractionValue);
    SkScalar b = SkScalarInterp(u, v, sx);
    return SkScalarInterp(a, b, sy);
}

SkScalar SkPerlinNoiseShader::PerlinNoiseShaderContext::calculateTurbulenceValueForPoint(
        int channel, StitchData& stitchData, const SkPoint& point) const {
    const SkPerlinNoiseShader& perlinNoiseShader = static_cast<const SkPerlinNoiseShader&>(fShader);
    if (perlinNoiseShader.fStitchTiles) {
        // Set up TurbulenceInitial stitch values.
        stitchData = fPaintingData->fStitchDataInit;
    }
    SkScalar turbulenceFunctionResult = 0;
    SkPoint noiseVector(SkPoint::Make(SkScalarMul(point.x(), fPaintingData->fBaseFrequency.fX),
                                      SkScalarMul(point.y(), fPaintingData->fBaseFrequency.fY)));
    SkScalar ratio = SK_Scalar1;
    for (int octave = 0; octave < perlinNoiseShader.fNumOctaves; ++octave) {
        SkScalar noise = noise2D(channel, stitchData, noiseVector);
        turbulenceFunctionResult += SkScalarDiv(
            (perlinNoiseShader.fType == kFractalNoise_Type) ? noise : SkScalarAbs(noise), ratio);
        noiseVector.fX *= 2;
        noiseVector.fY *= 2;
        ratio *= 2;
        if (perlinNoiseShader.fStitchTiles) {
            // Update stitch values
            stitchData.fWidth  *= 2;
            stitchData.fWrapX   = stitchData.fWidth + kPerlinNoise;
            stitchData.fHeight *= 2;
            stitchData.fWrapY   = stitchData.fHeight + kPerlinNoise;
        }
    }

    // The value of turbulenceFunctionResult comes from ((turbulenceFunctionResult) + 1) / 2
    // by fractalNoise and (turbulenceFunctionResult) by turbulence.
    if (perlinNoiseShader.fType == kFractalNoise_Type) {
        turbulenceFunctionResult =
            SkScalarMul(turbulenceFunctionResult, SK_ScalarHalf) + SK_ScalarHalf;
    }

    if (channel == 3) { // Scale alpha by paint value
        turbulenceFunctionResult = SkScalarMul(turbulenceFunctionResult,
            SkScalarDiv(SkIntToScalar(getPaintAlpha()), SkIntToScalar(255)));
    }

    // Clamp result
    return SkScalarPin(turbulenceFunctionResult, 0, SK_Scalar1);
}

SkPMColor SkPerlinNoiseShader::PerlinNoiseShaderContext::shade(
        const SkPoint& point, StitchData& stitchData) const {
    SkPoint newPoint;
    fMatrix.mapPoints(&newPoint, &point, 1);
    newPoint.fX = SkScalarRoundToScalar(newPoint.fX);
    newPoint.fY = SkScalarRoundToScalar(newPoint.fY);

    U8CPU rgba[4];
    for (int channel = 3; channel >= 0; --channel) {
        rgba[channel] = SkScalarFloorToInt(255 *
            calculateTurbulenceValueForPoint(channel, stitchData, newPoint));
    }
    return SkPreMultiplyARGB(rgba[3], rgba[0], rgba[1], rgba[2]);
}

SkShader::Context* SkPerlinNoiseShader::onCreateContext(const ContextRec& rec,
                                                        void* storage) const {
    return SkNEW_PLACEMENT_ARGS(storage, PerlinNoiseShaderContext, (*this, rec));
}

size_t SkPerlinNoiseShader::contextSize() const {
    return sizeof(PerlinNoiseShaderContext);
}

SkPerlinNoiseShader::PerlinNoiseShaderContext::PerlinNoiseShaderContext(
        const SkPerlinNoiseShader& shader, const ContextRec& rec)
    : INHERITED(shader, rec)
{
    SkMatrix newMatrix = *rec.fMatrix;
    newMatrix.preConcat(shader.getLocalMatrix());
    if (rec.fLocalMatrix) {
        newMatrix.preConcat(*rec.fLocalMatrix);
    }
    // This (1,1) translation is due to WebKit's 1 based coordinates for the noise
    // (as opposed to 0 based, usually). The same adjustment is in the setData() function.
    fMatrix.setTranslate(-newMatrix.getTranslateX() + SK_Scalar1, -newMatrix.getTranslateY() + SK_Scalar1);
    fPaintingData = SkNEW_ARGS(PaintingData, (shader.fTileSize, shader.fSeed, shader.fBaseFrequencyX, shader.fBaseFrequencyY, newMatrix));
}

SkPerlinNoiseShader::PerlinNoiseShaderContext::~PerlinNoiseShaderContext() {
    SkDELETE(fPaintingData);
}

void SkPerlinNoiseShader::PerlinNoiseShaderContext::shadeSpan(
        int x, int y, SkPMColor result[], int count) {
    SkPoint point = SkPoint::Make(SkIntToScalar(x), SkIntToScalar(y));
    StitchData stitchData;
    for (int i = 0; i < count; ++i) {
        result[i] = shade(point, stitchData);
        point.fX += SK_Scalar1;
    }
}

void SkPerlinNoiseShader::PerlinNoiseShaderContext::shadeSpan16(
        int x, int y, uint16_t result[], int count) {
    SkPoint point = SkPoint::Make(SkIntToScalar(x), SkIntToScalar(y));
    StitchData stitchData;
    DITHER_565_SCAN(y);
    for (int i = 0; i < count; ++i) {
        unsigned dither = DITHER_VALUE(x);
        result[i] = SkDitherRGB32To565(shade(point, stitchData), dither);
        DITHER_INC_X(x);
        point.fX += SK_Scalar1;
    }
}

/////////////////////////////////////////////////////////////////////

#if SK_SUPPORT_GPU

#include "GrTBackendEffectFactory.h"

class GrGLPerlinNoise : public GrGLEffect {
public:
    GrGLPerlinNoise(const GrBackendEffectFactory& factory,
                    const GrDrawEffect& drawEffect);
    virtual ~GrGLPerlinNoise() {}

    virtual void emitCode(GrGLProgramBuilder*,
                          const GrDrawEffect&,
                          const GrEffectKey&,
                          const char* outputColor,
                          const char* inputColor,
                          const TransformedCoordsArray&,
                          const TextureSamplerArray&) SK_OVERRIDE;

    virtual void setData(const GrGLProgramDataManager&, const GrDrawEffect&) SK_OVERRIDE;

    static inline void GenKey(const GrDrawEffect&, const GrGLCaps&, GrEffectKeyBuilder* b);

private:

    GrGLProgramDataManager::UniformHandle fStitchDataUni;
    SkPerlinNoiseShader::Type             fType;
    bool                                  fStitchTiles;
    int                                   fNumOctaves;
    GrGLProgramDataManager::UniformHandle fBaseFrequencyUni;
    GrGLProgramDataManager::UniformHandle fAlphaUni;

private:
    typedef GrGLEffect INHERITED;
};

/////////////////////////////////////////////////////////////////////

class GrPerlinNoiseEffect : public GrEffect {
public:
    static GrEffect* Create(SkPerlinNoiseShader::Type type,
                            int numOctaves, bool stitchTiles,
                            SkPerlinNoiseShader::PaintingData* paintingData,
                            GrTexture* permutationsTexture, GrTexture* noiseTexture,
                            const SkMatrix& matrix, uint8_t alpha) {
        return SkNEW_ARGS(GrPerlinNoiseEffect, (type, numOctaves, stitchTiles, paintingData,
                                                permutationsTexture, noiseTexture, matrix, alpha));
    }

    virtual ~GrPerlinNoiseEffect() {
        SkDELETE(fPaintingData);
    }

    static const char* Name() { return "PerlinNoise"; }
    virtual const GrBackendEffectFactory& getFactory() const SK_OVERRIDE {
        return GrTBackendEffectFactory<GrPerlinNoiseEffect>::getInstance();
    }
    const SkPerlinNoiseShader::StitchData& stitchData() const { return fPaintingData->fStitchDataInit; }

    SkPerlinNoiseShader::Type type() const { return fType; }
    bool stitchTiles() const { return fStitchTiles; }
    const SkVector& baseFrequency() const { return fPaintingData->fBaseFrequency; }
    int numOctaves() const { return fNumOctaves; }
    const SkMatrix& matrix() const { return fCoordTransform.getMatrix(); }
    uint8_t alpha() const { return fAlpha; }

    typedef GrGLPerlinNoise GLEffect;

private:
    virtual bool onIsEqual(const GrEffect& sBase) const SK_OVERRIDE {
        const GrPerlinNoiseEffect& s = CastEffect<GrPerlinNoiseEffect>(sBase);
        return fType == s.fType &&
               fPaintingData->fBaseFrequency == s.fPaintingData->fBaseFrequency &&
               fNumOctaves == s.fNumOctaves &&
               fStitchTiles == s.fStitchTiles &&
               fCoordTransform.getMatrix() == s.fCoordTransform.getMatrix() &&
               fAlpha == s.fAlpha &&
               fPermutationsAccess.getTexture() == s.fPermutationsAccess.getTexture() &&
               fNoiseAccess.getTexture() == s.fNoiseAccess.getTexture() &&
               fPaintingData->fStitchDataInit == s.fPaintingData->fStitchDataInit;
    }

    GrPerlinNoiseEffect(SkPerlinNoiseShader::Type type,
                        int numOctaves, bool stitchTiles,
                        SkPerlinNoiseShader::PaintingData* paintingData,
                        GrTexture* permutationsTexture, GrTexture* noiseTexture,
                        const SkMatrix& matrix, uint8_t alpha)
      : fType(type)
      , fNumOctaves(numOctaves)
      , fStitchTiles(stitchTiles)
      , fAlpha(alpha)
      , fPermutationsAccess(permutationsTexture)
      , fNoiseAccess(noiseTexture)
      , fPaintingData(paintingData) {
        this->addTextureAccess(&fPermutationsAccess);
        this->addTextureAccess(&fNoiseAccess);
        fCoordTransform.reset(kLocal_GrCoordSet, matrix);
        this->addCoordTransform(&fCoordTransform);
        this->setWillNotUseInputColor();
    }

    GR_DECLARE_EFFECT_TEST;

    SkPerlinNoiseShader::Type       fType;
    GrCoordTransform                fCoordTransform;
    int                             fNumOctaves;
    bool                            fStitchTiles;
    uint8_t                         fAlpha;
    GrTextureAccess                 fPermutationsAccess;
    GrTextureAccess                 fNoiseAccess;
    SkPerlinNoiseShader::PaintingData *fPaintingData;

    void getConstantColorComponents(GrColor*, uint32_t* validFlags) const SK_OVERRIDE {
        *validFlags = 0; // This is noise. Nothing is constant.
    }

private:
    typedef GrEffect INHERITED;
};

/////////////////////////////////////////////////////////////////////
GR_DEFINE_EFFECT_TEST(GrPerlinNoiseEffect);

GrEffect* GrPerlinNoiseEffect::TestCreate(SkRandom* random,
                                          GrContext* context,
                                          const GrDrawTargetCaps&,
                                          GrTexture**) {
    int      numOctaves = random->nextRangeU(2, 10);
    bool     stitchTiles = random->nextBool();
    SkScalar seed = SkIntToScalar(random->nextU());
    SkISize  tileSize = SkISize::Make(random->nextRangeU(4, 4096), random->nextRangeU(4, 4096));
    SkScalar baseFrequencyX = random->nextRangeScalar(0.01f,
                                                      0.99f);
    SkScalar baseFrequencyY = random->nextRangeScalar(0.01f,
                                                      0.99f);

    SkShader* shader = random->nextBool() ?
        SkPerlinNoiseShader::CreateFractalNoise(baseFrequencyX, baseFrequencyY, numOctaves, seed,
                                                stitchTiles ? &tileSize : NULL) :
        SkPerlinNoiseShader::CreateTurbulence(baseFrequencyX, baseFrequencyY, numOctaves, seed,
                                             stitchTiles ? &tileSize : NULL);

    SkPaint paint;
    GrColor paintColor;
    GrEffect* effect;
    SkAssertResult(shader->asNewEffect(context, paint, NULL, &paintColor, &effect));

    SkDELETE(shader);

    return effect;
}

GrGLPerlinNoise::GrGLPerlinNoise(const GrBackendEffectFactory& factory, const GrDrawEffect& drawEffect)
  : INHERITED (factory)
  , fType(drawEffect.castEffect<GrPerlinNoiseEffect>().type())
  , fStitchTiles(drawEffect.castEffect<GrPerlinNoiseEffect>().stitchTiles())
  , fNumOctaves(drawEffect.castEffect<GrPerlinNoiseEffect>().numOctaves()) {
}

void GrGLPerlinNoise::emitCode(GrGLProgramBuilder* builder,
                               const GrDrawEffect&,
                               const GrEffectKey& key,
                               const char* outputColor,
                               const char* inputColor,
                               const TransformedCoordsArray& coords,
                               const TextureSamplerArray& samplers) {
    sk_ignore_unused_variable(inputColor);

    GrGLFragmentShaderBuilder* fsBuilder = builder->getFragmentShaderBuilder();
    SkString vCoords = fsBuilder->ensureFSCoords2D(coords, 0);

    fBaseFrequencyUni = builder->addUniform(GrGLProgramBuilder::kFragment_Visibility,
                                            kVec2f_GrSLType, "baseFrequency");
    const char* baseFrequencyUni = builder->getUniformCStr(fBaseFrequencyUni);
    fAlphaUni = builder->addUniform(GrGLProgramBuilder::kFragment_Visibility,
                                    kFloat_GrSLType, "alpha");
    const char* alphaUni = builder->getUniformCStr(fAlphaUni);

    const char* stitchDataUni = NULL;
    if (fStitchTiles) {
        fStitchDataUni = builder->addUniform(GrGLProgramBuilder::kFragment_Visibility,
                                             kVec2f_GrSLType, "stitchData");
        stitchDataUni = builder->getUniformCStr(fStitchDataUni);
    }

    // There are 4 lines, so the center of each line is 1/8, 3/8, 5/8 and 7/8
    const char* chanCoordR  = "0.125";
    const char* chanCoordG  = "0.375";
    const char* chanCoordB  = "0.625";
    const char* chanCoordA  = "0.875";
    const char* chanCoord   = "chanCoord";
    const char* stitchData  = "stitchData";
    const char* ratio       = "ratio";
    const char* noiseVec    = "noiseVec";
    const char* noiseSmooth = "noiseSmooth";
    const char* floorVal    = "floorVal";
    const char* fractVal    = "fractVal";
    const char* uv          = "uv";
    const char* ab          = "ab";
    const char* latticeIdx  = "latticeIdx";
    const char* bcoords     = "bcoords";
    const char* lattice     = "lattice";
    const char* inc8bit     = "0.00390625";  // 1.0 / 256.0
    // This is the math to convert the two 16bit integer packed into rgba 8 bit input into a
    // [-1,1] vector and perform a dot product between that vector and the provided vector.
    const char* dotLattice  = "dot(((%s.ga + %s.rb * vec2(%s)) * vec2(2.0) - vec2(1.0)), %s);";

    // Add noise function
    static const GrGLShaderVar gPerlinNoiseArgs[] =  {
        GrGLShaderVar(chanCoord, kFloat_GrSLType),
        GrGLShaderVar(noiseVec, kVec2f_GrSLType)
    };

    static const GrGLShaderVar gPerlinNoiseStitchArgs[] =  {
        GrGLShaderVar(chanCoord, kFloat_GrSLType),
        GrGLShaderVar(noiseVec, kVec2f_GrSLType),
        GrGLShaderVar(stitchData, kVec2f_GrSLType)
    };

    SkString noiseCode;

    noiseCode.appendf("\tvec4 %s;\n", floorVal);
    noiseCode.appendf("\t%s.xy = floor(%s);\n", floorVal, noiseVec);
    noiseCode.appendf("\t%s.zw = %s.xy + vec2(1.0);\n", floorVal, floorVal);
    noiseCode.appendf("\tvec2 %s = fract(%s);\n", fractVal, noiseVec);

    // smooth curve : t * t * (3 - 2 * t)
    noiseCode.appendf("\n\tvec2 %s = %s * %s * (vec2(3.0) - vec2(2.0) * %s);",
        noiseSmooth, fractVal, fractVal, fractVal);

    // Adjust frequencies if we're stitching tiles
    if (fStitchTiles) {
        noiseCode.appendf("\n\tif(%s.x >= %s.x) { %s.x -= %s.x; }",
            floorVal, stitchData, floorVal, stitchData);
        noiseCode.appendf("\n\tif(%s.y >= %s.y) { %s.y -= %s.y; }",
            floorVal, stitchData, floorVal, stitchData);
        noiseCode.appendf("\n\tif(%s.z >= %s.x) { %s.z -= %s.x; }",
            floorVal, stitchData, floorVal, stitchData);
        noiseCode.appendf("\n\tif(%s.w >= %s.y) { %s.w -= %s.y; }",
            floorVal, stitchData, floorVal, stitchData);
    }

    // Get texture coordinates and normalize
    noiseCode.appendf("\n\t%s = fract(floor(mod(%s, 256.0)) / vec4(256.0));\n",
        floorVal, floorVal);

    // Get permutation for x
    {
        SkString xCoords("");
        xCoords.appendf("vec2(%s.x, 0.5)", floorVal);

        noiseCode.appendf("\n\tvec2 %s;\n\t%s.x = ", latticeIdx, latticeIdx);
        fsBuilder->appendTextureLookup(&noiseCode, samplers[0], xCoords.c_str(), kVec2f_GrSLType);
        noiseCode.append(".r;");
    }

    // Get permutation for x + 1
    {
        SkString xCoords("");
        xCoords.appendf("vec2(%s.z, 0.5)", floorVal);

        noiseCode.appendf("\n\t%s.y = ", latticeIdx);
        fsBuilder->appendTextureLookup(&noiseCode, samplers[0], xCoords.c_str(), kVec2f_GrSLType);
        noiseCode.append(".r;");
    }

#if defined(SK_BUILD_FOR_ANDROID)
    // Android rounding for Tegra devices, like, for example: Xoom (Tegra 2), Nexus 7 (Tegra 3).
    // The issue is that colors aren't accurate enough on Tegra devices. For example, if an 8 bit
    // value of 124 (or 0.486275 here) is entered, we can get a texture value of 123.513725
    // (or 0.484368 here). The following rounding operation prevents these precision issues from
    // affecting the result of the noise by making sure that we only have multiples of 1/255.
    // (Note that 1/255 is about 0.003921569, which is the value used here).
    noiseCode.appendf("\n\t%s = floor(%s * vec2(255.0) + vec2(0.5)) * vec2(0.003921569);",
                      latticeIdx, latticeIdx);
#endif

    // Get (x,y) coordinates with the permutated x
    noiseCode.appendf("\n\tvec4 %s = fract(%s.xyxy + %s.yyww);", bcoords, latticeIdx, floorVal);

    noiseCode.appendf("\n\n\tvec2 %s;", uv);
    // Compute u, at offset (0,0)
    {
        SkString latticeCoords("");
        latticeCoords.appendf("vec2(%s.x, %s)", bcoords, chanCoord);
        noiseCode.appendf("\n\tvec4 %s = ", lattice);
        fsBuilder->appendTextureLookup(&noiseCode, samplers[1], latticeCoords.c_str(),
            kVec2f_GrSLType);
        noiseCode.appendf(".bgra;\n\t%s.x = ", uv);
        noiseCode.appendf(dotLattice, lattice, lattice, inc8bit, fractVal);
    }

    noiseCode.appendf("\n\t%s.x -= 1.0;", fractVal);
    // Compute v, at offset (-1,0)
    {
        SkString latticeCoords("");
        latticeCoords.appendf("vec2(%s.y, %s)", bcoords, chanCoord);
        noiseCode.append("\n\tlattice = ");
        fsBuilder->appendTextureLookup(&noiseCode, samplers[1], latticeCoords.c_str(),
            kVec2f_GrSLType);
        noiseCode.appendf(".bgra;\n\t%s.y = ", uv);
        noiseCode.appendf(dotLattice, lattice, lattice, inc8bit, fractVal);
    }

    // Compute 'a' as a linear interpolation of 'u' and 'v'
    noiseCode.appendf("\n\tvec2 %s;", ab);
    noiseCode.appendf("\n\t%s.x = mix(%s.x, %s.y, %s.x);", ab, uv, uv, noiseSmooth);

    noiseCode.appendf("\n\t%s.y -= 1.0;", fractVal);
    // Compute v, at offset (-1,-1)
    {
        SkString latticeCoords("");
        latticeCoords.appendf("vec2(%s.w, %s)", bcoords, chanCoord);
        noiseCode.append("\n\tlattice = ");
        fsBuilder->appendTextureLookup(&noiseCode, samplers[1], latticeCoords.c_str(),
            kVec2f_GrSLType);
        noiseCode.appendf(".bgra;\n\t%s.y = ", uv);
        noiseCode.appendf(dotLattice, lattice, lattice, inc8bit, fractVal);
    }

    noiseCode.appendf("\n\t%s.x += 1.0;", fractVal);
    // Compute u, at offset (0,-1)
    {
        SkString latticeCoords("");
        latticeCoords.appendf("vec2(%s.z, %s)", bcoords, chanCoord);
        noiseCode.append("\n\tlattice = ");
        fsBuilder->appendTextureLookup(&noiseCode, samplers[1], latticeCoords.c_str(),
            kVec2f_GrSLType);
        noiseCode.appendf(".bgra;\n\t%s.x = ", uv);
        noiseCode.appendf(dotLattice, lattice, lattice, inc8bit, fractVal);
    }

    // Compute 'b' as a linear interpolation of 'u' and 'v'
    noiseCode.appendf("\n\t%s.y = mix(%s.x, %s.y, %s.x);", ab, uv, uv, noiseSmooth);
    // Compute the noise as a linear interpolation of 'a' and 'b'
    noiseCode.appendf("\n\treturn mix(%s.x, %s.y, %s.y);\n", ab, ab, noiseSmooth);

    SkString noiseFuncName;
    if (fStitchTiles) {
        fsBuilder->emitFunction(kFloat_GrSLType,
                                "perlinnoise", SK_ARRAY_COUNT(gPerlinNoiseStitchArgs),
                                gPerlinNoiseStitchArgs, noiseCode.c_str(), &noiseFuncName);
    } else {
        fsBuilder->emitFunction(kFloat_GrSLType,
                                "perlinnoise", SK_ARRAY_COUNT(gPerlinNoiseArgs),
                                gPerlinNoiseArgs, noiseCode.c_str(), &noiseFuncName);
    }

    // There are rounding errors if the floor operation is not performed here
    fsBuilder->codeAppendf("\n\t\tvec2 %s = floor(%s.xy) * %s;",
                           noiseVec, vCoords.c_str(), baseFrequencyUni);

    // Clear the color accumulator
    fsBuilder->codeAppendf("\n\t\t%s = vec4(0.0);", outputColor);

    if (fStitchTiles) {
        // Set up TurbulenceInitial stitch values.
        fsBuilder->codeAppendf("\n\t\tvec2 %s = %s;", stitchData, stitchDataUni);
    }

    fsBuilder->codeAppendf("\n\t\tfloat %s = 1.0;", ratio);

    // Loop over all octaves
    fsBuilder->codeAppendf("\n\t\tfor (int octave = 0; octave < %d; ++octave) {", fNumOctaves);

    fsBuilder->codeAppendf("\n\t\t\t%s += ", outputColor);
    if (fType != SkPerlinNoiseShader::kFractalNoise_Type) {
        fsBuilder->codeAppend("abs(");
    }
    if (fStitchTiles) {
        fsBuilder->codeAppendf(
            "vec4(\n\t\t\t\t%s(%s, %s, %s),\n\t\t\t\t%s(%s, %s, %s),"
                 "\n\t\t\t\t%s(%s, %s, %s),\n\t\t\t\t%s(%s, %s, %s))",
            noiseFuncName.c_str(), chanCoordR, noiseVec, stitchData,
            noiseFuncName.c_str(), chanCoordG, noiseVec, stitchData,
            noiseFuncName.c_str(), chanCoordB, noiseVec, stitchData,
            noiseFuncName.c_str(), chanCoordA, noiseVec, stitchData);
    } else {
        fsBuilder->codeAppendf(
            "vec4(\n\t\t\t\t%s(%s, %s),\n\t\t\t\t%s(%s, %s),"
                 "\n\t\t\t\t%s(%s, %s),\n\t\t\t\t%s(%s, %s))",
            noiseFuncName.c_str(), chanCoordR, noiseVec,
            noiseFuncName.c_str(), chanCoordG, noiseVec,
            noiseFuncName.c_str(), chanCoordB, noiseVec,
            noiseFuncName.c_str(), chanCoordA, noiseVec);
    }
    if (fType != SkPerlinNoiseShader::kFractalNoise_Type) {
        fsBuilder->codeAppendf(")"); // end of "abs("
    }
    fsBuilder->codeAppendf(" * %s;", ratio);

    fsBuilder->codeAppendf("\n\t\t\t%s *= vec2(2.0);", noiseVec);
    fsBuilder->codeAppendf("\n\t\t\t%s *= 0.5;", ratio);

    if (fStitchTiles) {
        fsBuilder->codeAppendf("\n\t\t\t%s *= vec2(2.0);", stitchData);
    }
    fsBuilder->codeAppend("\n\t\t}"); // end of the for loop on octaves

    if (fType == SkPerlinNoiseShader::kFractalNoise_Type) {
        // The value of turbulenceFunctionResult comes from ((turbulenceFunctionResult) + 1) / 2
        // by fractalNoise and (turbulenceFunctionResult) by turbulence.
        fsBuilder->codeAppendf("\n\t\t%s = %s * vec4(0.5) + vec4(0.5);", outputColor, outputColor);
    }

    fsBuilder->codeAppendf("\n\t\t%s.a *= %s;", outputColor, alphaUni);

    // Clamp values
    fsBuilder->codeAppendf("\n\t\t%s = clamp(%s, 0.0, 1.0);", outputColor, outputColor);

    // Pre-multiply the result
    fsBuilder->codeAppendf("\n\t\t%s = vec4(%s.rgb * %s.aaa, %s.a);\n",
                  outputColor, outputColor, outputColor, outputColor);
}

void GrGLPerlinNoise::GenKey(const GrDrawEffect& drawEffect, const GrGLCaps&, GrEffectKeyBuilder* b) {
    const GrPerlinNoiseEffect& turbulence = drawEffect.castEffect<GrPerlinNoiseEffect>();

    uint32_t key = turbulence.numOctaves();

    key = key << 3; // Make room for next 3 bits

    switch (turbulence.type()) {
        case SkPerlinNoiseShader::kFractalNoise_Type:
            key |= 0x1;
            break;
        case SkPerlinNoiseShader::kTurbulence_Type:
            key |= 0x2;
            break;
        default:
            // leave key at 0
            break;
    }

    if (turbulence.stitchTiles()) {
        key |= 0x4; // Flip the 3rd bit if tile stitching is on
    }

    b->add32(key);
}

void GrGLPerlinNoise::setData(const GrGLProgramDataManager& pdman, const GrDrawEffect& drawEffect) {
    INHERITED::setData(pdman, drawEffect);

    const GrPerlinNoiseEffect& turbulence = drawEffect.castEffect<GrPerlinNoiseEffect>();

    const SkVector& baseFrequency = turbulence.baseFrequency();
    pdman.set2f(fBaseFrequencyUni, baseFrequency.fX, baseFrequency.fY);
    pdman.set1f(fAlphaUni, SkScalarDiv(SkIntToScalar(turbulence.alpha()), SkIntToScalar(255)));

    if (turbulence.stitchTiles()) {
        const SkPerlinNoiseShader::StitchData& stitchData = turbulence.stitchData();
        pdman.set2f(fStitchDataUni, SkIntToScalar(stitchData.fWidth),
                                   SkIntToScalar(stitchData.fHeight));
    }
}

/////////////////////////////////////////////////////////////////////

bool SkPerlinNoiseShader::asNewEffect(GrContext* context, const SkPaint& paint,
                                      const SkMatrix* externalLocalMatrix, GrColor* paintColor,
                                      GrEffect** effect) const {
    SkASSERT(NULL != context);
    
    *paintColor = SkColor2GrColorJustAlpha(paint.getColor());

    SkMatrix localMatrix = this->getLocalMatrix();
    if (externalLocalMatrix) {
        localMatrix.preConcat(*externalLocalMatrix);
    }

    SkMatrix matrix = context->getMatrix();
    matrix.preConcat(localMatrix);

    if (0 == fNumOctaves) {
        SkColor clearColor = 0;
        if (kFractalNoise_Type == fType) {
            clearColor = SkColorSetARGB(paint.getAlpha() / 2, 127, 127, 127);
        }
        SkAutoTUnref<SkColorFilter> cf(SkColorFilter::CreateModeFilter(
                                                clearColor, SkXfermode::kSrc_Mode));
        *effect = cf->asNewEffect(context);
        return true;
    }

    // Either we don't stitch tiles, either we have a valid tile size
    SkASSERT(!fStitchTiles || !fTileSize.isEmpty());

    SkPerlinNoiseShader::PaintingData* paintingData = SkNEW_ARGS(PaintingData, (fTileSize, fSeed, fBaseFrequencyX, fBaseFrequencyY, matrix));
    GrTexture* permutationsTexture = GrLockAndRefCachedBitmapTexture(
        context, paintingData->getPermutationsBitmap(), NULL);
    GrTexture* noiseTexture = GrLockAndRefCachedBitmapTexture(
        context, paintingData->getNoiseBitmap(), NULL);

    SkMatrix m = context->getMatrix();
    m.setTranslateX(-localMatrix.getTranslateX() + SK_Scalar1);
    m.setTranslateY(-localMatrix.getTranslateY() + SK_Scalar1);
    if ((NULL != permutationsTexture) && (NULL != noiseTexture)) {
        *effect = GrPerlinNoiseEffect::Create(fType,
                                                fNumOctaves,
                                                fStitchTiles,
                                                paintingData,
                                                permutationsTexture, noiseTexture,
                                                m, paint.getAlpha());
    } else {
        SkDELETE(paintingData);
        *effect = NULL;
    }

    // Unlock immediately, this is not great, but we don't have a way of
    // knowing when else to unlock it currently. TODO: Remove this when
    // unref becomes the unlock replacement for all types of textures.
    if (NULL != permutationsTexture) {
        GrUnlockAndUnrefCachedBitmapTexture(permutationsTexture);
    }
    if (NULL != noiseTexture) {
        GrUnlockAndUnrefCachedBitmapTexture(noiseTexture);
    }

    return true;
}

#else

bool SkPerlinNoiseShader::asNewEffect(GrContext* context, const SkPaint& paint,
                                      const SkMatrix* externalLocalMatrix, GrColor* paintColor,
                                      GrEffect** effect) const {
    SkDEBUGFAIL("Should not call in GPU-less build");
    return false;
}

#endif

#ifndef SK_IGNORE_TO_STRING
void SkPerlinNoiseShader::toString(SkString* str) const {
    str->append("SkPerlinNoiseShader: (");

    str->append("type: ");
    switch (fType) {
        case kFractalNoise_Type:
            str->append("\"fractal noise\"");
            break;
        case kTurbulence_Type:
            str->append("\"turbulence\"");
            break;
        default:
            str->append("\"unknown\"");
            break;
    }
    str->append(" base frequency: (");
    str->appendScalar(fBaseFrequencyX);
    str->append(", ");
    str->appendScalar(fBaseFrequencyY);
    str->append(") number of octaves: ");
    str->appendS32(fNumOctaves);
    str->append(" seed: ");
    str->appendScalar(fSeed);
    str->append(" stitch tiles: ");
    str->append(fStitchTiles ? "true " : "false ");

    this->INHERITED::toString(str);

    str->append(")");
}
#endif
