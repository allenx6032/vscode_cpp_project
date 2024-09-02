
#include <vector>
#include <iostream>
#include <cmath>
#include <sstream>
#include "Float2.h"
#include "Float4.h"
#include "gl_canvas2d.h"
#include "BezierCurve.h"
#include <functional>
#include "Fatorial.h"
#include "GlobalManager.h"


BezierCurve::BezierCurve() {
    drawZIndex = false;
    drawConvexHull = false;
    isSelected = false;
}

BezierCurve::BezierCurve(Float3 lineColor, Float4 highlightColor,
                         std::vector<ControlPoint *> controlPoints) {
    this->lineColor = lineColor;
    this->highlightColor = highlightColor;
    this->_controlPoints = controlPoints;
    calculateConvexHull();
    isSelected = false;
    drawConvexHull = false;
    drawControlGraph = false;
    drawAnimation = false;
    drawZIndex = false;
    computeCentroid();
    int n = controlPoints.size() - 1;
    for (int i = 0; i <= n; ++i) {
        blendingFunctions.push_back(calculateBlendingFunction(n, i));
    }
    blendingFunctionsGraph = new BlendingFunctionsGraph(Float3(centroid.x, centroid.y, 0), Float3(300, 150, 0),
                                                        Float3(0, 0, 0),
                                                        blendingFunctions);
    blendingFunctionsGraph->setZIndex(100);
    blendingFunctionsGraph->setActive(false);
    controlGraphColor = HSVtoRGB(Float3(220, 0.8, 0.8));
    convexHullColor = HSVtoRGB(Float3(70, 0.5, 0.8));
}

float t = 0;

void BezierCurve::render() {
    if (_controlPoints.size() < 0)
        return;

    calculateConvexHull();

    color(controlGraphColor.x,controlGraphColor.y,controlGraphColor.z);
    if (drawControlGraph) {

        for (int i = 0; i < _controlPoints.size() - 1; ++i) {
            ControlPoint *p0 = _controlPoints[i];
            ControlPoint *p1 = _controlPoints[i + 1];

            line(p0->vertex.x, p0->vertex.y, p1->vertex.x, p1->vertex.y);
        }
    }

    if (drawAnimation) {
        curveAnimation(getPoints(), t, 0);
    }

    if (isSelected) {
        for (int i = 0; i < _controlPoints.size(); ++i) {
            _controlPoints[i]->handleColor = highlightColor;
        }
    } else {
        for (int i = 0; i < _controlPoints.size(); ++i) {
            _controlPoints[i]->handleColor = Float4(0, 0, 0, 1);
        }
    }
    Float3 p;
    bool draw = false;
    color(lineColor.x, lineColor.y, lineColor.z, 1);
    for (float t = 0; t <= 1; t += 0.01) {
        Float3 p1;
        for (int i = 0; i < _controlPoints.size(); ++i) {
            p1.x += _controlPoints[i]->vertex.x * blendingFunctions[i](t);
            p1.y += _controlPoints[i]->vertex.y * blendingFunctions[i](t);
        }
        if (draw) {
            if (isSelected)
                color(highlightColor.x, highlightColor.y, highlightColor.z, highlightColor.w);

            line(p.x, p.y, p1.x, p1.y);
        }
        p = p1;
        draw = true;
    }

    color(convexHullColor.x,convexHullColor.y,convexHullColor.z);
    if (drawConvexHull) {
        for (int i = 0; i < convexHull.hull.size() - 1; ++i) {
            Float3 p0 = convexHull.hull[i];
            Float3 p1 = convexHull.hull[i + 1];
            line(p0.x, p0.y, p1.x, p1.y);
        }
    }


    t = t + 0.1 * GlobalManager::getInstance()->deltaTime;
    if (t > 1) t = 0;

}

void BezierCurve::curveAnimation(std::vector<Float3> points, float t, int i) {
    if (points.size() <= 1) return;
    std::vector<Float3> interpolation;
    if (animationColors.size() <= i) {
        animationColors.push_back(
                HSVtoRGB(Float3((rand() % 360), 0.5, 0.8)));
    }
    Float3 c = animationColors[i];
    color(c.x, c.y, c.z);
    for (int i = 0; i < points.size() - 1; ++i) {
        Float3 p0 = points[i];
        Float3 p1 = points[i + 1];
        Float3 p = lerp(points[i], points[i + 1], t);
        interpolation.push_back(p);
        circleFill(p.x, p.y, 4, 20);
        line(p0.x, p0.y, p1.x, p1.y);
    }
    curveAnimation(interpolation, t, i + 1);
}

void BezierCurve::computeCentroid() {
    if (_controlPoints.size() <= 2) {
        centroid = (_controlPoints[0]->vertex + _controlPoints[1]->vertex) / 2.0;
        return;
    }

    float centroidX = 0, centroidY = 0;
    float det = 0, tempDet = 0;
    int j = 0;
    int nVertices = convexHull.hull.size();

    for (unsigned int i = 0; i < convexHull.hull.size(); i++) {
        // closed polygon
        if (i + 1 == nVertices)
            j = 0;
        else
            j = i + 1;

        // compute the determinant
        tempDet = convexHull.hull[i].x * convexHull.hull[j].y -
                  convexHull.hull[j].x * convexHull.hull[i].y;
        det += tempDet;

        centroidX += (convexHull.hull[i].x + convexHull.hull[j].x) * tempDet;
        centroidY += (convexHull.hull[i].y + convexHull.hull[j].y) * tempDet;
    }

    // divide by the total mass of the polygon
    centroidX /= 3 * det;
    centroidY /= 3 * det;

    centroid = {centroidX, centroidY, 0};
}

std::function<float(float)> BezierCurve::calculateBlendingFunction(int n, int i) {
    float binomial = Fatorial::fatorial(n) / (Fatorial::fatorial(i) * Fatorial::fatorial(n - i));
    return [binomial, n, i](float t) -> float {
        return binomial * std::pow(t, i) * std::pow(1 - t, n - i);
    };
}

Float3 BezierCurve::getCenter() {
    computeCentroid();
    return centroid;
}

void BezierCurve::translate(Float3 translationAmount) {
    for (int i = 0; i < _controlPoints.size(); i++) {
        _controlPoints[i]->vertex.x += translationAmount.x;
        _controlPoints[i]->vertex.y += translationAmount.y;
        _controlPoints[i]->vertex.z += translationAmount.z;
    }
    computeCentroid();
}

void BezierCurve::rotate(float angle, Float3 center) {
    translate({-center.x, -center.y, 0});
    for (int i = 0; i < _controlPoints.size(); i++) {
        float x = _controlPoints[i]->vertex.x * cos(angle) - _controlPoints[i]->vertex.y * sin(angle);
        float y = _controlPoints[i]->vertex.x * sin(angle) + _controlPoints[i]->vertex.y * cos(angle);
        _controlPoints[i]->vertex.x = x;
        _controlPoints[i]->vertex.y = y;
    }
    translate({center.x, center.y, 0});
    computeCentroid();
}

void BezierCurve::rescale(Float3 scale, Float3 center) {
    translate({-center.x, -center.y, 0});
    for (int i = 0; i < _controlPoints.size(); i++) {
        float x = _controlPoints[i]->vertex.x * scale.x;
        float y = _controlPoints[i]->vertex.y * scale.y;
        _controlPoints[i]->vertex.x = x;
        _controlPoints[i]->vertex.y = y;
    }
    translate({center.x, center.y, 0});
    computeCentroid();
}

bool BezierCurve::pointIntersectsObject(Float3 point) {
    calculateConvexHull();
    return isPointInsidePolygon({point.x, point.y}, convexHull.hull, convexHull.hull.size() - 1);
}

std::vector<Float3> BezierCurve::getPoints() {
    std::vector<Float3> points;
    for (int i = 0; i < _controlPoints.size(); ++i) {
        points.push_back(_controlPoints[i]->vertex);
    }
    return points;
}

void BezierCurve::calculateConvexHull() {
    convexHull.calculateConvexHull(getPoints());
}

BezierCurve::~BezierCurve() {
    for (int i = 0; i < _controlPoints.size(); ++i) {
        GlobalManager::getInstance()->deleteObject(_controlPoints[i]);
    }
    GlobalManager::getInstance()->deleteObject(blendingFunctionsGraph);
}
