#include "Scene.h"
#include "Utilities.h"
#include "BezierCurve.h"
#include "gl_canvas2d.h"
#include "GlobalManager.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include "ControlPoint.h"
#include "Float2.h"
#include "Float3.h"
#include "Float4.h"


Scene::Scene() {
    mode = SceneMode::Default;
    highlightColor = {242 / 255.0, 153 / 255.0, 0, 1};
    drawBounds = false;
    this->scale = Float3(*GlobalManager::getInstance()->screenWidth, *GlobalManager::getInstance()->screenHeight, 0);
    this->setZIndex(-10000);
    multipleSelect = false;
    drawControlPoints = true;
    drawControlGraph = false;
    drawAnimation = true;
    drawBounds = false;
    showBlendingFunctions = false;


    insertPolygon = new Button(position + Float3(20, 10, 0),
                               Float3(100, 30, 0), Float3(0, 0, 0), "Add Curve", Float3(1, 1, 1));
    insertPolygon->addListener([this] {
        setInsertMode();
    });
    showConvexHull = new Button(position + Float3(122, 10, 0),
                                Float3(120, 30, 0), Float3(0, 0, 0), "Convex Hull", Float3(1, 1, 1));
    showConvexHull->addListener([this] {
        drawBounds = !drawBounds;
        curveListManager.setDrawBounds(drawBounds, false);
    });
    showControlGraph = new Button(position + Float3(244, 10, 0),
                                  Float3(150, 30, 0), Float3(0, 0, 0), "Control Graph", Float3(1, 1, 1));
    showControlGraph->addListener([this] {
        drawControlGraph = !drawControlGraph;
        curveListManager.setDrawControlGraph(drawControlGraph, false);
    });
    showAnimation = new Button(position + Float3(396, 10, 0),
                               Float3(100, 30, 0), Float3(0, 0, 0), "Animate", Float3(1, 1, 1));
    showAnimation->addListener([this] {
        drawAnimation = !drawAnimation;
        curveListManager.setDrawAnimation(drawAnimation, false);
    });
    showBlendingFunctionsGraph = new Button(position + Float3(498, 10, 0),
                                            Float3(180, 30, 0), Float3(0, 0, 0), "Blending Functions", Float3(1, 1, 1));
    showBlendingFunctionsGraph->addListener([this] {
        curveListManager.activateBlendingFunctionGraph();
    });
}

void Scene::mouse(int button, int state, int wheel, int direction, int x, int y) {
    currentMousePosition = {(float)x, (float)y};
    switch (mode) {
        case SceneMode::Insert:
            handleInsertMode(button, state);
            break;
        case SceneMode::Translate:
            handleTranslateMode();
            break;
        case SceneMode::Rotate:
            handleRotateMode();
            break;
        case SceneMode::Scale:
            handleScaleMode();
            break;
        case SceneMode::Default:
            handleDefaultMode(button, state);
            break;
    }
    lastMousePosition = currentMousePosition;
}

void Scene::singleSelection(int x, int y) {
    if (isMouseInsideObject()) {
        curveListManager.clearSelectedCurve();
    }
    BezierCurve *fig = curveListManager.getFirstInteractedCurve({(float)x, (float)y});
    if (fig != nullptr) {
        //  if (!fig->isMouseInsideObject()) return;
        curveListManager.clearSelectedCurve();
        curveListManager.selectCurve(fig);
    }
}

void Scene::multipleSelection(int x, int y) {
    BezierCurve *fig = curveListManager.getFirstInteractedCurve({(float)x, (float)y});
    if (fig != nullptr) {
        //if (!fig->isMouseInsideObject()) return;
        curveListManager.selectCurve(fig);
    } else if (isMouseInsideObject()) {
        curveListManager.clearSelectedCurve();
    }
}

void Scene::insertNewcurve() {
    if (tmpVertices.size() < 2)
        return;
    Float3 lineColor = {0, 0, 0};
    std::vector<ControlPoint *> points;
    for (int i = 0; i < tmpVertices.size(); ++i) {
        ControlPoint *c = new ControlPoint(tmpVertices[i], 5);
        c->idx = i;
        points.push_back(c);
        c->setZIndex(10);
        c->setActive(drawControlPoints);
    }
    BezierCurve *curve = new BezierCurve(lineColor, highlightColor, points);
    curve->drawAnimation = drawAnimation;
    curve->drawControlGraph = drawControlGraph;
    curve->drawConvexHull = drawBounds;
    curveListManager.addCurve(curve);

}

void Scene::keyboard(int key) {
    if (mode == SceneMode::Default && key == 214) {
        multipleSelect = true;
    }
}

void Scene::keyboardUp(int key) {
    ignoreUpperCaseChar(key);
    handleSceneMode(static_cast<SceneMode>(key));
    handleSceneOperator(static_cast<Operator>(key));
}

void Scene::handleSceneMode(SceneMode sceneMode) {
    switch (sceneMode) {
        case SceneMode::Insert:
            setInsertMode();
            break;
        case SceneMode::Translate:
            if (curveListManager.isCurvesSelected())
                setTranslateMode();
            break;
        case SceneMode::Rotate:
            if (curveListManager.isCurvesSelected())
                setRotateMode();
            break;
        case SceneMode::Scale:
            if (curveListManager.isCurvesSelected())
                setScaleMode();
            break;
    }
}

void Scene::handleSceneOperator(Operator op) {
    switch (op) {
        case Operator::MultipleSelect:
            multipleSelect = false;
            break;
        case Operator::InsertPolygon:
            if (mode == SceneMode::Insert) {
                insertNewcurve();
                mode = SceneMode::Default;
            }
            break;
        case Operator::FixY:
            fixatedAxis = {0, 1};
            break;
        case Operator::FixX:
            fixatedAxis = {1, 0};
            break;
        case Operator::ConvexHull:
            drawBounds = !drawBounds;
            curveListManager.setDrawBounds(drawBounds, false);
            break;
        case Operator::DrawAnimation:
            drawAnimation = !drawAnimation;
            curveListManager.setDrawAnimation(drawAnimation, false);
            break;
        case Operator::ControlGraph:
            drawControlGraph = !drawControlGraph;
            curveListManager.setDrawControlGraph(drawControlGraph, false);
            break;
        case Operator::ShowControlPoints:
            drawControlPoints = !drawControlPoints;
            curveListManager.setDrawControlPoints(drawControlPoints, false);
            break;
        case Operator::ShowBlendingFunctions:
            curveListManager.activateBlendingFunctionGraph();
            break;
        case Operator::DeleteSelected:
            curveListManager.deleteSelectedcurves();
            break;
        default:
            break;
    }
}

void Scene::render() {
    color(0, 0, 0, 1);
    if (tmpVertices.size() > 0 && mode == SceneMode::Insert)
        renderPolygonInsertion();

    renderCurrentMode();

    if (mode == SceneMode::Rotate || mode == SceneMode::Scale) {
        line(selectionCenter.x, selectionCenter.y, currentMousePosition.x, currentMousePosition.y);
    }
}

void Scene::renderPolygonInsertion() {
    int size = tmpVertices.size();
    for (int i = 0; i < size; i++) {
        color(0, 0, 0);
        circle(tmpVertices[i].x, tmpVertices[i].y, 3, 10);
    }
    for (int i = 0; i < size - 1; i++) {
        color(1, 0, 0);
        line(tmpVertices[i].x, tmpVertices[i].y, tmpVertices[i + 1].x, tmpVertices[i + 1].y);
    }
}

void Scene::renderCurrentMode() {

    std::stringstream stream;
    int screenHeight = *GlobalManager::getInstance()->screenHeight;
    int screenWidth = *GlobalManager::getInstance()->screenWidth;
    stream << "FPS: " << GlobalManager::getInstance()->fps;
    color(0, 0, 0);
    text(screenWidth - 80, screenHeight - 10, stream.str().c_str());
    stream = std::stringstream();
    switch (mode) {
        case SceneMode::Default:
            color(0, 0, 0);
            text(20, screenHeight - 10, "Mode: Default");
            break;
        case SceneMode::Insert:
            color(0, 0, 0);
            stream << "Mode: Insert";
            text(20, screenHeight - 10, stream.str().c_str());
            text(20, screenHeight - 23, "Left mouse: Insert control point");
            text(20, screenHeight - 35, "I key: clear points");
            text(20, screenHeight - 47, "Enter key: finish insertion");
            break;
        case SceneMode::Translate:
            color(0, 0, 0);
            text(20, screenHeight - 10, "Mode: Translation");
            text(20, screenHeight - 23, "Move Mouse: Translate selected curves");
            text(20, screenHeight - 35, "T Key: finish translation");
            text(20, screenHeight - 47, "X Key: Fixate x axis");
            text(20, screenHeight - 59, "Y Key: Fixate y axis");
            break;
        case SceneMode::Rotate:
            color(0, 0, 0);
            text(20, screenHeight - 10, "Mode: Rotation");
            text(20, screenHeight - 23, "Move Mouse: Rotate selected curves");
            text(20, screenHeight - 35, "R Key: finish rotation");
            break;
        case SceneMode::Scale:
            color(0, 0, 0);
            text(20, screenHeight - 10, "Mode: Scaling");
            text(20, screenHeight - 23, "Move Mouse: Scale selected curves");
            text(20, screenHeight - 35, "S Key: finish scaling");
            text(20, screenHeight - 47, "X Key: Fixate x axis");
            text(20, screenHeight - 59, "Y Key: Fixate y axis");
            break;
        default:
            break;
    }
}

void Scene::ignoreUpperCaseChar(int &key) {
    if (key >= 'A' && key <= 'Z') {
        key = std::tolower(key);
    }
}

void Scene::setInsertMode() {
    mode = SceneMode::Insert;
    tmpVertices.clear();
}

void Scene::setTranslateMode() {
    if (mode == SceneMode::Translate) {
        mode = SceneMode::Default;
    } else {
        mode = SceneMode::Translate;
        lastMousePosition = GlobalManager::getInstance()->mousePosition;
        fixatedAxis = {1, 1};
    }
}

void Scene::setScaleMode() {
    if (mode == SceneMode::Scale) {
        mode = SceneMode::Default;
    } else {
        mode = SceneMode::Scale;
        lastMousePosition = GlobalManager::getInstance()->mousePosition;
        selectionCenter = curveListManager.calculateSelectedCurvesCenter();
        fixatedAxis = {1, 1};
    }
}

void Scene::setRotateMode() {
    if (mode == SceneMode::Rotate) {
        mode = SceneMode::Default;
    } else {
        mode = SceneMode::Rotate;
        selectionCenter = curveListManager.calculateSelectedCurvesCenter();
    }
}

void Scene::setDefaultMode() {
    mode = SceneMode::Default;
}

void Scene::handleInsertMode(int button, int state) {
    if (leftMouseDown(button, state)) {
        if (tmpVertices.size() < 40)
            tmpVertices.push_back({currentMousePosition.x, currentMousePosition.y, 0});
    }
}

void Scene::handleTranslateMode() {
    Float3 translation = {currentMousePosition.x - lastMousePosition.x, currentMousePosition.y - lastMousePosition.y,
                          0};
    curveListManager.translateCurves({translation.x * fixatedAxis.x, translation.y * fixatedAxis.y, 0});
}

void Scene::handleScaleMode() {
    Float3 scale1 = {currentMousePosition.x - selectionCenter.x, currentMousePosition.y - selectionCenter.y, 0};
    Float3 scale2 = {lastMousePosition.x - selectionCenter.x, lastMousePosition.y - selectionCenter.y, 0};

    float scaleAmount = scale1.length() / scale2.length();
    Float3 scale = {fixatedAxis.x != 0 ? scaleAmount : 1, fixatedAxis.y != 0 ? scaleAmount : 1, 0};
    curveListManager.rescaleCurves(scale, selectionCenter);
}

void Scene::handleRotateMode() {
    Float2 vector1 = Float2(currentMousePosition.x - selectionCenter.x,
                            currentMousePosition.y - selectionCenter.y).unit();
    Float2 vector2 = Float2(lastMousePosition.x - selectionCenter.x, lastMousePosition.y - selectionCenter.y).unit();
    float direction = vector1.x * vector2.y - vector1.y * vector2.x;
    direction = direction > 0 ? -1 : 1;
    float dot = vector1.x * vector2.x + vector1.y * vector2.y;
    dot = dot > 1 ? 1 : dot;
    float angle = std::acos(dot);
    curveListManager.rotateCurves(angle * direction, selectionCenter);
}

void Scene::handleDefaultMode(int button, int state) {
    bool isLeftMouseClicked = leftMouseDown(button, state);
    if (!isLeftMouseClicked)
        return;
    if (multipleSelect) {
        multipleSelection(currentMousePosition.x, currentMousePosition.y);
    } else {
        singleSelection(currentMousePosition.x, currentMousePosition.y);
    }
}

bool Scene::pointIntersectsObject(Float3 point) {
    return true;
}
