/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2012-2018 DragonBones team and other contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef DRAGONBONES_HEADERS_H
#define DRAGONBONES_HEADERS_H

// core
#include "cocos_DragonBones.h"
#include "cocos_dragonBonesBaseObject.h"

// geom
#include "cocos_dragonBonesMatrix.h"
#include "cocos_dragonBonesTransform.h"
#include "cocos_dragonBonesColorTransform.h"
#include "cocos_dragonBonesPoint.h"
#include "cocos_dragonBonesRectangle.h"

// model
#include "cocos_dragonBonesTextureAtlasData.h"
#include "cocos_dragonBonesUserData.h"
#include "cocos_DragonBonesData.h"
#include "cocos_dragonBonesArmatureData.h"
#include "cocos_dragonBonesConstraintData.h"
#include "cocos_dragonBonesCanvasData.h"
#include "cocos_dragonBonesSkinData.h"
#include "cocos_dragonBonesDisplayData.h"
#include "cocos_dragonBonesBoundingBoxData.h"
#include "cocos_dragonBonesAnimationData.h"
#include "cocos_dragonBonesAnimationConfig.h"

// armature
#include "cocos_dragonBonesIArmatureProxy.h"
#include "cocos_dragonBonesArmature.h"
#include "cocos_dragonBonesTransformObject.h"
#include "cocos_dragonBonesBone.h"
#include "cocos_dragonBonesSlot.h"
#include "cocos_dragonBonesConstraint.h"
#include "cocos_dragonBonesDeformVertices.h"

// animation
#include "cocos_dragonBonesIAnimatable.h"
#include "cocos_dragonBonesWorldClock.h"
#include "cocos_dragonBonesAnimation.h"
#include "cocos_dragonBonesAnimationState.h"
#include "cocos_dragonBonesBaseTimelineState.h"
#include "cocos_dragonBonesTimelineState.h"

// event
#include "cocos_dragonBonesEventObject.h"
#include "cocos_dragonBonesIEventDispatcher.h"

#ifndef EGRET_WASM

// parser
#include "cocos_dragonBonesDataParser.h"
#include "cocos_dragonBonesJSONDataParser.h"
#include "cocos_dragonBonesBinaryDataParser.h"

// factory
#include "cocos_dragonBonesBaseFactory.h"
#endif // EGRET_WASM

#endif // DRAGONBONES_HEADERS_H
