#pragma once

#include "SDL_GUI_model_base.h"

namespace SDL_GUI {
/** Base class for all the views. */
class ViewBase {
public:
    int _weight = 99;
    /** Destructor */
    virtual ~ViewBase() = default;

    /**
     * initialise view. Has to be overridden by implementation
     * @return true if initialisation succeeded. Otherwise false.
     */
    virtual bool init() = 0;

    /** deinitialise view. Has to be overridden by implementation */
    virtual void deinit() = 0;

    /**
     * update view. This gets called every iteration of the Applications main loop. Has to be
     * overridden by implementaiton.
     */
    virtual void update() = 0;

    /**
     * render view. This gets called every iteration of the Applications main loop. Has to be
     * overridden by implementation.
     */
    virtual void render() = 0;

protected:
    /** Constructor */
    ViewBase() = default;

    ModelBase *_model = nullptr;    /**< pointer to the coupled model. */
};
}
