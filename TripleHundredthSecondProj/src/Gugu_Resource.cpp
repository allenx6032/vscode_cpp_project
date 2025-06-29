////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_Resource.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_ManagerResources.h"
#include "Gugu_ResourceInfo.h"
#include "Gugu_String.h"
#include "Gugu_Logger.h"
#include "Gugu_PugiXmlUtility.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {
    
Resource::Resource()
{
    m_resourceInfos = nullptr;
}

Resource::~Resource()
{
}

void Resource::Init(ResourceInfo* _pResourceInfos)
{
    m_resourceInfos = _pResourceInfos;
}

const std::string& Resource::GetID() const
{
    return m_resourceInfos->resourceID;
}

const FileInfo& Resource::GetFileInfo() const
{
    return m_resourceInfos->fileInfo;
}

EResourceType::Type Resource::GetResourceType() const
{
    return EResourceType::Unknown;
}

bool Resource::LoadFromFile()
{
    if (!m_resourceInfos)
    {
        GetLogEngine()->Print(ELog::Error, ELogEngine::Resources, StringFormat("Resource cannot be loaded without ResourceInfos"));
        return false;
    }

    return LoadFromXmlFile();
}

bool Resource::LoadFromString(const std::string& source)
{
    return LoadFromXmlString(source);
}

bool Resource::SaveToFile() const
{
    if (!m_resourceInfos)
    {
        GetLogEngine()->Print(ELog::Error, ELogEngine::Resources, StringFormat("Resource cannot be saved without ResourceInfos"));
        return false;
    }

    return SaveToXmlFile();
}

bool Resource::SaveToString(std::string& result) const
{
    return SaveToXmlString(result);
}

bool Resource::LoadFromXmlFile()
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(GetFileInfo().GetFileSystemPath().c_str());
    if (!result)
        return false;

    return LoadFromXml(doc);
}

bool Resource::LoadFromXmlString(const std::string& source)
{
    pugi::xml_document doc;
    if (!xml::ParseDocumentFromString(source, doc))
        return false;

    return LoadFromXml(doc);
}

bool Resource::SaveToXmlFile() const
{
    pugi::xml_document doc;

    if (!SaveToXml(doc))
        return false;

    return doc.save_file(GetFileInfo().GetFileSystemPath().c_str(), PUGIXML_TEXT("\t"), pugi::format_default, pugi::encoding_utf8);
}

bool Resource::SaveToXmlString(std::string& result) const
{
    pugi::xml_document doc;

    if (!SaveToXml(doc))
        return false;

    result = xml::SaveDocumentToString(doc);
    return true;
}

void Resource::Unload()
{
    GetLogEngine()->Print(ELog::Error, ELogEngine::Resources, StringFormat("Unload is not supported for this type of resource : {0}", m_resourceInfos->fileInfo.GetFilePath_utf8()));
}

bool Resource::LoadFromXml(const pugi::xml_document& document)
{
    GetLogEngine()->Print(ELog::Error, ELogEngine::Resources, StringFormat("LoadFromXml is not supported for this type of resource : {0}", m_resourceInfos->fileInfo.GetFilePath_utf8()));
    return false;
}

bool Resource::SaveToXml(pugi::xml_document& document) const
{
    GetLogEngine()->Print(ELog::Error, ELogEngine::Resources, StringFormat("SaveToXml is not supported for this type of resource : {0}", m_resourceInfos->fileInfo.GetFilePath_utf8()));
    return false;
}

void Resource::GetDependencies(std::set<Resource*>& dependencies) const
{
    // No dependencies by default.
}

void Resource::OnDependencyUpdated(const Resource* dependency)
{
    // No dependencies by default.
}

void Resource::OnDependencyRemoved(const Resource* dependency)
{
    // No dependencies by default.
}

}   // namespace gugu
