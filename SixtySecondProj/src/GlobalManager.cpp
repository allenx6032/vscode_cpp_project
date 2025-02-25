
#include <vector>
#include "CanvasObject.h"
#include "GlobalManager.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include "Float2.h"

using namespace std::chrono;

GlobalManager::GlobalManager() {
    screenWidth = new int();
    screenHeight = new int();
    *screenWidth = 800;
    *screenHeight = 800;
}

GlobalManager *GlobalManager::getInstance() {
    static GlobalManager *instance = new GlobalManager();
    return instance;
}

void GlobalManager::keyboard(int key) {
    std::vector<CanvasObject *> callbackCaller = objects;
    for (int i = callbackCaller.size() - 1; i >= 0; i--) {
        if (!callbackCaller[i]->checkIfCanExecuteCallback() || !callbackCaller[i]->isValid)
            continue;
        callbackCaller[i]->keyboard(key);
    }
    cleanUpObjects();

}

void GlobalManager::keyboardUp(int key) {
    std::vector<CanvasObject *> callbackCaller = objects;
    for (int i = callbackCaller.size() - 1; i >= 0; i--) {
        if (!callbackCaller[i]->checkIfCanExecuteCallback() || !callbackCaller[i]->isValid)
            continue;
        callbackCaller[i]->keyboardUp(key);
    }
    cleanUpObjects();

}

void GlobalManager::mouse(int button, int state, int wheel, int direction, int x, int y) {
    std::vector<CanvasObject *> callbackCaller = objects;
    mousePosition = {(float)x, (float)y};
    for (int i = callbackCaller.size() - 1; i >= 0; i--) {
        if (!callbackCaller[i]->checkIfCanExecuteCallback() || !callbackCaller[i]->isValid)
            continue;
        callbackCaller[i]->mouse(button, state, wheel, direction, x, y);
    }
    cleanUpObjects();

}

void GlobalManager::render() {
    currentTime = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(currentTime - lastTime);
    deltaTime = time_span.count();
    time += deltaTime;
    if (time > lastFpsUpdate + fpsUpdateCycle) {
        fps = 1.0 / deltaTime;
        lastFpsUpdate = time;
    }
    std::vector<CanvasObject *> callbackCaller = objects;
    for (int i = callbackCaller.size() - 1; i >= 0; i--) {
        if (!callbackCaller[i]->checkIfCanExecuteCallback() || !callbackCaller[i]->isValid)
            continue;
        callbackCaller[i]->render();
    }
    cleanUpObjects();

    lastTime = currentTime;
}

int GlobalManager::registerObject(CanvasObject *object) {
    addObjectToList(object);
    return objectIdCounter++;
}

void GlobalManager::addObjectToList(CanvasObject *object) {
    int size = objects.size();
    if (size == 0) {
        objects.push_back(object);
    } else {
        for (int i = 0; i < size; i++) {
            if (objects[i]->getZIndex() <= object->getZIndex()) {
                objects.insert(objects.begin() + i, object);
                break;
            }
        }
        if (size == objects.size())
            objects.push_back(object);
    }
}

bool GlobalManager::isMouseInsideObject(CanvasObject *object) {
    for (int i = 0; i < objects.size(); i++) {
        if (!objects[i]->getActive())
            continue;
        if (objects[i]->pointIntersectsObject(Float3(mousePosition, 0))) {
            if (object == objects[i]) {
                return true;
            }
            return false;
        }
    }
    return false;
}

void GlobalManager::changeObjectZIndex(CanvasObject *object) {
    auto iterator = std::find(objects.begin(), objects.end(), object);
    if (iterator != objects.cend()) {
        objects.erase(iterator);
        addObjectToList(object);
    }
}

CanvasObject *GlobalManager::unregisterObject(CanvasObject *object) {
    auto iterator = std::find(objects.begin(), objects.end(), object);
    objects.erase(iterator);
    return object;
}

CanvasObject *GlobalManager::deleteObject(CanvasObject *object) {
    object->isValid = false;
}

CanvasObject *GlobalManager::cleanUpObjects() {
    for (int i = 0; i < objects.size(); ++i) {
        if (!objects[i]->isValid) {
            delete objects[i];
        }
    }
}