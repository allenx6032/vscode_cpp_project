#include "SLO_MenuPresenter.h"
#include "SLO_MenuDataStore.h"
#include "SLO_MenuScene.h"
#include "SLO_GLUCompatible.h"
#include "SLO_SDLTtfFont.h"
#include "SLO_SDLColor.h"
#include "SLO_GLTexture.h"
#include "SLO_GLSprite2D.h"
#include "SLO_GLSprite3D.h"
#include "SLO_GLBillBoard.h"
#include "SLO_SDLImage.h"
#include "SLO_Interpolator.h"
#include "SLO_GLTextView.h"
#include "SLO_GLGroupView.h"
#include "SLO_GLScreenView.h"
#include "SLO_MenuInfo.h"
#include "SLO_Application.h"
#include "SLO_Resources.h"
#include "SLO_ParallelTask.h"
#include "SLO_SerialTask.h"
#include "SLO_Interpolator.h"
#include "SLO_ActionTask.h"
#include <SDL_opengl.h>
#include <iostream>

MenuPresenter::MenuPresenter(MenuScene &scene, const MenuDataStore &dataStore)
	: Presenter(scene)
	, scene_(scene)
	, dataStore_(dataStore)
	, rotater_()
	, title_(std::make_shared<GL_::TextView>())
	, mixer_()
	, chunk_("res/sound/cursor7.wav")
	, optionMenu_(dataStore.getScreenWidth(), dataStore.getScreenHeight() / 8)
	, yoffset_(0.0)
	, center_(std::make_shared<GL_::Sprite2D>())
{
	auto fontSize = dataStore.getFontSize();
	SDL_::TtfFont font(dataStore.getFontFileName(), fontSize);

	auto root = getRootView();
	root->setBack(dataStore.getTextureScreenBack());

	GL_::GroupView::addChild(root, title_);
	title_->setXPos(32.0);
	title_->setYPos(32.0);
	title_->setBack(dataStore_.getTextureTextBack());
	title_->setWidth(400.0);
	title_->setHeight(80.0);
	title_->setText(font, dataStore.getStringTitle(), {0, 0, 0, 255});
	title_->setYAlign(GL_::ViewYAlign::Center);
	title_->setXAlign(GL_::ViewXAlign::Left);
	title_->keepInbound(false);
	title_->keepAspect(true);
	title_->setPaddingLeft(20.0);
	title_->setPaddingBottom(16.0);

	//init();

	mixer_.allocateChannels(dataStore_.getMenuInfoList().size());

	//optionMenu_.setXPos(0.0);
	optionMenu_.setYPos(dataStore.getScreenHeight());

	center_->setTexture(dataStore_.getTextureButtonNormalBack());
	center_->setXPos(0.0f);
	center_->setYPos(0.0f);
	center_->setZPos(0.0f);
	center_->setWidth(1.0f);
	center_->setHeight(1.0f);
}

MenuPresenter::~MenuPresenter()
{
}

void MenuPresenter::selectCursor(const std::string &value)
{
//	scene_.startNextScene(value);
}

void MenuPresenter::exposed()
{
	// Clear back buffer
	::glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Scene::Enter2DMode();
	getRootView()->draw();
	Scene::Leave2DMode();

	Scene::Enter3DMode();
	//center_->draw();
	gluLookAtCompatible(
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 1.0f, 0.0f
	);
	glTranslatef(0.0f, 0.0f, -40.0f);
	glRotatef(dataStore_.getDegrees(), 1.0f, 0.0f, 0.0f);

	glDepthMask(GL_FALSE);
	rotater_.setRadius(dataStore_.getRadius());
	rotater_.draw();
	glDepthMask(GL_TRUE);
	Scene::Leave3DMode();

	Scene::Enter2DMode();
	optionMenu_.draw();
	Scene::Leave2DMode();

	// Swap back and front buffer
	scene_.swap();
}

bool MenuPresenter::moveMainMenuCursor(int oldPos, int move)
{
	const auto size = rotater_.getMenuItemListSize();
	int newPos = oldPos + move;
	if (newPos < 0) {
		newPos += size;
	}
	else if (size <= newPos) {
		newPos -= size;
	}

	if (move != 0) {
		mixer_.playChannel(-1, chunk_, 0);
		const auto tick = Application::getTickCount();
		// サークルメニューの回転制御
		auto p = rotater_.computeDgrees(oldPos, move, CursorSelectingDuration);
		const auto degrees = p.first;
		const auto duration = p.second;
		{
			std::cout << "oldPos = " << oldPos << ", move = " << move << ", newPos = " << newPos << ", curr degrees = " << rotater_.getDegrees() << ", end degrees = " << degrees << ", duration = " << duration << std::endl;
			if (duration == 0) {
				return false;
			}
			scene_.unregisterTask(ID_ROTATE_MENU, false);
			auto animeRotate = ActionTask::get();
			animeRotate->setup(rotater_.getDegrees(), degrees, duration, tick);
			animeRotate->setAction([this](double value) {
				rotater_.setDegrees(value);
			});
			animeRotate->setInterpolator(InterpolatorFactory::create(InterpolatorType::Linear));
			scene_.registerTask(ID_ROTATE_MENU, animeRotate);
		}
		{
			scene_.unregisterTask(oldPos, false);
			auto taskNewIcon = ParallelTask::get();
			auto newIcon = rotater_.getMenuItem(newPos);
			{
				auto animeZOffset = ActionTask::get();
				animeZOffset->setup(newIcon->getZOffset(), MainMenuIconZOffsetSelect, duration, tick);
				animeZOffset->setInterpolator(InterpolatorFactory::create(CursorSelectInterpolatorType));
				animeZOffset->setAction([newIcon](double value){
					newIcon->setZOffset(value);
				});
				taskNewIcon->addTask(animeZOffset);
			}
			{
				auto animeAlpha = ActionTask::get();
				animeAlpha->setup(newIcon->getAlpha(), MainMenuIconAlphaSelect, duration, tick);
				animeAlpha->setInterpolator(InterpolatorFactory::create(CursorSelectInterpolatorType));
				animeAlpha->setAction([newIcon](double value){
					newIcon->setAlpha(value);
				});
				taskNewIcon->addTask(animeAlpha);
			}
			scene_.registerTask(newPos, taskNewIcon);
		}
		{
			auto taskOldIcon = ParallelTask::get();
			auto oldIcon = rotater_.getMenuItem(oldPos);
			{
				auto animeZOffset = ActionTask::get();
				animeZOffset->setup(oldIcon->getZOffset(), MainMenuIconZOffsetNormal, duration, tick);
				animeZOffset->setInterpolator(InterpolatorFactory::create(CursorNormalInterpolatorType));
				animeZOffset->setAction([oldIcon](double value){
					oldIcon->setZOffset(value);
				});
				taskOldIcon->addTask(animeZOffset);
			}
			{
				auto animeAlpha = ActionTask::get();
				animeAlpha->setup(oldIcon->getAlpha(), MainMenuIconAlphaNormal, duration, tick);
				animeAlpha->setInterpolator(InterpolatorFactory::create(CursorNormalInterpolatorType));
				animeAlpha->setAction([oldIcon](double value){
					oldIcon->setAlpha(value);
				});
				taskOldIcon->addTask(animeAlpha);
			}
			scene_.registerTask(oldPos, taskOldIcon);
		}
	}
	else {
		{
			auto oldIcon = rotater_.getMenuItem(oldPos);
			oldIcon->setAlpha(MainMenuIconAlphaNormal);
			oldIcon->setZOffset(MainMenuIconZOffsetNormal);
		}
		{
			auto newIcon = rotater_.getMenuItem(newPos);
			newIcon->setAlpha(MainMenuIconAlphaSelect);
			newIcon->setZOffset(dataStore_.getZOffset());
		}
	}
	return true;
}

void MenuPresenter::moveSubMenuCursor(int oldPos, int newPos, bool animation)
{
	if (animation) {
		mixer_.playChannel(-1, chunk_, 0);
	}
	optionMenu_.moveCursor(newPos, animation);
}

void MenuPresenter::changeMode(bool mode, bool animation)
{
	/*
	if (animation) {
		mixer_.playChannel(-1, chunk_, 0);
	}
	auto tick = Application::getTickCount();
	if (mode) {
		const auto SelectOptionMenuHeight = dataStore_.getScreenHeight() / 3;
		if (animation) {
			const auto OptionMenuHeightChangeDuration = 200;
			const auto SelectOptionMenuHeightChangeInterpolatorType = InterpolatorType::AccelerateDecelerate;
			animationList_.addAction(optionMenu_.getHeight(), SelectOptionMenuHeight, OptionMenuHeightChangeDuration, tick, SelectOptionMenuHeightChangeInterpolatorType, [this](double value){
				optionMenu_.setHeight(value);
			});
			animationList_.addAction(MainMenuCameraEyeYOffsetNormal, MainMenuCameraEyeYOffsetSelect, OptionMenuHeightChangeDuration, tick, SelectOptionMenuHeightChangeInterpolatorType, [this](double value){
				yoffset_ = value;
			});
		}
		else {
			optionMenu_.setHeight(SelectOptionMenuHeight);
		}
	}
	else {
		const auto NormalOptionMenuHeight = dataStore_.getScreenHeight() / 8;
		if (animation) {
			const auto OptionMenuHeightChangeDuration = 200;
			const auto NormalOptionMenuHeightChangeInterpolatorType = InterpolatorType::AccelerateDecelerate;
			animationList_.addAction(optionMenu_.getHeight(), NormalOptionMenuHeight, OptionMenuHeightChangeDuration, tick, NormalOptionMenuHeightChangeInterpolatorType, [this](double value){
				optionMenu_.setHeight(value);
			});
			animationList_.addAction(MainMenuCameraEyeYOffsetSelect, MainMenuCameraEyeYOffsetNormal, OptionMenuHeightChangeDuration, tick, NormalOptionMenuHeightChangeInterpolatorType, [this](double value){
				yoffset_ = value;
			});
		}
		else {
			optionMenu_.setHeight(NormalOptionMenuHeight);
		}
	}
	*/
}

void MenuPresenter::action(uint32_t tick, int posMainMenu)
{
	setIdling(false);
	exposed();
}

void MenuPresenter::init()
{
	optionMenu_.clear();

	const auto xslide = 132;
	auto xpos = 0;
	for (auto info : dataStore_.getMenuInfoList()) {
		std::shared_ptr<GL_::Sprite3D> mainIcon;
//		if (dataStore_.isBillboard()) {
//			mainIcon = std::make_shared<GL_::BillBoard>(std::make_shared<SDL_::Image>(info->getPath()));
//		}
//		else {
			mainIcon = std::make_shared<GL_::Sprite3D>(std::make_shared<SDL_::Image>(info->getPath()));
//		}
		mainIcon->setWidth(4.0);
		mainIcon->setHeight(3.0);
		mainIcon->setAlpha(MainMenuIconAlphaNormal);
		rotater_.addMenuItem(mainIcon);

		auto subIcon = std::make_shared<GL_::Sprite2D>(std::make_shared<SDL_::Image>(info->getPath()), GL_::Sprite::XOrigin::Center, GL_::Sprite::YOrigin::Bottom);
		subIcon->setWidth(120.0);
		subIcon->setHeight(90.0);
		subIcon->setXPos(xpos);
		subIcon->setYPos(-(optionMenu_.getHeight() - subIcon->getHeight()) / 2);
		optionMenu_.addIcon(subIcon);
		xpos += xslide;
	}
}
