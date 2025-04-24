#include "demo_CCDeviceEx.hpp"

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

const char* jailbreak_tool_pathes[] = {
    "/Applications/Cydia.app",
    "/Library/MobileSubstrate/MobileSubstrate.dylib",
    "/bin/bash",
    "/usr/sbin/sshd",
    "/etc/apt"
};
static BOOL detectJailBreakByJailBreakFileExisted() {
    for (int i = 0; i< ARRAY_SIZE(jailbreak_tool_pathes); i++) {
        NSString *path = [NSString stringWithUTF8String:jailbreak_tool_pathes[i]];
        if ([[NSFileManager defaultManager] fileExistsAtPath:path]) {
            return YES;
        }
    }
    return NO;
}

static BOOL detectJailBreakByCydiaPathExisted() {
    if ([[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:@"cydia://"]]) {
        return YES;
    }
    return NO;
}
#define USER_APP_PATH                 @"/User/Applications/"
static BOOL detectJailBreakByAppPathExisted() {
    if ([[NSFileManager defaultManager] fileExistsAtPath:USER_APP_PATH]) {
        NSArray *applist = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:USER_APP_PATH error:nil];
        NSLog(@"applist = %@", applist);
        return YES;
    }
    return NO;
}
static char* printEnv(void) {
    char *env = getenv("DYLD_INSERT_LIBRARIES");
    return env;
}
static BOOL detectJailBreakByEnvironmentExisted() {
    if (printEnv()) {
        return YES;
    }
    return NO;
}
#endif

NS_CC_BEGIN

bool DeviceEx::isJailbroken()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    BOOL result =  NO;
    
    result = detectJailBreakByJailBreakFileExisted();
    
    if (!result) {
        result = detectJailBreakByAppPathExisted();
    }
    
    if (!result) {
        result = detectJailBreakByEnvironmentExisted();
    }
    
    if (!result) {
        result = detectJailBreakByCydiaPathExisted();
    }
    
    return result;
    
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#else
    return false;
#endif
}

NS_CC_END
