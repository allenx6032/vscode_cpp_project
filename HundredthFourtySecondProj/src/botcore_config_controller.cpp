#include "botcore_config_controller.hxx"
#include <sstream>

ConfigManager &ConfigManager::getInstance()
{
    static ConfigManager instance;
    return instance;
}

template <typename T>
void ConfigManager::setConfig(const std::string &key, const T &value, AccessType access)
{
    if (configData.find(key) != configData.end() && configData[key].access == AccessType::RO)
    {
        throw std::runtime_error("Attempt to write to read-only parameter: " + key);
    }

    configData[key] = {toString(value), access};
}

std::string ConfigManager::getConfig(const std::string &key) const
{
    auto it = configData.find(key);
    if (it != configData.end())
    {
        return it->second.value;
    }
    throw std::runtime_error("Parameter not found: " + key);
}

template <typename T>
std::string ConfigManager::toString(const T &value) const
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template <>
std::string ConfigManager::toString(const bool &value) const
{
    return value ? "true" : "false";
}

template void ConfigManager::setConfig<bool>(const std::string &key, const bool &value, AccessType access);
template void ConfigManager::setConfig<int>(const std::string &key, const int &value, AccessType access);
template void ConfigManager::setConfig<std::string>(const std::string &key, const std::string &value, AccessType access);
