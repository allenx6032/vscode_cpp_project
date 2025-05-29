////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_ResourceInfo.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_Resource.h"
#include "Gugu_Memory.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {
    
ResourceInfo::ResourceInfo()
{
    resource = nullptr;
}

ResourceInfo::~ResourceInfo()
{
    SafeDelete(resource);
}

bool ResourceInfo::CompareID(const ResourceInfo* pLeft, const ResourceInfo* pRight)
{
    return pLeft->resourceID < pRight->resourceID;
}

}   // namespace gugu
