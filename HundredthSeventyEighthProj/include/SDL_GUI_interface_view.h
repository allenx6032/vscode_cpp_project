#pragma once

#include <SDL.h>

#include "SDL_GUI_view_base.h"
#include "SDL_GUI_interface_model.h"

namespace SDL_GUI {
/** View that renders the drawable tree to the applications renderer. */
class InterfaceView : public ViewBase {
protected:
    SDL_Renderer *_renderer;                    /**< SDL Renderer to render on */
    const InterfaceModel *_interface_model;     /**< The applications interface model */
public:
    /**
     * Constructor
     * @param renderer The SDL renderer to render on
     * @param interface_model The applications interface model
     */
    InterfaceView(SDL_Renderer *renderer, const InterfaceModel *interface_model);

    bool init() override;
    void deinit() override;
    void update() override;
    void render() override;
};
}
