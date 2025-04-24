
#include "demo_CryptFileUtils.hpp"

static inline unsigned char* toBuffer(cocos2d::Data&& d, ssize_t* size)
{
    unsigned char* buf = d.getBytes();
    *size = d.getSize();
    d.fastSet(nullptr, 0);
    return buf;
}

static inline std::string toString(const cocos2d::Data& d)
{
    return std::string(reinterpret_cast<const char*>(d.getBytes()), static_cast<size_t>(d.getSize()));
}


CryptFileUtils::CryptFileUtils(Crypt* crypt)
: FlatformFileUtils()
, crypt_(crypt)
{
    std::vector<std::string> paths(FileUtils::getInstance()->getSearchPaths());
    init();
    _searchPathArray = paths;
}

cocos2d::Data CryptFileUtils::decrypt(const cocos2d::Data& data)
{
    if (!crypt_->isEncrypted(data))
        return cocos2d::Data(data);
    
    cocos2d::Data rv;
    crypt_->decrypt(data, &rv);
    return rv;
}

std::string CryptFileUtils::getStringFromFile(const std::string& filename)
{
    return toString(decrypt(FlatformFileUtils::getDataFromFile(filename)));
}


cocos2d::Data CryptFileUtils::getDataFromFile(const std::string& filename)
{
    return decrypt(FlatformFileUtils::getDataFromFile(filename));
}


unsigned char* CryptFileUtils::getFileData(const std::string& filename, const char* mode, ssize_t *size)
{
    return toBuffer(decrypt(FlatformFileUtils::getDataFromFile(filename)), size);
}

cocos2d::Data CryptFileUtils::getDataFromZip(const std::string& zipFilePath, const std::string& filename)
{
    ssize_t sz= 0;
    auto ptr = FlatformFileUtils::getFileDataFromZip(zipFilePath, filename, &sz);
    
    cocos2d::Data data;
    data.fastSet(ptr, sz);
    return data;
}

unsigned char* CryptFileUtils::getFileDataFromZip(const std::string& zipFilePath, const std::string& filename, ssize_t *size)
{
    return toBuffer(decrypt(getDataFromZip(zipFilePath, filename)), size);
}


cocos2d::Data CryptFileUtils::encrypt(const cocos2d::Data& data)
{
    cocos2d::Data d;
    crypt_->encrypt(data, &d);
    return d; // RVO or move construct
}

bool CryptFileUtils::writeStringToFile(const std::string& dataStr, const std::string& fullPath)
{
    cocos2d::Data d;
    d.fastSet((unsigned char*)dataStr.data(), dataStr.size());
    
    auto rv = writeDataToFile(d, fullPath);
    
    d.fastSet(nullptr, 0);
    return rv;
}

bool CryptFileUtils::writeDataToFile(const cocos2d::Data& data, const std::string& fullPath)
{
    auto coded = encrypt(data);
    return FlatformFileUtils::writeDataToFile(coded, fullPath);
}

void CryptFileUtils::purgeCachedEntries()
{
    FlatformFileUtils::purgeCachedEntries();
}
