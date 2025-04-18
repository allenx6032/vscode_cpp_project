#include "demo_pugixmlspriterfiledocumentwrapper.h"

#include "demo_pugixmlspriterfileelementwrapper.h"

namespace SpriterEngine
{
    PugiXmlSpriterFileDocumentWrapper::PugiXmlSpriterFileDocumentWrapper()
    {
    }

    void PugiXmlSpriterFileDocumentWrapper::loadFile(std::string fileName)
    {
        doc.load_file(fileName.c_str());
    }

    SpriterFileElementWrapper * PugiXmlSpriterFileDocumentWrapper::newElementWrapperFromFirstElement()
    {
        return new PugiXmlSpriterFileElementWrapper(doc.first_child());
    }

    SpriterFileElementWrapper * PugiXmlSpriterFileDocumentWrapper::newElementWrapperFromFirstElement(const std::string & elementName)
    {
        return new PugiXmlSpriterFileElementWrapper(doc.child(elementName.c_str()));
    }

}
