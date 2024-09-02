
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GlobalManager.h"
#include "BezierCurve.h"
#include "BezierCurveManager.h"

void BezierCurveManager::deleteSelectedcurves() {
    if (selectedCurves.size() == 0)
        return;
    for (int i = 0; i < selectedCurves.size(); i++) {
        BezierCurve *fig = selectedCurves[i];
        removeCurve(curves, fig);
        GlobalManager::getInstance()->deleteObject(fig);
    }

    selectedCurves.clear();
}

void BezierCurveManager::clearSelectedCurve() {
    for (int i = 0; i < selectedCurves.size(); i++) {
        selectedCurves[i]->isSelected = false;
    }
    selectedCurves.clear();
}

void BezierCurveManager::addCurve(BezierCurve *curve) {
    addCurve(curves, curve);
}

Float3 BezierCurveManager::calculateSelectedCurvesCenter() {
    Float3 mean = {0, 0, 0};
    int size = selectedCurves.size();
    for (int i = 0; i < size; i++) {
        mean.x += selectedCurves[i]->getCenter().x;
        mean.y += selectedCurves[i]->getCenter().y;
        mean.z += selectedCurves[i]->getCenter().z;
    }

    return {mean.x / size, mean.y / size, mean.z / size};
}

void BezierCurveManager::setSelectedCurvesRender(curveRender curveRender) {


}

void BezierCurveManager::updateSelectedCurvesZIndex(int zIndexModifier) {
    for (int i = 0; i < selectedCurves.size(); i++) {
        selectedCurves[i]->setZIndex(selectedCurves[i]->getZIndex() + zIndexModifier);
        updatecurveZIndex(selectedCurves[i]);
    }
}

void BezierCurveManager::updatecurveZIndex(BezierCurve *curve) {
    if (curve->isSelected) {
        removeCurve(selectedCurves, curve);
        addCurve(selectedCurves, curve);
    }

    removeCurve(curves, curve);
    addCurve(curves, curve);
}

void BezierCurveManager::selectCurve(BezierCurve *curve) {
    curve->isSelected = true;
    addCurve(selectedCurves, curve);
}

void BezierCurveManager::deselectCurve(BezierCurve *curve) {
    curve->isSelected = false;
    removeCurve(selectedCurves, curve);
}

BezierCurve *BezierCurveManager::getFirstInteractedCurve(Float2 mousePosition) {
    int size = curves.size();
    for (int i = 0; i < size; i++) {
        BezierCurve *fig = curves[i];
        if (fig->isMouseInsideObject()) {
            return fig;
        }
        for (int j = 0; j < fig->_controlPoints.size(); ++j) {
            if (fig->_controlPoints[j]->isMouseInsideObject()) return fig;
        }
    }

    return nullptr;
}

void BezierCurveManager::addCurve(std::vector<BezierCurve *> &curves, BezierCurve *curve) {
    int size = curves.size();
    if (size == 0) {
        curves.push_back(curve);
    } else {
        for (int i = 0; i < size; i++) {
            if (curves[i]->getZIndex() <= curve->getZIndex()) {
                curves.insert(curves.begin() + i, curve);
                break;
            }
        }
        if (size == curves.size())
            curves.push_back(curve);
    }
}

bool BezierCurveManager::removeCurve(std::vector<BezierCurve *> &curves, BezierCurve *curve) {
    auto iterator = std::find(curves.begin(), curves.end(), curve);
    if (iterator != curves.cend()) {
        curves.erase(iterator);
        return true;
    }
    return false;
}

void BezierCurveManager::translateCurves(Float3 translationAmount) {

    for (int i = 0; i < selectedCurves.size(); i++) {
        selectedCurves[i]->translate({translationAmount.x, translationAmount.y, translationAmount.z});
    }
}

void BezierCurveManager::rotateCurves(float angle, Float3 center) {
    for (int i = 0; i < selectedCurves.size(); i++) {
        selectedCurves[i]->rotate(angle, center);
    }
}

void BezierCurveManager::rescaleCurves(Float3 scale, Float3 center) {
    for (int i = 0; i < selectedCurves.size(); i++) {
        selectedCurves[i]->rescale({scale.x, scale.y, scale.z}, center);
    }
}

void BezierCurveManager::setSelectedCurvesColor(Float3 color, bool fillColor) {
    for (int i = 0; i < selectedCurves.size(); i++) {
        selectedCurves[i]->lineColor = color;
    }
}

bool BezierCurveManager::isCurvesSelected() {
    return !selectedCurves.empty();
}

void BezierCurveManager::setRenderZIndex(bool drawZIndex) {
    for (int i = 0; i < curves.size(); i++) {
        curves[i]->drawZIndex = drawZIndex;
    }
}

void BezierCurveManager::setDrawBounds(bool drawBounds, bool selectedOnly) {
    if (selectedOnly) {
        for (int i = 0; i < selectedCurves.size(); i++) {
            selectedCurves[i]->drawConvexHull = drawBounds;

        }
    } else {
        for (int i = 0; i < curves.size(); i++) {
            curves[i]->drawConvexHull = drawBounds;
        }
    }
}

void BezierCurveManager::setDrawControlPoints(bool drawControlPoints, bool selectedOnly) {
    if (selectedOnly) {
        for (int i = 0; i < selectedCurves.size(); i++) {
            for (int j = 0; j < selectedCurves[i]->_controlPoints.size(); ++j) {
                selectedCurves[i]->_controlPoints[j]->setActive(drawControlPoints);
            }
        }
    } else {
        for (int i = 0; i < curves.size(); i++) {
            for (int j = 0; j < curves[i]->_controlPoints.size(); ++j) {
                curves[i]->_controlPoints[j]->setActive(drawControlPoints);
            }
        }
    }
}

void BezierCurveManager::setDrawControlGraph(bool drawControlGraph, bool selectedOnly) {
    if (selectedOnly) {
        for (int i = 0; i < selectedCurves.size(); i++) {
            selectedCurves[i]->drawControlGraph = drawControlGraph;

        }
    } else {
        for (int i = 0; i < curves.size(); i++) {
            curves[i]->drawControlGraph = drawControlGraph;
        }
    }
}

void BezierCurveManager::setDrawAnimation(bool drawAnimation, bool selectedOnly) {
    if (selectedOnly) {
        for (int i = 0; i < selectedCurves.size(); i++) {
            selectedCurves[i]->drawAnimation = drawAnimation;

        }
    } else {
        for (int i = 0; i < curves.size(); i++) {
            curves[i]->drawAnimation = drawAnimation;
        }
    }
}

void BezierCurveManager::activateBlendingFunctionGraph() {
    if (selectedCurves.empty()) return;
    auto selectedCurve = selectedCurves[0];
    selectedCurve->blendingFunctionsGraph->setActive(!selectedCurve->blendingFunctionsGraph->getActive());
    clearSelectedCurve();
    selectCurve(selectedCurve);
}
