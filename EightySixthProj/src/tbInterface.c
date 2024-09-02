#include "tbConfig.h"
#include "tbInterface.h"
#include "tbMemory.h"

tbInterface* tbCreateInterface(tbSimulation* simulation,tbWindow* window)
{
	tbInterface* interface_;

	interface_=tbAlloc(sizeof(interface_));
	interface_->simulation=simulation;
	interface_->window=window;

	return interface_;
}

int tbUpdateInterface(tbInterface* interface_)
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch(event.button.button)
				{
					case SDL_BUTTON_LEFT:
						{
							const tbWindow* window=interface_->window;
							const tbVector point={window->center[0]+((tbScalar)event.button.x-(tbScalar)window->width/2)*(2*window->scale),
												window->center[1]+((tbScalar)window->height/2-(tbScalar)event.button.y)*(2*window->scale)};
							const tbVector min={point[0]-0.2f,point[1]-0.2f},max={point[0]+0.2f,point[1]+0.2f};
							tbBroadphase* broadphase=interface_->simulation->broadphase;

							/* TODO : select object to be grabbed */
							
							tbSetCursor(interface_->window,TB_CURSOR_HAND);
							interface_->state=TB_INTERFACE_GRAB;
						}
						break;
					case SDL_BUTTON_RIGHT:
						tbSetCursor(interface_->window,TB_CURSOR_MOVE);
						interface_->state=TB_INTERFACE_MOVE;
						break;
					default:
						tbSetCursor(interface_->window,TB_CURSOR_ARROW);
						interface_->state=0;
						break;
				}
				break;
			case SDL_MOUSEMOTION:
				switch(interface_->state)
				{
					case TB_INTERFACE_GRAB:
						/* TODO : grab object */
						break;
					case TB_INTERFACE_MOVE:
						{
							tbWindow* window=interface_->window;
							tbVector center={window->center[0]-event.motion.xrel*window->scale*2,
								window->center[1]+event.motion.yrel*window->scale*2};
							tbSetView(window,center,window->scale);
						}
						break;
				}
				break;
			case SDL_MOUSEBUTTONUP:	
				tbSetCursor(interface_->window,TB_CURSOR_ARROW);
				interface_->state=0;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym==SDLK_SPACE)
					interface_->simulation->flags^=TB_SIMULATION_PAUSED;
				if(event.key.keysym.sym==SDLK_a)
					tbSetView(interface_->window,interface_->window->center,interface_->window->scale*(tbScalar)0.8);
				if(event.key.keysym.sym==SDLK_b)
					tbSetView(interface_->window,interface_->window->center,interface_->window->scale*(tbScalar)1.25);
				break;
			case SDL_KEYUP:	
				if(interface_->state)
					interface_->state=0;
				break;
			case SDL_QUIT:
				return 0;
		}
	}
	return 1;
}

void tbDestroyInterface(tbInterface* interface_)
{
	tbFree(interface_);
}
