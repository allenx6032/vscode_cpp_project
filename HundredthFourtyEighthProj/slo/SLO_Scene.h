#if !defined(SCENE_H_)
#define SCENE_H_

#include "SLO_Misc.h"
#include <memory>

namespace SDL_
{
class Window;
}

class Scene;
class SceneResumeCommand
{
public:
	UNCOPYABLE(SceneResumeCommand);
	SceneResumeCommand() = default;
	virtual ~SceneResumeCommand() = default;
	virtual std::shared_ptr<Scene> resume() = 0;
};

union SDL_Event;
class TaskManager;
class Task;
class Scene
{
public:
	UNCOPYABLE(Scene);
	explicit Scene(uint32_t);
	virtual ~Scene() = default;

	void setNextScene(std::shared_ptr<Scene>);
	void finish() { isFinished_ = true; }

	bool isChangeScene() const { return isFinished_ || nextScene_; }
	bool isFinished() const { return isFinished_; }
	std::shared_ptr<Scene> getNextScene() const { return nextScene_; }
	TaskManager &getTaskManager() const { return *manager_; }
	void setTaskManager(TaskManager &manager);
	void unsetTaskManager();
	void registerTask(int, std::shared_ptr<Task>&&);
	void unregisterTask(int, bool);

	virtual void dispatch(const SDL_Event &) = 0;
	virtual std::shared_ptr<SceneResumeCommand> onSuspend() = 0;
	virtual void onAddJoystick(int);
	virtual bool onIdle(uint32_t);
	virtual void onCreate();
	virtual void onDestroy();

	std::shared_ptr<SDL_::Window> getWindow() { return window_; }
	void update();
	void clear();
	void swap();
	void quit();

	static void calcViewPort(int, int, int, int);
	static void Enter2DMode();
	static void Leave2DMode();
	static void Enter3DMode();
	static void Leave3DMode();

private:
	std::shared_ptr<Scene> nextScene_;
	std::shared_ptr<SDL_::Window> window_;
	TaskManager *manager_;
	bool isFinished_;
};

#endif // SCENE_H_
