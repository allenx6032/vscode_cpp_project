#include "e2d_scene_mgr.h"

namespace Easy2D
{
    template <typename T>
    SPtr<T> SceneManager::createScene(const String& name)
    {
        auto pScene = std::make_shared<T>(name);
        if (!pScene->setup())
        {
            LOG_WARN << ("SceneManager::createScene: setup '") << pScene->getName() << ("' failed!");
            return nullptr;
        }
        if (!addScene(pScene))
        {
            LOG_WARN << ("SceneManager::createScene: addScene '") << pScene->getName() << ("' failed!");
            return nullptr;
        }
        return pScene;
    }

    template <typename T>
    SPtr<T> SceneManager::getScene(const uint64 guid)
    {
        auto it = mScenes.find(guid);
        if (it != mScenes.end())
        {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        return nullptr;
    }
}
