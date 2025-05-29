////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_Datasheet.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_ManagerResources.h"
#include "Gugu_ResourceInfo.h"
#include "Gugu_DatasheetObject.h"
#include "Gugu_Container.h"
#include "Gugu_String.h"
#include "Gugu_Logger.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

Datasheet::Datasheet()
    : m_rootObject(nullptr)
{
}

Datasheet::~Datasheet()
{
    Unload();
}

EResourceType::Type Datasheet::GetResourceType() const
{
    return EResourceType::Datasheet;
}

void Datasheet::Unload()
{
    m_rootObject = nullptr;
    ClearStdMap(m_instanceObjects);
}

bool Datasheet::LoadFromFile()
{
    Unload();

    // TODO: handle extensions with several dots (like name.type.xml instead of name.type).
    m_rootObject = dynamic_cast<DatasheetObject*>(GetResources()->InstanciateDataObject(m_resourceInfos->fileInfo.GetExtension()));
    if (!m_rootObject)
    {
        GetLogEngine()->Print(ELog::Error, ELogEngine::Resources, StringFormat("Could not instantiate Datasheet Root Object : {0}", GetID()));
        return false;
    }

    std::vector<Datasheet*> ancestors;
    ancestors.push_back(this);

    return m_rootObject->LoadFromFile(GetFileInfo().GetFileSystemPath(), this, ancestors);
}

const DatasheetObject* Datasheet::GetRootObject() const
{
    return m_rootObject;
}

}   // namespace gugu
