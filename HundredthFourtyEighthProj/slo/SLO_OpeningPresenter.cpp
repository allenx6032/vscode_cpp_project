#include "SLO_OpeningPresenter.h"
#include "SLO_OpeningDataStore.h"
#include "SLO_OpeningScene.h"
#include "SLO_MenuScene.h"
#include "SLO_Application.h"
#include "SLO_GLUCompatible.h"
#include "SLO_SDLWindow.h"
#include "SLO_SDLTtfFont.h"
#include "SLO_SDLColor.h"
#include "SLO_GLSprite2D.h"
#include "SLO_GLSprite3D.h"
#include "SLO_SDLImage.h"
#include "SLO_GLTextView.h"
#include "SLO_GLScreenView.h"
#include "SLO_Interpolator.h"
#include "SLO_ActionTask.h"
#include <SDL_opengl.h>

OpeningPresenter::OpeningPresenter(OpeningScene &scene, const OpeningDataStore &dataStore)
	: Presenter(scene)
	, scene_(scene)
	, dataStore_(dataStore)
	, listMenuItem_(dataStore_.getOpeningMenuItemList())
	, mixer_()
	, chunk_("res/sound/cursor7.wav")
{
	auto root = getRootView();
	root->setBack(dataStore.getTextureScreenBack());

	auto fontSize = dataStore.getTitleFontSize();
	SDL_::TtfFont fontTitle(dataStore.getFontFileName(), fontSize);

	auto title = std::make_shared<GL_::TextView>();
	GL_::GroupView::addChild(root, title);
	title->setText(fontTitle, dataStore.getStringTitle(), Color::WHITE);
	title->setWrapContentSize();
	title->centeringXPos();
	title->setYPos(100);

	init();
	mixer_.allocateChannels(listMenuItem_.size());

}

OpeningPresenter::~OpeningPresenter()
{
}

void OpeningPresenter::exposed()
{
	::glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Scene::Enter2DMode();
	getRootView()->draw();
	Scene::Leave2DMode();

	Scene::Enter3DMode();
	gluLookAtCompatible(
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 1.0f, 0.0f
	);
	glTranslated(0.0, 0.0, -40.0);
	rotateMenu_.draw();
	Scene::Leave3DMode();

	scene_.swap();
}

bool OpeningPresenter::moveCursor(int oldPos, int newPos, bool isAnimation)
{
	const auto pair = rotateMenu_.computeDgrees(oldPos, newPos - oldPos, 100);
	const auto degrees = pair.first;
	const auto duration = pair.second;
	if (duration == 0) {
		return false;
	}

	scene_.unregisterTask(ID_ROTATE_MENU, false);
	if (isAnimation) {
		mixer_.playChannel(-1, chunk_, 0);
		auto animation = ActionTask::get();
		animation->setup(rotateMenu_.getDegrees(), degrees, duration, Application::getTickCount());
		animation->setInterpolator(InterpolatorFactory::create(InterpolatorType::AccelerateDecelerate));
		animation->setAction([this](double value) {
			rotateMenu_.setDegrees(value);
		});
		scene_.registerTask(ID_ROTATE_MENU, std::move(animation));
	}
	else {
		rotateMenu_.setDegrees(degrees);
	}
	return true;
}

void OpeningPresenter::action(uint32_t tick)
{
	setIdling(false);
	exposed();
}

void OpeningPresenter::startMenuScene()
{
	scene_.setNextScene(std::make_shared<MenuScene>(scene_.getWindow()->getWindowId()));
}

void OpeningPresenter::startSettingScene()
{
}

void OpeningPresenter::init()
{
	auto fontSize = dataStore_.getMenuFontSize();
	SDL_::TtfFont font(dataStore_.getFontFileName(), fontSize);

	rotateMenu_.clearMenuItem();
	rotateMenu_.setRadius(dataStore_.getRadius());
	for (auto item : listMenuItem_) {
		auto sprite = std::make_shared<GL_::Sprite3D>();
		auto imageMenuString = font.renderBlendedText(item, Color::WHITE);
		sprite->setTexture(imageMenuString);
		sprite->setWidth(imageMenuString->getWidth() * 4 / imageMenuString->getHeight());
		sprite->setHeight(4.0);
		rotateMenu_.addMenuItem(sprite);
	}
}
