#ifndef SCENE_OPENING_OPENINGUSECASE_H_
#define SCENE_OPENING_OPENINGUSECASE_H_

#include "SLO_UseCase.h"
#include <vector>
#include <string>

class OpeningPresenter;
class OpeningDataStore;
class OpeningUseCase : public UseCase
{
public:
	OpeningUseCase(OpeningPresenter &, OpeningDataStore &);
	virtual ~OpeningUseCase() override;

	void onCreate();
	void onResume(int);
	void pushEnter();
	void pushCancel();
	void pushUp();
	void pushDown();
	void pushLeft();
	void pushRight();
	void action(uint32_t);

	int getPos() const { return pos_; }
	void setPos(int pos) { pos_ = pos; }

private:
	OpeningPresenter &presenter_;
	OpeningDataStore &dataStore_;
	const std::vector<const char *> &listMenuItem_;
	int pos_;
};

#endif // SCENE_OPENING_OPENINGUSECASE_H_
