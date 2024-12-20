#ifndef SCENE_USECASE_H_
#define SCENE_USECASE_H_

#include "SLO_Misc.h"

class Presenter;
class UseCase
{
public:
	UNCOPYABLE(UseCase);
	UseCase(Presenter &);
	virtual ~UseCase();

	void exposed();
	void finish();
	void quit();

private:
	Presenter &presenter_;
};

#endif // SCENE_USECASE_H_
