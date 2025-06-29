////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_DatasaveObject.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_DataBindingUtility.h"
#include "Gugu_PugiXmlUtility.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

DatasaveObject::DatasaveObject()
{
}

DatasaveObject::~DatasaveObject()
{
}

bool DatasaveObject::LoadFromFile(const std::string& path)
{
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(path.c_str());
    if (!result)
        return false;

    pugi::xml_node rootNode = document.child("Datasave");
    if (rootNode.empty())
        return false;

    DataParseContext context;
    context.currentNode = &rootNode;
    ParseMembers(context);

    return true;
}

bool DatasaveObject::SaveToFile(const std::string& path) const
{
    pugi::xml_document document;

    if (!SaveToXml(document))
        return false;

    return document.save_file(path.c_str(), PUGIXML_TEXT("\t"), pugi::format_default, pugi::encoding_utf8);
}

bool DatasaveObject::SaveToString(std::string& result) const
{
    pugi::xml_document document;

    if (!SaveToXml(document))
        return false;

    result = xml::SaveDocumentToString(document);
    return true;
}

bool DatasaveObject::SaveToXml(pugi::xml_document& document) const
{
    pugi::xml_node datasaveNode = document.append_child("Datasave");
    datasaveNode.append_attribute("serializationVersion").set_value(1);
    datasaveNode.append_attribute("bindingVersion").set_value(1);

    DataSaveContext context;
    context.currentNode = &datasaveNode;
    SerializeMembers(context);

    return true;
}

}   // namespace gugu
