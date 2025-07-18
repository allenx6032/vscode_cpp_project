
#ifndef __CC_EXTENSION_CCNATIVE_H_
#define __CC_EXTENSION_CCNATIVE_H_

#include "quick_cocos2dx_extra.h"
#include "quick_CCAlertViewDelegate.h"

#if CC_LUA_ENGINE_ENABLED > 0
#include "lua_cocos2dx_CCLuaEngine.h"
#endif

NS_CC_EXTRA_BEGIN

class Native
{
public:
    
    /** @brief Show activity indicator */
    static void showActivityIndicator(void);
    
    /** @brief Hide activity indicator */
    static void hideActivityIndicator(void);
    
    /** @brief Create alert view */
    static void createAlert(const char* title,
                            const char* message,
                            const char* cancelButtonTitle);
    
    /** @brief Add button to alert view, return button index */
    static int addAlertButton(const char* buttonTitle);

    /** @brief Show alert view */
    static void showAlert(AlertViewDelegate* delegate = NULL);
    /** @brief Hide and remove alert view */
    static void cancelAlert(void);
    
    /** @brief Get OpenUDID value */
    static const std::string getOpenUDID(void);
    
    /** @brief Open a web page in the browser; create an email; or call a phone number. */
    static void openURL(const char* url);
    
	/** @brief Show alert view, and get user input */
    static const std::string getInputText(const char* title, const char* message, const char* defaultValue);
    
    static const std::string getDeviceName(void);
    static void vibrate();
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    static void showAlertObjc(void *delegate);
#endif
    
#if CC_LUA_ENGINE_ENABLED > 0
    static int addAlertButtonLua(const char* buttonTitle);
    static void showAlertLua(LUA_FUNCTION listener);
#endif

private:
    Native(void) {}
};

NS_CC_EXTRA_END

#endif // __CC_EXTENSION_CCNATIVE_H_
