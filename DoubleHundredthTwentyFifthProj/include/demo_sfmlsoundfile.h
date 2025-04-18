#ifndef SFMLSOUNDFILE_H
#define SFMLSOUNDFILE_H

#include "spriterengine_soundfile.h"

#include "SFML_Audio.hpp"

namespace SpriterEngine
{

	class SfmlSoundFile : public SoundFile
	{
	public:
		SfmlSoundFile(std::string initialFilePath);

		SoundObjectInfoReference * newSoundInfoReference();

	private:
		sf::SoundBuffer buffer;

		void initializeFile();
	};

}

#endif // SFMLSOUNDFILE_H
