#include "SLO_SDLWindowView.h"
#include "SLO_SDLRenderer.h"
#include "SLO_SDLWindow.h"

namespace SDL_
{

WindowView::WindowView(std::shared_ptr<Window> window)
	: GroupView()
{
	this->setPos(window->getPosition());
	this->setSize(window->getSize());
}

void WindowView::onDraw(Renderer& renderer)
{
	const Color tmp_color = renderer.getDrawColor();
	renderer.clear();
	GroupView::onDraw(renderer);
	renderer.present();
	renderer.setDrawColor(tmp_color);
}

} // SDL_
