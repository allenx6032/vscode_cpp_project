#ifndef SFMLPOINTINSTANCEINFO_H
#define SFMLPOINTINSTANCEINFO_H

#include "SFML_RenderWindow.hpp"

#include "SFML_CircleShape.hpp"

#include "spriterengine_pointinstanceinfo.h"

namespace SpriterEngine
{

	class SfmlPointInstanceInfo : public PointInstanceInfo
	{
	public:
		SfmlPointInstanceInfo(sf::RenderWindow *validRenderWindow);

		void render() override;

	private:
		sf::RenderWindow *renderWindow;

		sf::CircleShape circle;
	};

}

#endif // SFMLPOINTINSTANCEINFO_H
