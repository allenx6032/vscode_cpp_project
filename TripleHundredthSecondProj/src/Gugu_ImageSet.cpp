////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_ImageSet.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_ManagerResources.h"
#include "Gugu_Container.h"
#include "Gugu_PugiXmlUtility.h"
#include "Gugu_Texture.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {
    
SubImage::SubImage(ImageSet* imageSet)
: m_imageSet(imageSet)
{
}

SubImage::SubImage(ImageSet* imageSet, const std::string& name, const sf::IntRect& rect)
: m_imageSet(imageSet)
, m_name(name)
, m_rect(rect)
{
}

SubImage::~SubImage()
{
}

ImageSet* SubImage::GetImageSet() const
{
    return m_imageSet;
}

const std::string& SubImage::GetName() const
{
    return m_name;
}

void SubImage::SetName(const std::string& name)
{
    m_name = name;
}

bool SubImage::IsName(const std::string& name) const
{
    return m_name == name;
}

const sf::Rect<int>& SubImage::GetRect() const
{
    return m_rect;
}

void SubImage::SetRect(const sf::IntRect& _oRect)
{
    m_rect = _oRect;
}


ImageSet::ImageSet()
{
    m_texture = nullptr;
}

ImageSet::~ImageSet()
{
    Unload();
}

void ImageSet::SetTexture(Texture* _pTexture)
{
    m_texture = _pTexture;
}

Texture* ImageSet::GetTexture() const
{
    return m_texture;
}

SubImage* ImageSet::AddSubImage(const std::string& _strName)
{
    SubImage* pSubImage = new SubImage(this);
    m_subImages.push_back(pSubImage);
    pSubImage->SetName(_strName);
    return pSubImage;
}

bool ImageSet::DeleteSubImage(SubImage* _pSubImage)
{
    if (!_pSubImage)
        return false;

    auto iteSubImage = StdVectorFind(m_subImages, _pSubImage);
    if (iteSubImage != m_subImages.end())
    {
        m_subImages.erase(iteSubImage);
        SafeDelete(_pSubImage);
        return true;
    }

    return false;
}

void ImageSet::DeleteAllSubImages()
{
    ClearStdVector(m_subImages);
}

SubImage* ImageSet::GetSubImage(size_t _uiIndex) const
{
    if (_uiIndex >= 0 && _uiIndex < m_subImages.size())
        return m_subImages[_uiIndex];
    return nullptr;
}

SubImage* ImageSet::GetSubImage(const std::string& _strName) const
{
    for (size_t i = 0; i < m_subImages.size(); ++i)
    {
        if (m_subImages[i]->IsName(_strName))
            return m_subImages[i];
    }

    return nullptr;
}

const std::vector<SubImage*>& ImageSet::GetSubImages() const
{
    return m_subImages;
}

size_t ImageSet::GetSubImageCount() const
{
    return m_subImages.size();
}

EResourceType::Type ImageSet::GetResourceType() const
{
    return EResourceType::ImageSet;
}

void ImageSet::GetDependencies(std::set<Resource*>& dependencies) const
{
    if (m_texture)
    {
        dependencies.insert(m_texture);
    }
}

void ImageSet::OnDependencyRemoved(const Resource* removedDependency)
{
    if (m_texture == removedDependency)
    {
        m_texture = nullptr;
    }
}

void ImageSet::Unload()
{
    DeleteAllSubImages();
    m_texture = nullptr;
}

bool ImageSet::LoadFromXml(const pugi::xml_document& document)
{
    Unload();

    pugi::xml_node nodeRoot = document.child("ImageSet");
    if (!nodeRoot)
        return false;

    pugi::xml_attribute oAttributeTexture = nodeRoot.attribute("texture");
    if (oAttributeTexture)
        m_texture = GetResources()->GetTexture(oAttributeTexture.as_string());

    for (pugi::xml_node oNodeSubImage = nodeRoot.child("SubImage"); oNodeSubImage; oNodeSubImage = oNodeSubImage.next_sibling("SubImage"))
    {
        std::string strNameSubImage = oNodeSubImage.attribute("name").as_string();
        if (!strNameSubImage.empty())
        {
            m_subImages.push_back(new SubImage(this, strNameSubImage, xml::ReadRect(oNodeSubImage)));
        }
    }

    return true;
}

bool ImageSet::SaveToXml(pugi::xml_document& document) const
{
    pugi::xml_node nodeRoot = document.append_child("ImageSet");
    nodeRoot.append_attribute("serializationVersion") = 1;

    nodeRoot.append_attribute("texture") = (!m_texture) ? "" : m_texture->GetID().c_str();

    for (const SubImage* subImage : m_subImages)
    {
        pugi::xml_node nodeSubImage = nodeRoot.append_child("SubImage");
        nodeSubImage.append_attribute("name") = subImage->GetName().c_str();
        xml::WriteRect(nodeSubImage, subImage->GetRect());
    }

    return true;
}

}   // namespace gugu
