#include "player_HelloWorldScene.h"
#include "player_MainScene.h"
#include <stdlib.h>

USING_NS_CC;

inline float randomf1()
{
    return (float)rand() / (float)RAND_MAX;
}

inline float randomf(float from, float to)
{
    return randomf1() * (to - from) + from;
}

inline float map(float val, float inputMin, float inputMax, float outputMin, float outputMax)
{
    /*
     var inputRange = inputMax - inputMin

     var inputFraction = (val - inputMin)/inputRange

     var outputRange = outputMax - outputMin

     var output = (outputRange * inputFraction) + outputMin

     return output
     */

    return ((outputMax - outputMin) * ((val - inputMin)/(inputMax - inputMin))) + outputMin;
}

inline float randomPlusMinus(float chance = 0)
{
    chance = chance ? chance : 0.5;
    return (randomf1() > chance) ? -1 : 1;
}

inline int randomInt(int from, int to)
{
    to += 1;
    return (int)randomf(from, to);
}

bool randomBool(float chance = 0)
{
    chance = chance ? chance : 0.5;
    return (randomf1() < chance) ? true : false;
}


Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

Vector<SpriteFrame*> bunnyTextures;
bool HelloWorld::init()
{
    if (!Layer::init()) {
        return false;
    }

    timeval t;
    gettimeofday(&t, NULL);
    srand(t.tv_sec * 10000 + t.tv_usec);

    auto size = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    maxX = size.width;
    maxY = size.height;

    count = startBunnyCount;

    std::string textureFilename("res/bunnys.png");
    bunnyTextures.pushBack(SpriteFrame::create(textureFilename, Rect(2, 47, 26, 37)));
    bunnyTextures.pushBack(SpriteFrame::create(textureFilename, Rect(2, 86, 26, 37)));
    bunnyTextures.pushBack(SpriteFrame::create(textureFilename, Rect(2, 125, 26, 37)));
    bunnyTextures.pushBack(SpriteFrame::create(textureFilename, Rect(2, 164, 26, 37)));
    bunnyTextures.pushBack(SpriteFrame::create(textureFilename, Rect(2, 2, 26, 37)));
    size_t bunnyType = 2;

    SpriteFrame *currentTexture = bunnyTextures.at(bunnyType);

    for (int i = 0; i < startBunnyCount; i++) {
        Bunny *bunny = new Bunny();
        bunny->sprite = Sprite::createWithSpriteFrame(bunnyTextures.at(i % 5));
        bunny->sprite->setPosition(Vec2(minX + 10, maxY - 30));
        bunny->speedX = randomf1() * 10.0f;
        bunny->speedY = randomf1() * 10.0f - 5.0f;
        bunny->sprite->setAnchorPoint(Vec2(0.5, 0));

        bunnys.push_back(bunny);
        addChild(bunny->sprite);
    }

	{
		auto clickTipsSprite = Sprite::create("res/click.png");
		clickTipsSprite->setPosition(size.width / 2, size.height - clickTipsSprite->getContentSize().height);
		addChild(clickTipsSprite, 1);
	}

	{
		auto label = Label::createWithSystemFont("BACK", "sans", 32);
		label->setAnchorPoint(ccp(1, 1));
		label->setPosition(size.width, size.height);
		auto touch = EventListenerTouchOneByOne::create();
        touch->setSwallowTouches(true);
		touch->onTouchBegan = [=](Touch* touch, Event*) -> bool {
			if (label->getBoundingBox().containsPoint(touch->getLocation())) {
				Director::getInstance()->replaceScene(TransitionFade::create(0.5, MainScene::create()));
                return true;
            }
            return false;
		};

		getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, label);
		addChild(label, 1);
	}

    scheduleUpdate();

    EventListenerTouchOneByOne* touch = EventListenerTouchOneByOne::create();
    touch->onTouchBegan = [=](Touch*, Event*) -> bool {
        for (int i = 0; i < 300; i++) {
            Bunny *bunny = new Bunny();
            bunny->sprite = Sprite::createWithSpriteFrame(bunnyTextures.at(i % 5));
            bunny->sprite->setPosition(Vec2(minX + 10, maxY - 30));
            bunny->speedX = randomf1() * 10.0f;
            bunny->speedY = randomf1() * 10.0f - 5.0f;
            bunny->sprite->setAnchorPoint(Vec2(0.5, 0));

            bunnys.push_back(bunny);
            addChild(bunny->sprite);
        }
        printf("%lu", bunnys.size());
        return true;
    };
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);
    return true;
}

void HelloWorld::update(float dt)
{
    size_t count = bunnys.size();
    Bunny *bunny;
    for (size_t i = 0; i < count; i++) {
        bunny = bunnys[i];

        Vec2 pos = bunny->sprite->getPosition();
        pos.x += bunny->speedX;
        pos.y -= bunny->speedY;
        bunny->speedY += gravity;

        if (pos.x > maxX) {
            bunny->speedX *= -1;
            pos.x = maxX;
        } else if (pos.x < minX) {
            bunny->speedX *= -1;
            pos.x = minX;
        }

        if (pos.y < minY) {
            bunny->speedY *= -0.85;
            pos.y = minY;
            bunny->sprite->setRotation((randomf1() - 0.5) * 0.2f);
            if (randomBool()) {
                bunny->speedY -= randomf1() * 6.0f;
            }
        } else if (pos.y > maxY) {
            bunny->speedY = 0;
            pos.y = maxY;
        }

        bunny->sprite->setPosition(pos);
    }

    fpsTime += dt;
    fpsCount++;
    if (fpsTime > 1.0)
    {
        printf("%lu bunnies %.2f FPS\n", bunnys.size(), (fpsCount / fpsTime));
        fpsTime = 0;
        fpsCount = 0;
    }
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *unused_event)
{
    return true;
}
