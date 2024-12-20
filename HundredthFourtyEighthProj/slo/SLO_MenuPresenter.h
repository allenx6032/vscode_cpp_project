#ifndef SCENE_MENU_MENUPRESENTER_H_
#define SCENE_MENU_MENUPRESENTER_H_

#include "SLO_Presenter.h"
#include "SLO_OptionMenu.h"
#include "SLO_SDLSoundMixer.h"
#include "SLO_SDLSoundChunk.h"
#include "SLO_Interpolator.h"
#include "SLO_HorizontalRotateMenu.h"
#include <vector>
#include <memory>

namespace GL_
{
class Sprite;
class BillBoard;
class Sprite2D;
class Sprite3D;
class TextView;
class GroupView;
class ScreenView;
}

class MenuScene;
class MenuDataStore;
class MenuPresenter : public Presenter
{
public:
	MenuPresenter(MenuScene &scene, const MenuDataStore &dataStore);
	virtual ~MenuPresenter() override;

	void selectCursor(const std::string &);
	virtual void exposed() override;
	bool moveMainMenuCursor(int, int);
	void moveSubMenuCursor(int, int, bool);
	void changeMode(bool, bool);
	void action(uint32_t, int);
	void init();

private:
	MenuScene &scene_;
	const MenuDataStore &dataStore_;
	//std::vector<std::shared_ptr<GL_::Sprite>> listMenuIcon_;
	GL_::HorizontalRotateMenu rotater_;
	std::shared_ptr<GL_::TextView> title_;
	SDL_::SoundMixer mixer_;
	SDL_::SoundChunk chunk_;
	OptionMenu optionMenu_;
	double yoffset_;
	std::shared_ptr<GL_::Sprite> center_;

	static constexpr int ID_ROTATE_MENU = 100;

	static constexpr auto MainMenuIconAlphaNormal = 160.0;
	static constexpr auto MainMenuIconAlphaSelect = 255.0;
	static constexpr auto MainMenuIconZOffsetNormal = 0.0;
	static constexpr auto MainMenuIconZOffsetSelect = 10.0;
	static constexpr auto MainMenuCameraEyeYOffsetNormal = 0.0;
	static constexpr auto MainMenuCameraEyeYOffsetSelect = 1.8;

	static constexpr uint32_t CursorSelectingDuration = 100;
	static const InterpolatorType CursorSelectInterpolatorType = InterpolatorType::Accelerate;
	static const InterpolatorType CursorNormalInterpolatorType = InterpolatorType::Decelerate;
};

#endif // SCENE_MENU_MENUPRESENTER_H_
