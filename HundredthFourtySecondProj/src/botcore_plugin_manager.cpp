#include "botcore_plugin_manager.hxx"
#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <sys/stat.h>
#include <botcore_logprint.hxx>

logprint PluginLogs("PLUGIN MANAGER", "./logs/plugin_manager.log");

bool fileExists(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

PluginManager::PluginManager() : handle(nullptr) {}

PluginManager::~PluginManager()
{
    if (handle)
    {
        unloadPlugin();
    }
}

bool PluginManager::loadPlugin(const std::string &pluginPath)
{
    if (!fileExists(pluginPath))
    {
        PluginLogs.LOGE("Plugin file not found: %s", pluginPath.c_str());
        return false;
    }

#if defined(_WIN32)
    handle = LoadLibrary(pluginPath.c_str());
    if (!handle)
    {
        char errorBuffer[256];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL, GetLastError(), 0, errorBuffer, sizeof(errorBuffer), NULL);
        PluginLogs.LOGE("Failed to load plugin: %s", errorBuffer);
        return false;
    }
#else
    dlerror();

    handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
    if (!handle)
    {
        const char *error = dlerror();
        PluginLogs.LOGE("Failed to load plugin: %s", error ? error : "unknown error");
        return false;
    }
#endif
    return true;
}

void PluginManager::unloadPlugin()
{
    if (handle)
    {
#if defined(_WIN32)
        FreeLibrary(static_cast<HMODULE>(handle));
#else
        dlclose(handle);
#endif
        handle = nullptr;
    }
}

void PluginManager::executePluginFunction(const std::string &functionName)
{
    typedef void (*PluginFunction)();
#if defined(_WIN32)
    PluginFunction func = reinterpret_cast<PluginFunction>(GetProcAddress(static_cast<HMODULE>(handle), functionName.c_str()));
    if (!func)
    {
        char errorBuffer[256];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL, GetLastError(), 0, errorBuffer, sizeof(errorBuffer), NULL);
        PluginLogs.LOGE("Failed to load function: %s", errorBuffer);
        return;
    }
#else
    PluginFunction func = reinterpret_cast<PluginFunction>(dlsym(handle, functionName.c_str()));
    const char *dlsym_error = dlerror();
    if (dlsym_error)
    {
        PluginLogs.LOGE("Failed to load function: %s", dlsym_error);
        return;
    }
#endif
    func();
}
