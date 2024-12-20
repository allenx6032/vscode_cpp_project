#ifndef SDLWINDOWVIEW_H_
#define SDLWINDOWVIEW_H_

#include "SLO_SDLGroupView.h"

namespace SDL_
{

class Window;
class Renderer;
class WindowView final : public GroupView
{
public:
	WindowView(std::shared_ptr<Window> window);
	virtual ~WindowView() override = default;

protected:
	virtual void onDraw(Renderer &renderer) override;
};

} // SDL_

#endif // SDLWINDOWVIEW_H_
