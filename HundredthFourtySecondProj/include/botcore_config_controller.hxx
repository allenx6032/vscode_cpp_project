#ifndef _CONFIG_CONTROLLER_HXX
#define _CONFIG_CONTROLLER_HXX

#include <string>
#include <unordered_map>

class ConfigManager
{
public:
    enum class AccessType
    {
        RO, // read-only
        RW  // read-write
    };

    static ConfigManager &getInstance();

    template <typename T>
    void setConfig(const std::string &key, const T &value, AccessType access);

    std::string getConfig(const std::string &key) const;

    ConfigManager(const ConfigManager &) = delete;
    ConfigManager &operator=(const ConfigManager &) = delete;

private:
    ConfigManager() = default;

    struct ConfigEntry
    {
        std::string value;
        AccessType access;
    };

    std::unordered_map<std::string, ConfigEntry> configData;

    template <typename T>
    std::string toString(const T &value) const;
};

#endif