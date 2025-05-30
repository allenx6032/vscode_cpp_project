#include "e2d_asset_mgr.h"
#include "e2d_filesystem.h"

using namespace Easy2D;

AssetManager::AssetManager()
{
}

SPtr<Stream> AssetManager::loadAsset(const Path& path)
{
#ifdef WIN32
    Path fullPath = mBasePath / path;
    auto stream = std::make_shared<FileStream>(fullPath);
    return std::dynamic_pointer_cast<Stream>(stream);
#endif
    return nullptr;
}

bool AssetManager::loadAssetData(const Path& path, Bytes& data)
{
    auto stream = loadAsset(path);
    if (nullptr == stream)
    {
        LOG_ERROR << ("AssetManager::loadAssetData loadAsset failed!");
        return false;
    }
    if (stream->read(data) <= 0)
    {
        LOG_ERROR << ("AssetManager::loadAssetData read data failed!");
        return false;
    }
    return true;
}

Vector<Path> AssetManager::enumerateDirectory(const Path& path, bool full)
{
    Path fullPath = mBasePath / path;
    return FileSystem::enumerateDirectory(fullPath, full);
}

SPtr<Stream> AssetManager::asyncAsset(const Path& path)
{
    return nullptr;
}
