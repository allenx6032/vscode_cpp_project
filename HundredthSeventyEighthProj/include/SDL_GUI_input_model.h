#pragma once

#include <map>
#include <set>

#include <SDL.h>

#include "SDL_GUI_model_base.h"
#include "SDL_GUI_position.h"

namespace SDL_GUI {
/** Base Model for all input related data */
class InputModelBase : public ModelBase {
protected:
    Position _mouse_position;   /**< current position of the mouse */
    Position _mouse_offset;     /**< difference between current and previous mouse position */
    Position _mouse_wheel;      /**< mouse wheel difference */
public:
    /** Reset mouse wheel difference */
    void update();

    /**
     * Setter for the mouse position.
     * This will also change the mouse offset.
     * @param position current position of the mouse
     */
    void set_mouse_position(Position position);

    /**
     * getter for _mouse_position
     * @returns this->_mouse_position
     */
    Position mouse_position() const;

    /**
     * getter for _mouse_offset
     * @returns this->_mouse_offset
     */
    Position mouse_offset() const;

    /**
     * Setter for the mouse wheel change
     * @param offset mouse wheel change
     */
    void set_mouse_wheel(Position offset);

    /**
     * getter for _mouse_wheel
     * @returns this->_mouse_wheel
     */
    Position mouse_wheel() const;

};

/**
 * Atual input model that also holds information about possible input values
 * @tparam IType enum with all the possible input values
 * @tparam IState enum with all the possible input states
 */
template<typename IType, typename IState>
class InputModel : public InputModelBase {
    /* make sure IType and IState are actually enums */
    static_assert(std::is_enum<IType>::value);
    static_assert(std::is_enum<IState>::value);

    std::set<IType> _pressed;   /**< active (pressed) inputs */
    std::set<IType> _down;      /**< inputs that just got activated (pressed) */
    std::set<IType> _up;        /**< inputs that just got deactivated (released) */
    std::set<IType> _triggered; /**< inputs that just got triggered */

    IState _current_state;
    IState _default_state;

    bool _state_changed = false;

public:
    /** Constructor */
    InputModel(IState default_input_state)
        : _current_state(default_input_state), _default_state(default_input_state) {}

    /** Clear _down and _up */
    virtual void update() {
        InputModelBase::update();
        this->_down.clear();
        this->_up.clear();
        this->_triggered.clear();
        if (this->_state_changed) {
            this->_pressed.clear();
            this->_state_changed = false;
        }
    }

    const std::set<IType> pressed() const {
        return this->_pressed;
    }

    const std::set<IType> down() const {
        return this->_down;
    }

    const std::set<IType> up() const {
        return this->_up;
    }

    IState state() const {
        return this->_current_state;
    }

    IState default_state() const {
        return this->_default_state;
    }

    /**
     * set the state of a given input to be pressed
     * @param input The input to set as pressed
     */
    virtual void press(IType input) {
        this->_down.insert(input);
        this->_pressed.insert(input);
    }

    /**
     * Set the state of a given input to be pressed just for this one tick.
     * This is used for events that will not get released.
     * @param input The input to trigger
     */
    virtual void trigger(IType input) {
        this->_triggered.insert(input);
    }

    /**
     * set the state of a given input to be released
     * @param input The input to set as released
     */
    virtual void release(IType input) {
        // if (not this->_pressed.contains(input)) {
        if ( !(this->_pressed.find(input) != this->_pressed.end())) {
            return;
        }
        this->_pressed.erase(input);
        this->_up.insert(input);
    }

    void set_state(IState state, bool clear_current = true) {
        if (state == this->_current_state) {
            return;
        }
        this->_current_state = state;
        this->_state_changed = clear_current;
    }

    /**
     * check if a given key is pressed
     * @param key the key to check
     * @returns True if key is pressed. False otherwise
     */
    bool is_pressed(IType key) const {
        return this->_pressed.contains(key) or this->_triggered.contains(key);
    }

    /**
     * check if a given key has just been pressed
     * @param key the key to check
     * @returns True if key has pressed down. False otherwise
     */
    bool is_down(IType key) const {
        return this->_down.contains(key) or this->_triggered.contains(key);
    }

    /**
     * check if a given key has just been released
     * @param key the key to check
     * @returns True if key has just been released. False otherwise
     */
    bool is_up(IType key) const {
        return this->_up.contains(key) or this->_triggered.contains(key);
    }
};
}
