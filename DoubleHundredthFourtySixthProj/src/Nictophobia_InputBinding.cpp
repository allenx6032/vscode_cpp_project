#include "Nictophobia_InputBinding.h"
#include "Nictophobia_input_constants.h"

InputBinding::InputBinding(trigger_func trigger) : trigger(trigger) {
}

void InputBinding::update() {
	bool active = trigger();
	if (active) {
		if (status == INPUT_INACTIVE || status == INPUT_JUST_INACTIVED) {
			for (hotkey_callback callback : callbacks) {
				callback();
			}
			status = INPUT_JUST_ACTIVED;
		}
		else {
			status = INPUT_ACTIVE;
		}
	} else {
		if (status == INPUT_ACTIVE || status == INPUT_JUST_ACTIVED) {
			status = INPUT_JUST_INACTIVED;
		}
		else {
			status = INPUT_INACTIVE;
		}
	}
}

void InputBinding::setTrigger(trigger_func trigger) {
	this->trigger = trigger;
}

bool InputBinding::isActive() const {
	return status > 0;
}

bool InputBinding::justActived() const {
	return status == INPUT_JUST_ACTIVED;
}

bool InputBinding::justInactived() const {
	return status == INPUT_JUST_INACTIVED;
}


template <typename T>
InputBindings<T>::~InputBindings() {
	for (auto it = bindings.begin(); it != bindings.end(); it++) {
	    delete it->second;
	}
}

template <typename T>
void InputBindings<T>::bind(T name, trigger_func trigger) {
	auto binding = bindings.find(name);
	if (binding == bindings.end()) {
		bindings[name] = new InputBinding(trigger);
	}
}

template <typename T>
void InputBindings<T>::rebind(T name, trigger_func trigger) {
	auto binding = bindings.find(name);
	if (binding == bindings.end()) {
		bindings[name] = new InputBinding(trigger);
	} else {
		binding->second->setTrigger(trigger);
	}
}

template <typename T>
void InputBindings<T>::unbind(T name) {
	auto binding = bindings.find(name);
	if (binding != bindings.end()) {
		bindings.erase(binding);
	}
}

template <typename T>
void InputBindings<T>::update() {
	for (auto it = bindings.begin(); it != bindings.end(); it++) {
	    it->second->update();
	}
}

template <typename T>
void InputBindings<T>::hotkey(T name, hotkey_callback callback) {
	auto binding = bindings.find(name);
	if (binding != bindings.end()) {
		binding->second->addCallback(callback);
	}
}

template <typename T>
bool InputBindings<T>::isActive(T name) {
	auto binding = bindings.find(name);
	if (binding == bindings.end()) {
		return false;
	}
	return binding->second->isActive();
}

template <typename T>
bool InputBindings<T>::justActived(T name) {
	auto binding = bindings.find(name);
	if (binding == bindings.end()) {
		return false;
	}
	return binding->second->justActived();
}

template <typename T>
bool InputBindings<T>::justInactived(T name) {
	auto binding = bindings.find(name);
	if (binding == bindings.end()) {
		return false;
	}
	return binding->second->justInactived();
}
