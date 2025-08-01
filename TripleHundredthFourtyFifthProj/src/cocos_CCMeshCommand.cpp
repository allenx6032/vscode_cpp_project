/****************************************************************************
 Copyright (c) 2013-2014 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "cocos_CCMeshCommand.h"
#include "cocos_ccMacros.h"
#include "cocos_CCConfiguration.h"
#include "cocos_CCDirector.h"
#include "cocos_CCEventCustom.h"
#include "cocos_CCEventListenerCustom.h"
#include "cocos_CCEventDispatcher.h"
#include "cocos_CCEventType.h"
#include "cocos_CCConfiguration.h"
#include "cocos_CCLight.h"
#include "cocos_ccGLStateCache.h"
#include "cocos_CCGLProgramState.h"
#include "cocos_CCRenderer.h"
#include "cocos_CCTextureAtlas.h"
#include "cocos_CCTexture2D.h"
#include "cocos_ccGLStateCache.h"
#include "xxhash.h"

NS_CC_BEGIN

//render state
static bool   s_cullFaceEnabled = false;
static GLenum s_cullFace = 0;
static bool   s_depthTestEnabled = false;
static bool   s_depthWriteEnabled = false;

static const char          *s_dirLightUniformColorName = "u_DirLightSourceColor";
static std::vector<Vec3> s_dirLightUniformColorValues;
static const char          *s_dirLightUniformDirName = "u_DirLightSourceDirection";
static std::vector<Vec3> s_dirLightUniformDirValues;

static const char          *s_pointLightUniformColorName = "u_PointLightSourceColor";
static std::vector<Vec3> s_pointLightUniformColorValues;
static const char          *s_pointLightUniformPositionName = "u_PointLightSourcePosition";
static std::vector<Vec3> s_pointLightUniformPositionValues;
static const char          *s_pointLightUniformRangeInverseName = "u_PointLightSourceRangeInverse";
static std::vector<float> s_pointLightUniformRangeInverseValues;

static const char          *s_spotLightUniformColorName = "u_SpotLightSourceColor";
static std::vector<Vec3> s_spotLightUniformColorValues;
static const char          *s_spotLightUniformPositionName = "u_SpotLightSourcePosition";
static std::vector<Vec3> s_spotLightUniformPositionValues;
static const char          *s_spotLightUniformDirName = "u_SpotLightSourceDirection";
static std::vector<Vec3> s_spotLightUniformDirValues;
static const char          *s_spotLightUniformInnerAngleCosName = "u_SpotLightSourceInnerAngleCos";
static std::vector<float> s_spotLightUniformInnerAngleCosValues;
static const char          *s_spotLightUniformOuterAngleCosName = "u_SpotLightSourceOuterAngleCos";
static std::vector<float> s_spotLightUniformOuterAngleCosValues;
static const char          *s_spotLightUniformRangeInverseName = "u_SpotLightSourceRangeInverse";
static std::vector<float> s_spotLightUniformRangeInverseValues;

static const char          *s_ambientLightUniformColorName = "u_AmbientLightSourceColor";


MeshCommand::MeshCommand()
: _textureID(0)
, _glProgramState(nullptr)
, _blendType(BlendFunc::DISABLE)
, _displayColor(1.0f, 1.0f, 1.0f, 1.0f)
, _matrixPalette(nullptr)
, _matrixPaletteSize(0)
, _materialID(0)
, _vao(0)
, _cullFaceEnabled(false)
, _cullFace(GL_BACK)
, _depthTestEnabled(false)
, _depthWriteEnabled(false)
, _lightMask(-1)
{
    _type = RenderCommand::Type::MESH_COMMAND;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // listen the event that renderer was recreated on Android
    _rendererRecreatedListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, CC_CALLBACK_1(MeshCommand::listenRendererRecreated, this));
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_rendererRecreatedListener, -1);
#endif
}

void MeshCommand::init(float globalOrder,
                       GLuint textureID,
                       GLProgramState* glProgramState,
                       BlendFunc blendType,
                       GLuint vertexBuffer,
                       GLuint indexBuffer,
                       GLenum primitive,
                       GLenum indexFormat,
                       ssize_t indexCount,
                       const Mat4 &mv)
{
    CCASSERT(glProgramState, "GLProgramState cannot be nill");
    
    _globalOrder = globalOrder;
    _textureID = textureID;
    _blendType = blendType;
    _glProgramState = glProgramState;

    _vertexBuffer = vertexBuffer;
    _indexBuffer = indexBuffer;
    _primitive = primitive;
    _indexFormat = indexFormat;
    _indexCount = indexCount;
    _mv.set(mv);

}

void MeshCommand::setCullFaceEnabled(bool enable)
{
    _cullFaceEnabled = enable;
}

void MeshCommand::setCullFace(GLenum cullFace)
{
    _cullFace = cullFace;
}

void MeshCommand::setDepthTestEnabled(bool enable)
{
    _depthTestEnabled = enable;
}

void MeshCommand::setDepthWriteEnabled(bool enable)
{
    _depthWriteEnabled = enable;
}

void MeshCommand::setDisplayColor(const Vec4& color)
{
    _displayColor = color;
}

MeshCommand::~MeshCommand()
{
    releaseVAO();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    Director::getInstance()->getEventDispatcher()->removeEventListener(_rendererRecreatedListener);
#endif
}

void MeshCommand::applyRenderState()
{
    if (_cullFaceEnabled && !s_cullFaceEnabled)
    {
        glEnable(GL_CULL_FACE);
        s_cullFaceEnabled = true;
    }
    if (s_cullFace != _cullFace)
    {
        glCullFace(_cullFace);
        s_cullFace = _cullFace;
    }
    if (_depthTestEnabled && !s_depthTestEnabled)
    {
        glEnable(GL_DEPTH_TEST);
        s_depthTestEnabled = true;
    }
    if (_depthWriteEnabled && !s_depthWriteEnabled)
    {
        glDepthMask(GL_TRUE);
        s_depthWriteEnabled = true;
    }
}

void MeshCommand::restoreRenderState()
{
    if (s_cullFaceEnabled)
    {
        glDisable(GL_CULL_FACE);
        s_cullFaceEnabled = false;
    }
    if (s_depthTestEnabled)
    {
        glDisable(GL_DEPTH_TEST);
        s_depthTestEnabled = false;
    }
    if (s_depthWriteEnabled)
    {
        glDepthMask(GL_FALSE);
        s_depthWriteEnabled = false;
    }
    s_cullFace = 0;
}

void MeshCommand::genMaterialID(GLuint texID, void* glProgramState, GLuint vertexBuffer, GLuint indexBuffer, const BlendFunc& blend)
{
    int intArray[7] = {0};
    intArray[0] = (int)texID;
    *(int**)&intArray[1] = (int*) glProgramState;
    intArray[3] = (int) vertexBuffer;
    intArray[4] = (int) indexBuffer;
    intArray[5] = (int) blend.src;
    intArray[6] = (int) blend.dst;
    _materialID = XXH32((const void*)intArray, sizeof(intArray), 0);
}

void MeshCommand::MatrixPalleteCallBack( GLProgram* glProgram, Uniform* uniform)
{
    glUniform4fv( uniform->location, (GLsizei)_matrixPaletteSize, (const float*)_matrixPalette );
}

void MeshCommand::preBatchDraw()
{
    // Set material
    GL::bindTexture2D(_textureID);
    GL::blendFunc(_blendType.src, _blendType.dst);

    if (Configuration::getInstance()->supportsShareableVAO() && _vao == 0)
        buildVAO();
    if (_vao)
    {
        GL::bindVAO(_vao);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
        _glProgramState->applyAttributes();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    }
}
void MeshCommand::batchDraw()
{
    // set render state
    applyRenderState();
    
    _glProgramState->setUniformVec4("u_color", _displayColor);
    
    if (_matrixPaletteSize && _matrixPalette)
    {
        _glProgramState->setUniformCallback("u_matrixPalette", CC_CALLBACK_2(MeshCommand::MatrixPalleteCallBack, this));
        
    }
    
    _glProgramState->applyGLProgram(_mv);
    _glProgramState->applyUniforms();

    if (Director::getInstance()->getRunningScene()->getLights().size() > 0)
        setLightUniforms();
    
    // Draw
    glDrawElements(_primitive, (GLsizei)_indexCount, _indexFormat, 0);
    
    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, _indexCount);
}
void MeshCommand::postBatchDraw()
{
    //restore render state
    restoreRenderState();
    if (_vao)
    {
        GL::bindVAO(0);
    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void MeshCommand::execute()
{
    // set render state
    applyRenderState();
    // Set material
    GL::bindTexture2D(_textureID);
    GL::blendFunc(_blendType.src, _blendType.dst);

    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    _glProgramState->setUniformVec4("u_color", _displayColor);
    
    if (_matrixPaletteSize && _matrixPalette)
    {
        _glProgramState->setUniformCallback("u_matrixPalette", CC_CALLBACK_2(MeshCommand::MatrixPalleteCallBack, this));
        
    }
    
    _glProgramState->apply(_mv);   

    if (Director::getInstance()->getRunningScene()->getLights().size() > 0)
        setLightUniforms();
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    
    // Draw
    glDrawElements(_primitive, (GLsizei)_indexCount, _indexFormat, 0);
    
    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, _indexCount);
    
    //restore render state
    restoreRenderState();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshCommand::buildVAO()
{
    releaseVAO();
    glGenVertexArrays(1, &_vao);
    GL::bindVAO(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    auto flags = _glProgramState->getVertexAttribsFlags();
    for (int i = 0; flags > 0; i++) {
        int flag = 1 << i;
        if (flag & flags)
            glEnableVertexAttribArray(i);
        flags &= ~flag;
    }
    _glProgramState->applyAttributes(false);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    
    GL::bindVAO(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void MeshCommand::releaseVAO()
{
    if (_vao)
    {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
        GL::bindVAO(0);
    }
}


void MeshCommand::setLightUniforms()
{
    Director *director = Director::getInstance();
    auto scene = director->getRunningScene();
    const auto& conf = Configuration::getInstance();
    int maxDirLight = conf->getMaxSupportDirLightInShader();
    int maxPointLight = conf->getMaxSupportPointLightInShader();
    int maxSpotLight = conf->getMaxSupportSpotLightInShader();
    auto &lights = scene->getLights();
    auto glProgram = _glProgramState->getGLProgram();
    if (_glProgramState->getVertexAttribsFlags() & (1 << GLProgram::VERTEX_ATTRIB_NORMAL))
    {
        resetLightUniformValues();

        GLint enabledDirLightNum = 0;
        GLint enabledPointLightNum = 0;
        GLint enabledSpotLightNum = 0;
        Vec3 ambientColor;
        for (const auto& light : lights)
        {
            bool useLight = light->isEnabled() && ((unsigned int)light->getLightFlag() & _lightMask);
            if (useLight)
            {
                float intensity = light->getIntensity();
                switch (light->getLightType())
                {
                    case LightType::DIRECTIONAL:
                    {
                        if(enabledDirLightNum < maxDirLight)
                        {
                            auto dirLight = static_cast<DirectionLight *>(light);
                            Vec3 dir = dirLight->getDirectionInWorld();
                            dir.normalize();
                            const Color3B &col = dirLight->getDisplayedColor();
                            s_dirLightUniformColorValues[enabledDirLightNum] = Vec3(col.r / 255.0f * intensity, col.g / 255.0f * intensity, col.b / 255.0f * intensity);
                            s_dirLightUniformDirValues[enabledDirLightNum] = dir;
                            ++enabledDirLightNum;
                        }
                        
                    }
                        break;
                    case LightType::POINT:
                    {
                        if(enabledPointLightNum < maxPointLight)
                        {
                            auto pointLight = static_cast<PointLight *>(light);
                            Mat4 mat= pointLight->getNodeToWorldTransform();
                            const Color3B &col = pointLight->getDisplayedColor();
                            s_pointLightUniformColorValues[enabledPointLightNum] = Vec3(col.r / 255.0f * intensity, col.g / 255.0f * intensity, col.b / 255.0f * intensity);
                            s_pointLightUniformPositionValues[enabledPointLightNum] = Vec3(mat.m[12], mat.m[13], mat.m[14]);
                            s_pointLightUniformRangeInverseValues[enabledPointLightNum] = 1.0f / pointLight->getRange();
                            ++enabledPointLightNum;
                        }
                    }
                        break;
                    case LightType::SPOT:
                    {
                        if(enabledSpotLightNum < maxSpotLight)
                        {
                            auto spotLight = static_cast<SpotLight *>(light);
                            Vec3 dir = spotLight->getDirectionInWorld();
                            dir.normalize();
                            Mat4 mat= light->getNodeToWorldTransform();
                            const Color3B &col = spotLight->getDisplayedColor();
                            s_spotLightUniformColorValues[enabledSpotLightNum] = Vec3(col.r / 255.0f * intensity, col.g / 255.0f * intensity, col.b / 255.0f * intensity);
                            s_spotLightUniformPositionValues[enabledSpotLightNum] = Vec3(mat.m[12], mat.m[13], mat.m[14]);
                            s_spotLightUniformDirValues[enabledSpotLightNum] = dir;
                            s_spotLightUniformInnerAngleCosValues[enabledSpotLightNum] = spotLight->getCosInnerAngle();
                            s_spotLightUniformOuterAngleCosValues[enabledSpotLightNum] = spotLight->getCosOuterAngle();
                            s_spotLightUniformRangeInverseValues[enabledSpotLightNum] = 1.0f / spotLight->getRange();
                            ++enabledSpotLightNum;
                        }
                    }
                        break;
                    case LightType::AMBIENT:
                    {
                        auto ambLight = static_cast<AmbientLight *>(light);
                        const Color3B &col = ambLight->getDisplayedColor();
                        ambientColor += Vec3(col.r / 255.0f * intensity, col.g / 255.0f * intensity, col.b / 255.0f * intensity);
                    }
                        break;
                    default:
                        break;
                }
            }
        }
        
        if (0 < maxDirLight)
        {
            glProgram->setUniformLocationWith3fv((GLint)glProgram->getUniformLocationForName(s_dirLightUniformColorName), (GLfloat*)(&s_dirLightUniformColorValues[0]), (unsigned int)s_dirLightUniformColorValues.size());
            glProgram->setUniformLocationWith3fv((GLint)glProgram->getUniformLocationForName(s_dirLightUniformDirName), (GLfloat*)(&s_dirLightUniformDirValues[0]), (unsigned int)s_dirLightUniformDirValues.size());
        }

        if (0 < maxPointLight)
        {
            glProgram->setUniformLocationWith3fv((GLint)glProgram->getUniformLocationForName(s_pointLightUniformColorName), (GLfloat*)(&s_pointLightUniformColorValues[0]), (unsigned int)s_pointLightUniformColorValues.size());
            glProgram->setUniformLocationWith3fv((GLint)glProgram->getUniformLocationForName(s_pointLightUniformPositionName), (GLfloat*)(&s_pointLightUniformPositionValues[0]), (unsigned int)s_pointLightUniformPositionValues.size());
            glProgram->setUniformLocationWith1fv((GLint)glProgram->getUniformLocationForName(s_pointLightUniformRangeInverseName), (GLfloat*)(&s_pointLightUniformRangeInverseValues[0]), (unsigned int)s_pointLightUniformRangeInverseValues.size());
        }

        if (0 < maxSpotLight)
        {
            glProgram->setUniformLocationWith3fv((GLint)glProgram->getUniformLocationForName(s_spotLightUniformColorName), (GLfloat*)(&s_spotLightUniformColorValues[0]), (unsigned int)s_spotLightUniformColorValues.size());
            glProgram->setUniformLocationWith3fv((GLint)glProgram->getUniformLocationForName(s_spotLightUniformPositionName), (GLfloat*)(&s_spotLightUniformPositionValues[0]), (unsigned int)s_spotLightUniformPositionValues.size());
            glProgram->setUniformLocationWith3fv((GLint)glProgram->getUniformLocationForName(s_spotLightUniformDirName), (GLfloat*)(&s_spotLightUniformDirValues[0]), (unsigned int)s_spotLightUniformDirValues.size());
            glProgram->setUniformLocationWith1fv((GLint)glProgram->getUniformLocationForName(s_spotLightUniformInnerAngleCosName), (GLfloat*)(&s_spotLightUniformInnerAngleCosValues[0]), (unsigned int)s_spotLightUniformInnerAngleCosValues.size());
            glProgram->setUniformLocationWith1fv((GLint)glProgram->getUniformLocationForName(s_spotLightUniformOuterAngleCosName), (GLfloat*)(&s_spotLightUniformOuterAngleCosValues[0]), (unsigned int)s_spotLightUniformOuterAngleCosValues.size());
            glProgram->setUniformLocationWith1fv((GLint)glProgram->getUniformLocationForName(s_spotLightUniformRangeInverseName), (GLfloat*)(&s_spotLightUniformRangeInverseValues[0]), (unsigned int)s_spotLightUniformRangeInverseValues.size());
        }

        glProgram->setUniformLocationWith3f(glProgram->getUniformLocationForName(s_ambientLightUniformColorName), ambientColor.x, ambientColor.y, ambientColor.z);
    }
    else // normal does not exist
    {
        Vec3 ambient(0.0f, 0.0f, 0.0f);
        bool hasAmbient = false;
        for (const auto& light : lights)
        {
            if (light->getLightType() == LightType::AMBIENT)
            {
                bool useLight = light->isEnabled() && ((unsigned int)light->getLightFlag() & _lightMask);
                if (useLight)
                {
                    hasAmbient = true;
                    const Color3B &col = light->getDisplayedColor();
                    ambient.x += col.r * light->getIntensity();
                    ambient.y += col.g * light->getIntensity();
                    ambient.z += col.b * light->getIntensity();
                }
            }
        }
        if (hasAmbient)
        {
            ambient.x /= 255.f; ambient.y /= 255.f; ambient.z /= 255.f;
        }
        glProgram->setUniformLocationWith4f(glProgram->getUniformLocationForName("u_color"), _displayColor.x * ambient.x, _displayColor.y * ambient.y, _displayColor.z * ambient.z, _displayColor.w);
    }
}

void MeshCommand::resetLightUniformValues()
{
    const auto& conf = Configuration::getInstance();
    int maxDirLight = conf->getMaxSupportDirLightInShader();
    int maxPointLight = conf->getMaxSupportPointLightInShader();
    int maxSpotLight = conf->getMaxSupportSpotLightInShader();

    s_dirLightUniformColorValues.assign(maxDirLight, Vec3::ZERO);
    s_dirLightUniformDirValues.assign(maxDirLight, Vec3::ZERO);

    s_pointLightUniformColorValues.assign(maxPointLight, Vec3::ZERO);
    s_pointLightUniformPositionValues.assign(maxPointLight, Vec3::ZERO);
    s_pointLightUniformRangeInverseValues.assign(maxPointLight, 0.0f);

    s_spotLightUniformColorValues.assign(maxSpotLight, Vec3::ZERO);
    s_spotLightUniformPositionValues.assign(maxSpotLight, Vec3::ZERO);
    s_spotLightUniformDirValues.assign(maxSpotLight, Vec3::ZERO);
    s_spotLightUniformInnerAngleCosValues.assign(maxSpotLight, 0.0f);
    s_spotLightUniformOuterAngleCosValues.assign(maxSpotLight, 0.0f);
    s_spotLightUniformRangeInverseValues.assign(maxSpotLight, 0.0f);
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void MeshCommand::listenRendererRecreated(EventCustom* event)
{
    _vao = 0;
}

#endif

NS_CC_END
