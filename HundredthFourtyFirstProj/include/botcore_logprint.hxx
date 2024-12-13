#ifndef _LOGPRINT_HXX
#define _LOGPRINT_HXX

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#if defined(BP_LIB_EXPORT)
#undef BP_LIB_EXPORT
#endif
#if defined(_WIN32) || defined(_WIN64)
#define BP_LIB_EXPORT __declspec(dllexport)
#elif defined(__linux__) || defined(__APPLE__)
#define BP_LIB_EXPORT __attribute__((visibility("default")))
#else
#define BP_LIB_EXPORT
#endif

class BP_LIB_EXPORT logprint
{
public:
    logprint(const char *tagCall, const char *logP) : logPath(logP), tag(tagCall)
    {
        printf("Logger loaded with path %s and tag %s\n", logP, tagCall);
    }
    ~logprint()
    {
        printf("Logger unloaded, path: %s, tag: %s\n", logPath, tag);
    }
    void LOGE(const char *message, ...);
    void LOGI(const char *message, ...);

private:
    const char *getTimestamp();
    char *generateFormattedString(const char *format, va_list args);
    void writeLog(const char *message);
    const char *logPath;
    const char *tag;
};

#endif