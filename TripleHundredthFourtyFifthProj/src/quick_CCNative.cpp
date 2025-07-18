
#include "quick_CCNative.h"
#include "quick_CCNativeWin32.h"

NS_CC_EXTRA_BEGIN

void Native::showActivityIndicator(void)
{
	NativeWin32::sharedInstance()->showActivityIndicator();
}

void Native::hideActivityIndicator(void)
{
	NativeWin32::sharedInstance()->hideActivityIndicator();
}


void Native::createAlert(const char* title,
                           const char* message,
                           const char* cancelButtonTitle)
{
	NativeWin32::sharedInstance()->createAlertView(title, message, cancelButtonTitle);
}

int Native::addAlertButton(const char* buttonTitle)
{
	return NativeWin32::sharedInstance()->addAlertButton(buttonTitle);
}

#if CC_LUA_ENGINE_ENABLED > 0
int Native::addAlertButtonLua(const char* buttonTitle)
{
    return addAlertButton(buttonTitle) + 1;
}
#endif

void Native::showAlert(AlertViewDelegate* delegate)
{
	NativeWin32::sharedInstance()->showAlertViewWithDelegate(delegate);
}

#if CC_LUA_ENGINE_ENABLED > 0
void Native::showAlertLua(cocos2d::LUA_FUNCTION listener)
{
	NativeWin32::sharedInstance()->showAlertViewWithLuaListener(listener);
}
#endif

void Native::cancelAlert(void)
{
	NativeWin32::sharedInstance()->cancelAlertView();
}

const std::string Native::getOpenUDID(void)
{
    return NativeWin32::sharedInstance()->getUDID();
}

void Native::openURL(const char* url)
{
	if (!url) {
		return;
	}
	int urlLen = MultiByteToWideChar(CP_UTF8, 0, url, -1, NULL, 0) + 1;
	WCHAR *wUrl = new WCHAR[urlLen];
	memset(wUrl, 0, sizeof(WCHAR)*urlLen);
	MultiByteToWideChar(CP_UTF8, 0, url, -1, wUrl, urlLen);
	ShellExecute(0, L"open", wUrl, NULL, 0, 0);
	delete wUrl;
}

const std::string Native::getInputText(const char* title, const char* message, const char* defaultValue)
{
	return NativeWin32::sharedInstance()->getInputText(title, message, defaultValue);
}

const string Native::getDeviceName(void)
{
    return "Win32";
}

void Native::vibrate()
{
}


NS_CC_EXTRA_END
