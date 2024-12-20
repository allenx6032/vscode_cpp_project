#include "SLO_MenuUseCase.h"
#include "SLO_MenuDataStore.h"
#include "SLO_MenuPresenter.h"
#include <iostream>

MenuUseCase::MenuUseCase(MenuPresenter &presenter, MenuDataStore &dataStore)
	: UseCase(presenter)
	, presenter_(presenter)
	, dataStore_(dataStore)
	, posMain_(0)
	, posSub_()
	, mode_()
{
}

MenuUseCase::~MenuUseCase()
{
}

void MenuUseCase::pushEnter()
{
}

void MenuUseCase::moveUpCursor()
{
	if (mode_) {
		mode_ = false;
		presenter_.changeMode(mode_, true);
	}
}

void MenuUseCase::moveDownCursor()
{
	if (!mode_) {
		mode_ = true;
		presenter_.changeMode(mode_, true);
	}
}

void MenuUseCase::moveLeftCursor()
{
	if (mode_) {
		if (posSub_ > 0) {
			const auto oldPos = posSub_--;
			presenter_.moveSubMenuCursor(oldPos, posSub_, true);
		}
	}
	else {
		if (presenter_.moveMainMenuCursor(posMain_, -1)) {
			if (--posMain_ < 0) {
				posMain_ = dataStore_.getMenuInfoList().size() - 1;
			}
		}
	}
}

void MenuUseCase::moveRightCursor()
{
	if (mode_) {
		if (posSub_+1 < dataStore_.getMenuInfoList().size()) {
			const auto oldPos = posSub_++;
			presenter_.moveSubMenuCursor(oldPos, posSub_, true);
		}
	}
	else {
		if (presenter_.moveMainMenuCursor(posMain_, 1)) {
			if (++posMain_ >= static_cast<int>(dataStore_.getMenuInfoList().size())) {
				posMain_ = 0;
			}
		}
	}
}

void MenuUseCase::action(uint32_t tick)
{
	presenter_.action(tick, posMain_);
}

void MenuUseCase::onResume(int posMain, int posSub, bool mode)
{
	init(posMain, posSub, mode);
}

void MenuUseCase::onCreate()
{
	init(0, 0, false);
}

void MenuUseCase::init(int posMain, int posSub, bool mode)
{
	setMainMenuPos(posMain);
	setSubMenuPos(posSub);
	presenter_.init();
	presenter_.changeMode(mode, false);
	presenter_.moveMainMenuCursor(posMain, 0);
	presenter_.moveSubMenuCursor(0, posSub, false);
	presenter_.update();
}

void MenuUseCase::addDegrees(float degrees)
{
	dataStore_.setDegrees(dataStore_.getDegrees() + degrees);
	presenter_.exposed();
}

void MenuUseCase::addRadius(float radius)
{
	dataStore_.setRadius(dataStore_.getRadius() + radius);
	presenter_.exposed();
}

void MenuUseCase::addScale(float scale)
{
	dataStore_.setScale(dataStore_.getScale() + scale);
	presenter_.exposed();
}

void MenuUseCase::addCameraYEye(float ypos)
{
	dataStore_.setCameraYEye(dataStore_.getCameraYEye() + ypos);
	presenter_.exposed();
}

void MenuUseCase::addCameraYCenter(float ypos)
{
	dataStore_.setCameraYCenter(dataStore_.getCameraYCenter() + ypos);
	presenter_.exposed();
}

void MenuUseCase::addZPos(float zpos)
{
	dataStore_.setZPos(dataStore_.getZPos() + zpos);
}

void MenuUseCase::addViewAngle(float viewAngle)
{
	dataStore_.addViewAngle(viewAngle);
}

void MenuUseCase::addZOffset(float zoffset)
{
	dataStore_.setZOffset(dataStore_.getZOffset() + zoffset);
}

void MenuUseCase::showParam() const
{
	std::cout << "========" << std::endl;
	std::cout << "radius = " << dataStore_.getRadius() << std::endl;
	std::cout << "degrees = " << dataStore_.getDegrees() << std::endl;
	std::cout << "scale = " << dataStore_.getScale() << std::endl;
	std::cout << "zpos = " << dataStore_.getZPos() << std::endl;
	std::cout << "camera y eye = " << dataStore_.getCameraYEye() << std::endl;
	std::cout << "camera y center = " << dataStore_.getCameraYCenter() << std::endl;
	std::cout << "zoffset = " << dataStore_.getZOffset() << std::endl;
	std::cout << "view angle = " << dataStore_.getViewAngle() << std::endl;

}

void MenuUseCase::flipBillboard()
{
	dataStore_.setBillboard(!dataStore_.isBillboard());
}
