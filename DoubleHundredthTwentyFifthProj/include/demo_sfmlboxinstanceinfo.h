#ifndef SFMLBOXINSTANCEINFO_H
#define SFMLBOXINSTANCEINFO_H

#include "SFML_RenderWindow.hpp"

#include "SFML_RectangleShape.hpp"

#include "spriterengine_boxinstanceinfo.h"

namespace SpriterEngine
{

	class SfmlBoxInstanceInfo : public BoxInstanceInfo
	{
	public:
		SfmlBoxInstanceInfo(point initialSize, sf::RenderWindow *validRenderWindow);

		void render() override;

	private:
		sf::RenderWindow *renderWindow;

		sf::RectangleShape rectangle;
	};

}

#endif // SFMLBOXINSTANCEINFO_H
