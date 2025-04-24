/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

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

#include "cocos2d-CCConfig.h"
#if CC_USE_CCS > 0

#ifndef __EDITOR_SUPPORT_COCOSTUDIO_H__
#define __EDITOR_SUPPORT_COCOSTUDIO_H__

#include "cocos2d-CCActionFrame.h"
#include "cocos2d-CCActionFrameEasing.h"
#include "cocos2d-CCActionManagerEx.h"
#include "cocos2d-CCActionNode.h"
#include "cocos2d-CCActionObject.h"
#include "cocos2d-CCArmature.h"
#include "cocos2d-CCBone.h"
#include "cocos2d-CCArmatureAnimation.h"
#include "cocos2d-CCProcessBase.h"
#include "cocos2d-CCTween.h"
#include "cocos2d-CCDatas.h"
#include "cocos2d-CCBatchNode.h"
#include "cocos2d-CCDecorativeDisplay.h"
#include "cocos2d-CCDisplayFactory.h"
#include "cocos2d-CCDisplayManager.h"
#include "cocos2d-CCSkin.h"
#include "cocos2d-CCColliderDetector.h"
#include "cocos2d-CCArmatureDataManager.h"
#include "cocos2d-CCArmatureDefine.h"
#include "cocos2d-CCDataReaderHelper.h"
#include "cocos2d-CCTransformHelp.h"
#include "cocos2d-CCUtilMath.h"
#include "cocos2d-CCComBase.h"
#include "cocos2d-CCComAttribute.h"
#include "cocos2d-CCComAudio.h"
#include "cocos2d-CCComController.h"
#include "cocos2d-CCComRender.h"
#include "cocos2d-CCInputDelegate.h"
#include "cocos2d-DictionaryHelper.h"
#include "cocos2d-CCSGUIReader.h"
#include "cocos2d-CCSSceneReader.h"
#include "cocos2d-TriggerBase.h"
#include "cocos2d-CCActionTimelineCache.h"
#include "cocos2d-CCFrame.h"
#include "cocos2d-CCTimeLine.h"
#include "cocos2d-CCActionTimeline.h"
#include "cocos2d-CCActionTimelineNode.h"
#include "cocos2d-CCBoneNode.h"
#include "cocos2d-CCSkeletonNode.h"
#include "cocos2d-CSLoader.h"
#include "cocos2d-CocosStudioExport.h"

namespace cocostudio
{
    void CC_STUDIO_DLL destroyCocosStudio();
}

#endif


#endif // CC_USE_CCS

