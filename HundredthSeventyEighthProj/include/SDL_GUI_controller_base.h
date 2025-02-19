#pragma once

#include "SDL_GUI_model_base.h"

namespace SDL_GUI {
/** Base class for all the controllers.  */
class ControllerBase {
public:
    int _weight = 99;
    /** Default destructor */
    virtual ~ControllerBase() = default;

    /**
     * function that gets triggered every iteration in the applications main loop. Has to be
     * overridden
     */
    virtual void update() = 0;

protected:
    /** Constructor */
    ControllerBase() = default;
};
}
