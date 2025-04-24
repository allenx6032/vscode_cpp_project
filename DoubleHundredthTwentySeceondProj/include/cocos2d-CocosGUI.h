

#include "cocos2d-CCConfig.h"
#if CC_USE_UI > 0

/****************************************************************************
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

#ifndef __COCOSGUI_H__
#define __COCOSGUI_H__


#include "cocos2d-UIWidget.h"
#include "cocos2d-UILayout.h"
#include "cocos2d-UIButton.h"
#include "cocos2d-UICheckBox.h"
#include "cocos2d-UIRadioButton.h"
#include "cocos2d-UIImageView.h"
#include "cocos2d-UIText.h"
#include "cocos2d-UITextAtlas.h"
#include "cocos2d-UILoadingBar.h"
#include "cocos2d-UIScrollView.h"
#include "cocos2d-UIListView.h"
#include "cocos2d-UISlider.h"
#include "cocos2d-UITextField.h"
#include "cocos2d-UITextBMFont.h"
#include "cocos2d-UIPageView.h"
#include "cocos2d-UIHelper.h"
#include "cocos2d-UIRichText.h"
#include "cocos2d-UIHBox.h"
#include "cocos2d-UIVBox.h"
#include "cocos2d-UIRelativeBox.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN)
#include "cocos2d-UIVideoPlayer.h"
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN)
#include "cocos2d-UIWebView.h"
#endif
#include "cocos2d-UIDeprecated.h"
#include "cocos2d-GUIExport.h"
#include "cocos2d-UIScale9Sprite.h"
#include "cocos2d-UIEditBox.h"
#include "cocos2d-UILayoutComponent.h"
#include "cocos2d-UITabControl.h"
#include "cocos2d-CCResourceData.h"

/**
 * @addtogroup ui
 * @{
 */
NS_CC_BEGIN
namespace ui {


/**
 * Get current cocos GUI module version string.
 *@return A string representation of GUI module version number
 */
CC_GUI_DLL const char* CocosGUIVersion();

}

NS_CC_END
// end of ui group
/// @}

#endif /* defined(__CocosGUITest__Cocos__) */


#endif // CC_USE_UI

