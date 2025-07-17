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

#ifndef __COCOSGUI_H__
#define __COCOSGUI_H__


#include "cocos_UIWidget.h"
#include "cocos_UILayout.h"
#include "cocos_UIButton.h"
#include "cocos_UICheckBox.h"
#include "cocos_UIImageView.h"
#include "cocos_UIText.h"
#include "cocos_UITextAtlas.h"
#include "cocos_UILoadingBar.h"
#include "cocos_UIScrollView.h"
#include "cocos_UIListView.h"
#include "cocos_UISlider.h"
#include "cocos_UITextField.h"
#include "cocos_UITextBMFont.h"
#include "cocos_UIPageView.h"
#include "cocos_UIHelper.h"
#include "cocos_UIRichText.h"
#include "cocos_UIHBox.h"
#include "cocos_UIVBox.h"
#include "cocos_UIRelativeBox.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "cocos_UIVideoPlayer.h"
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "cocos_UIWebView.h"
#endif
#include "cocos_GUIExport.h"
#include "cocos_UIScale9Sprite.h"
#include "cocos_UIEditBox.h"
#include "cocos_UILayoutComponent.h"

NS_CC_BEGIN
namespace ui {

CC_GUI_DLL const char* CocosGUIVersion();

}
NS_CC_END

#endif /* defined(__CocosGUITest__Cocos__) */
