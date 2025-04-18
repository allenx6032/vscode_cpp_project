#include "spriterengine_filefactory.h"

#include "spriterengine_imagefile.h"
#include "spriterengine_soundfile.h"
#include "spriterengine_atlasfile.h"

#include "spriterengine_spriterfiledocumentwrapper.h"

namespace SpriterEngine
{

	FileFactory::FileFactory()
	{
	}

	FileFactory::~FileFactory()
	{
	}

	ImageFile * FileFactory::newImageFile(const std::string &initialFilePath, point initialDefaultPivot, atlasdata atlasData)
	{
		return new ImageFile(initialFilePath, initialDefaultPivot);
	}

	SoundFile * FileFactory::newSoundFile(const std::string &initialFilePath)
	{
		return new SoundFile(initialFilePath);
	}

	AtlasFile *FileFactory::newAtlasFile(const std::string &initialFilePath)
	{
		return new AtlasFile(initialFilePath);
	}

	SpriterFileDocumentWrapper * FileFactory::newScmlDocumentWrapper()
	{
		return 0;
	}

	SpriterFileDocumentWrapper * FileFactory::newSconDocumentWrapper()
	{
		return 0;
	}

}
