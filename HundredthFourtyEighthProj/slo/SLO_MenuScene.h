#ifndef SCENE_MENU_MENUSCENE_H_
#define SCENE_MENU_MENUSCENE_H_

#include "SLO_Scene.h"
#include "SLO_MenuController.h"
#include "SLO_MenuDataStore.h"
#include "SLO_MenuPresenter.h"
#include "SLO_MenuUseCase.h"
#include <memory>
#include <string>
#include <vector>

class MenuScene: public Scene
{
public:
	explicit MenuScene(uint32_t windowId);
	virtual ~MenuScene() override = default;

	virtual void dispatch(const SDL_Event &) override;
	virtual bool onIdle(uint32_t) override;
	virtual std::shared_ptr<SceneResumeCommand> onSuspend() override;
	virtual void onCreate() override;
	virtual void onDestroy() override;
	virtual void onAddJoystick(int) override;
	void onResume(int, int, bool);

	void reload();

private:
	MenuDataStore dataStore_;
	MenuPresenter presenter_;
	MenuUseCase useCase_;
	MenuController controller_;
};

class MenuSceneResumeCommand : public SceneResumeCommand
{
public:
	MenuSceneResumeCommand(uint32_t windowId, int posMain, int posSub, bool mode);
	virtual ~MenuSceneResumeCommand() override = default;
	virtual std::shared_ptr<Scene> resume() override;

private:
	const uint32_t windowId_;
	const int posMain_;
	const int posSub_;
	const bool mode_;
};

#endif // SCENE_MENU_MENUSCENE_H_
