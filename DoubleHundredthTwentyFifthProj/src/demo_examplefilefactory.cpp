#include "demo_examplefilefactory.h"

#include "spriterengine_imagefile.h"
#include "spriterengine_soundfile.h"

// #define __USE_PUGIXML
#ifndef __USE_PUGIXML
#include "demo_tinyxmlspriterfiledocumentwrapper.h"
#else
#include "demo_pugixmlspriterfiledocumentwrapper.h"
#endif

#include "demo_jsonspriterfiledocumentwrapper.h"

#include "demo_sfmlimagefile.h"
#include "demo_sfmlatlasfile.h"
#include "demo_sfmlsoundfile.h"

namespace SpriterEngine
{

	ExampleFileFactory::ExampleFileFactory(sf::RenderWindow *validRenderWindow) :
		renderWindow(validRenderWindow)
	{
	}

	ImageFile * ExampleFileFactory::newImageFile(const std::string &initialFilePath, point initialDefaultPivot, atlasdata atlasData)
	{
		return new SfmlImageFile(initialFilePath, initialDefaultPivot, renderWindow);
	}

	AtlasFile *ExampleFileFactory::newAtlasFile(const std::string &initialFilePath)
	{
		return new SfmlAtlasFile(initialFilePath);

	}

	SoundFile * ExampleFileFactory::newSoundFile(const std::string & initialFilePath)
	{
		return new SfmlSoundFile(initialFilePath);
	}

	SpriterFileDocumentWrapper * ExampleFileFactory::newScmlDocumentWrapper()
	{
#ifndef __USE_PUGIXML
		return new TinyXmlSpriterFileDocumentWrapper();
#else
		return new PugiXmlSpriterFileDocumentWrapper();		
#endif
	}

	SpriterFileDocumentWrapper *ExampleFileFactory::newSconDocumentWrapper()
	{
		return new JSONSpriterFileDocumentWrapper();
	}
}
