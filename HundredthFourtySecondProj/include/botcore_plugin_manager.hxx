#ifndef _PLUGIN_MANAGER_HXX
#define _PLUGIN_MANAGER_HXX

#include <string>

class PluginManager
{
public:
    PluginManager();
    ~PluginManager();

    bool loadPlugin(const std::string &pluginPath);
    void unloadPlugin();
    void executePluginFunction(const std::string &functionName);

private:
    void *handle;
};

#endif
