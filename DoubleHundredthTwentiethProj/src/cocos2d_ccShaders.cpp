/****************************************************************************
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2012 		cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "cocos2d_ccShaders.h"

#define STRINGIFY(A)  #A

NS_CC_BEGIN
//

#include "shaders/ccShader_Position_uColor.frag"

#include "shaders/ccShader_Position_uColor.vert"

//
#include "shaders/ccShader_PositionColor.frag"
#include "shaders/ccShader_PositionColor.vert"

//
#include "shaders/ccShader_PositionColorTextureAsPointsize.vert"

//
#include "shaders/ccShader_PositionTexture.frag"
#include "shaders/ccShader_PositionTexture.vert"

//
#include "shaders/ccShader_PositionTextureA8Color.frag"
#include "shaders/ccShader_PositionTextureA8Color.vert"

//
#include "shaders/ccShader_PositionTextureColor.frag"
#include "shaders/ccShader_PositionTextureColor.vert"

//
#include "shaders/ccShader_PositionTextureColor_noMVP.frag"
#include "shaders/ccShader_PositionTextureColor_noMVP.vert"

//
#include "shaders/ccShader_PositionTextureColorAlphaTest.frag"

//
#include "shaders/ccShader_PositionTexture_uColor.frag"
#include "shaders/ccShader_PositionTexture_uColor.vert"

#include "shaders/ccShader_PositionColorLengthTexture.frag"
#include "shaders/ccShader_PositionColorLengthTexture.vert"

#include "shaders/ccShader_UI_Gray.frag"
//
#include "shaders/ccShader_Label.vert"
#include "shaders/ccShader_Label_df.frag"
#include "shaders/ccShader_Label_df_glow.frag"
#include "shaders/ccShader_Label_normal.frag"
#include "shaders/ccShader_Label_outline.frag"

//
#include "shaders/ccShader_CameraClear.vert"
#include "shaders/ccShader_CameraClear.frag"

// ETC1 ALPHA support
#include "shaders/ccShader_ETC1AS_PositionTextureColor.frag"
#include "shaders/ccShader_ETC1AS_PositionTextureGray.frag"

#include "shaders/ccShader_Position.vert"
#include "shaders/ccShader_LayerRadialGradient.frag"

NS_CC_END
