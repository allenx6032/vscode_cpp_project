#include "demo_exampleobjectfactory.h"

#include "demo_sfmlpointinstanceinfo.h"
#include "demo_sfmlboxinstanceinfo.h"
#include "demo_sfmlboneinstanceinfo.h"
#include "demo_sfmlsoundobjectinforeference.h"

namespace SpriterEngine
{

	ExampleObjectFactory::ExampleObjectFactory(sf::RenderWindow *validRenderWindow) :
		renderWindow(validRenderWindow)
	{
	}

	PointInstanceInfo * ExampleObjectFactory::newPointInstanceInfo()
	{
		return new SfmlPointInstanceInfo(renderWindow);
	}

	BoxInstanceInfo * ExampleObjectFactory::newBoxInstanceInfo(point size)
	{
		return new SfmlBoxInstanceInfo(size, renderWindow);
	}

	BoneInstanceInfo * ExampleObjectFactory::newBoneInstanceInfo(point size)
	{
		return new SfmlBoneInstanceInfo(size, renderWindow);
	}

}