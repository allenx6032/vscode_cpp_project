#ifndef SFMLSOUNDOBJECTINFOREFERENCE_H
#define SFMLSOUNDOBJECTINFOREFERENCE_H

#include "spriterengine_soundobjectinforeference.h"

#include "SFML_Audio.hpp"

namespace SpriterEngine
{
	

	class SfmlSoundObjectInfoReference : public SoundObjectInfoReference
	{
	public:
		SfmlSoundObjectInfoReference(sf::SoundBuffer &buffer);

		void playTrigger() override;

	private:
		void setPlaybackVolume() override;
		void setPlaybackPanning() override;

		sf::Sound sound;
	};

}

#endif // SFMLSOUNDOBJECTINFOREFERENCE_H
