#ifndef SCENE_MENU_MENUUSECASE_H_
#define SCENE_MENU_MENUUSECASE_H_

#include "SLO_UseCase.h"
#include <vector>
#include <string>

class MenuPresenter;
class MenuDataStore;
class MenuUseCase : public UseCase
{
public:
	UNCOPYABLE(MenuUseCase);
	MenuUseCase(MenuPresenter &, MenuDataStore &);
	virtual ~MenuUseCase();

	void pushEnter();
	void moveUpCursor();
	void moveDownCursor();
	void moveLeftCursor();
	void moveRightCursor();
	void action(uint32_t);
	void onResume(int, int, bool);
	void onCreate();
	void init(int, int, bool);

	int getMainMenuPos() const { return posMain_; }
	void setMainMenuPos(int pos) { posMain_ = pos; }
	int getSubMenuPos() const { return posSub_; }
	void setSubMenuPos(int pos) { posSub_ = pos; }
	bool getMode() const { return mode_; }
	void setMode(bool mode) { mode_ = mode; }

	void addDegrees(float);
	void addRadius(float);
	void addScale(float);
//	void addYPos(float);
	void addCameraYEye(float);
	void addCameraYCenter(float);
	void addZPos(float);
	void addViewAngle(float);
	void addZOffset(float);
	void showParam() const;
	void flipBillboard();

private:
	MenuPresenter &presenter_;
	MenuDataStore &dataStore_;
	int posMain_;
	int posSub_;
	bool mode_;
};

#endif // SCENE_MENU_MENUUSECASE_H_
