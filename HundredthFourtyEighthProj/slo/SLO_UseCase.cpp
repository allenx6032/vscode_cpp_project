#include "SLO_UseCase.h"
#include "SLO_Presenter.h"

UseCase::UseCase(Presenter &presenter)
	: presenter_(presenter)
{
}

UseCase::~UseCase()
{
}

void UseCase::exposed()
{
	presenter_.exposed();
}

void UseCase::finish()
{
	presenter_.finish();
}

void UseCase::quit()
{
	presenter_.quit();
}
