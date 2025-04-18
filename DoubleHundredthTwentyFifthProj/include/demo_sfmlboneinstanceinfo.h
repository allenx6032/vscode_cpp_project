#ifndef SFMLBONEINSTANCEINFO_H
#define SFMLBONEINSTANCEINFO_H

#include "SFML_RenderWindow.hpp"

#include "SFML_ConvexShape.hpp"

#include "spriterengine_boneinstanceinfo.h"

namespace SpriterEngine
{

	class SfmlBoneInstanceInfo : public BoneInstanceInfo
	{
	public:
		SfmlBoneInstanceInfo(point initialSize, sf::RenderWindow *validRenderWindow);

		void render() override;

	private:
		sf::RenderWindow *renderWindow;

		sf::ConvexShape boneShape;
	};

}

#endif // SFMLBONEINSTANCEINFO_H
