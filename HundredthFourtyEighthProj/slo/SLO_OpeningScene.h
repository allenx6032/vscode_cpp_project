#ifndef SCENE_OPENING_OPENINGSCENE_H_
#define SCENE_OPENING_OPENINGSCENE_H_

#include "SLO_Scene.h"
#include "SLO_OpeningController.h"
#include "SLO_OpeningDataStore.h"
#include "SLO_OpeningPresenter.h"
#include "SLO_OpeningUseCase.h"
#include <memory>

class OpeningScene: public Scene
{
public:
	explicit OpeningScene(uint32_t windowId);
	virtual ~OpeningScene() override = default;

	virtual void dispatch(const SDL_Event &) override;
	virtual bool onIdle(uint32_t) override;
	virtual std::shared_ptr<SceneResumeCommand> onSuspend() override;
	virtual void onCreate() override;
	virtual void onDestroy() override;
	virtual void onAddJoystick(int) override;
	void onResume(int);

private:
	OpeningDataStore dataStore_;
	OpeningPresenter presenter_;
	OpeningUseCase useCase_;
	OpeningController controller_;
};

class OpeningSceneResumeCommand : public SceneResumeCommand
{
public:
	OpeningSceneResumeCommand(uint32_t windowId, int pos);
	virtual ~OpeningSceneResumeCommand() override = default;
	virtual std::shared_ptr<Scene> resume() override;

private:
	const uint32_t windowId_;
	const int pos_;
};

#endif // SCENE_OPENING_OPENINGSCENE_H_
