#include "SLO_OpeningUseCase.h"
#include "SLO_OpeningDataStore.h"
#include "SLO_OpeningPresenter.h"

OpeningUseCase::OpeningUseCase(OpeningPresenter &presenter, OpeningDataStore &dataStore)
	: UseCase(presenter)
	, presenter_(presenter)
	, dataStore_(dataStore)
	, listMenuItem_(dataStore_.getOpeningMenuItemList())
	, pos_(0)
{
}

OpeningUseCase::~OpeningUseCase()
{
}

void OpeningUseCase::onCreate()
{
	setPos(0);
	presenter_.update();
}

void OpeningUseCase::onResume(int pos)
{
	setPos(pos);
	presenter_.update();
}

void OpeningUseCase::pushEnter()
{
	presenter_.startMenuScene();
}

void OpeningUseCase::pushCancel()
{
	presenter_.finish();
}

void OpeningUseCase::pushUp()
{
	const auto oldPos = pos_;
	const auto newPos = --pos_;
	if (pos_ < 0) {
		pos_ = listMenuItem_.size() - 1;
	}
	presenter_.moveCursor(oldPos, newPos, true);
}

void OpeningUseCase::pushDown()
{
	const auto oldPos = pos_;
	const auto newPos = ++pos_;
	if (pos_ >= static_cast<int>(listMenuItem_.size())) {
		pos_ = 0;
	}
	presenter_.moveCursor(oldPos, newPos, true);
}

void OpeningUseCase::pushLeft()
{
}

void OpeningUseCase::pushRight()
{
}

void OpeningUseCase::action(uint32_t tick)
{
	presenter_.action(tick);
}
