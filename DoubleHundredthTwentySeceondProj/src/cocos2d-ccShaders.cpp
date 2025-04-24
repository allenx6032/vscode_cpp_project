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

#include "cocos2d-CCShaders.h"

#define STRINGIFY(A)  #A

NS_CC_BEGIN
//

#include "cocos2d-CCShader_Position_uColor.frag"

#include "cocos2d-CCShader_Position_uColor.vert"

//
#include "cocos2d-CCShader_PositionColor.frag"
#include "cocos2d-CCShader_PositionColor.vert"

//
#include "cocos2d-CCShader_PositionColorTextureAsPointsize.vert"

//
#include "cocos2d-CCShader_PositionTexture.frag"
#include "cocos2d-CCShader_PositionTexture.vert"

//
#include "cocos2d-CCShader_PositionTextureA8Color.frag"
#include "cocos2d-CCShader_PositionTextureA8Color.vert"

//
#include "cocos2d-CCShader_PositionTextureColor.frag"
#include "cocos2d-CCShader_PositionTextureColor.vert"

//
#include "cocos2d-CCShader_PositionTextureColor_noMVP.frag"
#include "cocos2d-CCShader_PositionTextureColor_noMVP.vert"

//
#include "cocos2d-CCShader_PositionTextureColorAlphaTest.frag"

//
#include "cocos2d-CCShader_PositionTexture_uColor.frag"
#include "cocos2d-CCShader_PositionTexture_uColor.vert"

#include "cocos2d-CCShader_PositionColorLengthTexture.frag"
#include "cocos2d-CCShader_PositionColorLengthTexture.vert"

#include "cocos2d-CCShader_UI_Gray.frag"
//
#include "cocos2d-CCShader_Label.vert"
#include "cocos2d-CCShader_Label_df.frag"
#include "cocos2d-CCShader_Label_df_glow.frag"
#include "cocos2d-CCShader_Label_normal.frag"
#include "cocos2d-CCShader_Label_outline.frag"

//
#include "cocos2d-CCShader_CameraClear.vert"
#include "cocos2d-CCShader_CameraClear.frag"

// ETC1 ALPHA support
#include "cocos2d-CCShader_ETC1AS_PositionTextureColor.frag"
#include "cocos2d-CCShader_ETC1AS_PositionTextureGray.frag"

#include "cocos2d-CCShader_Position.vert"
#include "cocos2d-CCShader_LayerRadialGradient.frag"

NS_CC_END
