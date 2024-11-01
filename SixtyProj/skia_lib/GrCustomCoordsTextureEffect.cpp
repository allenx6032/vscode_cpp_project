/*
 * Copyright 2013 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "GrGLProgramBuilder.h"
#include "GrCustomCoordsTextureEffect.h"
#include "GrGLEffect.h"
#include "GrGLSL.h"
#include "GrGLTexture.h"
#include "GrGLVertexEffect.h"
#include "GrTBackendEffectFactory.h"
#include "GrTexture.h"

class GrGLCustomCoordsTextureEffect : public GrGLVertexEffect {
public:
    GrGLCustomCoordsTextureEffect(const GrBackendEffectFactory& factory, const GrDrawEffect& drawEffect)
        : INHERITED (factory) {}

    virtual void emitCode(GrGLFullProgramBuilder* builder,
                          const GrDrawEffect& drawEffect,
                          const GrEffectKey& key,
                          const char* outputColor,
                          const char* inputColor,
                          const TransformedCoordsArray&,
                          const TextureSamplerArray& samplers) SK_OVERRIDE {
        SkASSERT(1 == drawEffect.castEffect<GrCustomCoordsTextureEffect>().numVertexAttribs());

        SkString fsCoordName;
        const char* vsVaryingName;
        const char* fsVaryingNamePtr;
        builder->addVarying(kVec2f_GrSLType, "textureCoords", &vsVaryingName, &fsVaryingNamePtr);
        fsCoordName = fsVaryingNamePtr;

        GrGLVertexShaderBuilder* vsBuilder = builder->getVertexShaderBuilder();
        const SkString* attr0Name =
            vsBuilder->getEffectAttributeName(drawEffect.getVertexAttribIndices()[0]);
        vsBuilder->codeAppendf("\t%s = %s;\n", vsVaryingName, attr0Name->c_str());

        GrGLFragmentShaderBuilder* fsBuilder = builder->getFragmentShaderBuilder();
        fsBuilder->codeAppendf("\t%s = ", outputColor);
        fsBuilder->appendTextureLookupAndModulate(inputColor,
                                                  samplers[0],
                                                  fsCoordName.c_str(),
                                                  kVec2f_GrSLType);
        fsBuilder->codeAppend(";\n");
    }

    virtual void setData(const GrGLProgramDataManager& pdman,
                         const GrDrawEffect& drawEffect) SK_OVERRIDE {}

private:
    typedef GrGLVertexEffect INHERITED;
};

///////////////////////////////////////////////////////////////////////////////

GrCustomCoordsTextureEffect::GrCustomCoordsTextureEffect(GrTexture* texture,
                                                         const GrTextureParams& params)
    : fTextureAccess(texture, params) {
    this->addTextureAccess(&fTextureAccess);
    this->addVertexAttrib(kVec2f_GrSLType);
}

bool GrCustomCoordsTextureEffect::onIsEqual(const GrEffect& other) const {
    const GrCustomCoordsTextureEffect& cte = CastEffect<GrCustomCoordsTextureEffect>(other);
    return fTextureAccess == cte.fTextureAccess;
}

void GrCustomCoordsTextureEffect::getConstantColorComponents(GrColor* color,
                                                             uint32_t* validFlags) const {
    if ((*validFlags & kA_GrColorComponentFlag) && 0xFF == GrColorUnpackA(*color) &&
        GrPixelConfigIsOpaque(this->texture(0)->config())) {
        *validFlags = kA_GrColorComponentFlag;
    } else {
        *validFlags = 0;
    }
}

const GrBackendEffectFactory& GrCustomCoordsTextureEffect::getFactory() const {
    return GrTBackendEffectFactory<GrCustomCoordsTextureEffect>::getInstance();
}

///////////////////////////////////////////////////////////////////////////////

GR_DEFINE_EFFECT_TEST(GrCustomCoordsTextureEffect);

GrEffect* GrCustomCoordsTextureEffect::TestCreate(SkRandom* random,
                                                  GrContext*,
                                                  const GrDrawTargetCaps&,
                                                  GrTexture* textures[]) {
    int texIdx = random->nextBool() ? GrEffectUnitTest::kSkiaPMTextureIdx :
                                      GrEffectUnitTest::kAlphaTextureIdx;
    static const SkShader::TileMode kTileModes[] = {
        SkShader::kClamp_TileMode,
        SkShader::kRepeat_TileMode,
        SkShader::kMirror_TileMode,
    };
    SkShader::TileMode tileModes[] = {
        kTileModes[random->nextULessThan(SK_ARRAY_COUNT(kTileModes))],
        kTileModes[random->nextULessThan(SK_ARRAY_COUNT(kTileModes))],
    };
    GrTextureParams params(tileModes, random->nextBool() ? GrTextureParams::kBilerp_FilterMode :
                                                           GrTextureParams::kNone_FilterMode);

    return GrCustomCoordsTextureEffect::Create(textures[texIdx], params);
}
