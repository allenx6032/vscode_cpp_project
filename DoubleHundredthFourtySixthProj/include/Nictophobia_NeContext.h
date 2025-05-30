#ifndef NECORE_NECONTEXT_H_
#define NECORE_NECONTEXT_H_

#include <vector>
#include "Nictophobia_AssetsLoader.h"
#include "Nictophobia_InputBinding.h"
#include "Nictophobia_FreeCamera.h"

class Window;
class Stage;
class Camera;
class FreeCamera;

class NeContext {
public:
	Window* window;
	Assets assets;
	std::vector<assets_asker_func> askers;
	InputBindings<std::string> bindings;
	Stage* stage = nullptr;
	Stage* stage3d = nullptr;
	Camera* camera = nullptr;
	FreeCamera freeCamera;
	unsigned long long timer = 0;
	unsigned long long frameTimeMicros = 0;

	NeContext(Window* window);
	virtual ~NeContext();
};

#endif /* NECORE_NECONTEXT_H_ */
