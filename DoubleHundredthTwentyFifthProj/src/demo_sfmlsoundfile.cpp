#include "demo_sfmlsoundfile.h"

#include "spriterengine_settings.h"

#include "demo_sfmlsoundobjectinforeference.h"

namespace SpriterEngine
{
	SfmlSoundFile::SfmlSoundFile(std::string initialFilePath) :
		SoundFile(initialFilePath)
	{
		initializeFile();
	}

	void SfmlSoundFile::initializeFile()
	{
		if (!buffer.loadFromFile(path()))
		{
			Settings::error("SfmlSoundFile::initializeFile - sfml sound buffer unable to load file from path \"" + path() + "\"");
		}
	}

	SoundObjectInfoReference * SfmlSoundFile::newSoundInfoReference()
	{
		return new SfmlSoundObjectInfoReference(buffer);
	}

}
