#ifndef JSONSPRITERFILEDOCUMENTWRAPPER_H
#define JSONSPRITERFILEDOCUMENTWRAPPER_H

#include "spriterengine_spriterfiledocumentwrapper.h"
#include "json.hpp"

using json = nlohmann::json;

namespace SpriterEngine
{
	class JSONSpriterFileDocumentWrapper : public SpriterFileDocumentWrapper
	{
	public:
		JSONSpriterFileDocumentWrapper();

		void loadFile(std::string fileName) override;

	private:
		SpriterFileElementWrapper *newElementWrapperFromFirstElement() override;
		SpriterFileElementWrapper *newElementWrapperFromFirstElement(const std::string & elementName) override;

		json m_json;
	};

}

#endif // JSONSPRITERFILEDOCUMENTWRAPPER_H
