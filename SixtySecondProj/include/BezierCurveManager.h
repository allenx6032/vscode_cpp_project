//Class responsible for handling the created curves
//Serializing and deserializing them, dealing with selected curves, translation
//rotation and scaling, changing color, adding and deleting curves, and
//dealing with the zIndex ordered curve list

#ifndef curveLISTMANAGER_H
#define curveLISTMANAGER_H

#include <vector>
#include <string>
#include <fstream>
#include "BezierCurve.h"

class BezierCurveManager {
public:
    void deleteSelectedcurves();

    Float3 calculateSelectedCurvesCenter();

    void addCurve(BezierCurve *curve);

    void updateSelectedCurvesZIndex(int zIndexModifier);

    void selectCurve(BezierCurve *curve);

    void deselectCurve(BezierCurve *curve);

    void clearSelectedCurve();

    void setDrawBounds(bool drawBounds, bool selectedOnly);

    void setDrawControlPoints(bool drawControlPoints, bool selectedOnly);

    void setDrawControlGraph(bool drawControlGraph, bool selectedOnly);

    void setDrawAnimation(bool drawAnimation, bool selectedOnly);

    void activateBlendingFunctionGraph();

    void translateCurves(Float3 translationAmount);

    void rotateCurves(float angle, Float3 center);

    void rescaleCurves(Float3 scale, Float3 center);

    void setSelectedCurvesColor(Float3 color, bool fillColor);

    void setSelectedCurvesRender(curveRender curveRender);

    BezierCurve *getFirstInteractedCurve(Float2 mousePosition);

    bool isCurvesSelected();

    void setRenderZIndex(bool b);

private:
    std::vector<BezierCurve *> curves;
    std::vector<BezierCurve *> selectedCurves;

    void updatecurveZIndex(BezierCurve *curve);

    void addCurve(std::vector<BezierCurve *> &curves, BezierCurve *curve);

    bool removeCurve(std::vector<BezierCurve *> &curves, BezierCurve *curve);

};

#endif
