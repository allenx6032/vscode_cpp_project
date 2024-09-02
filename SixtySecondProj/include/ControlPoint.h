//
// Created by menegais1 on 02/05/2020.
//

#ifndef BEZIERCURVES_CONTROLPOINT_H
#define BEZIERCURVES_CONTROLPOINT_H


#include "CanvasObject.h"

class ControlPoint : public CanvasObject {

public:

    Float3 vertex;
    float radius;
    int idx;
    Float4 handleColor;

    ControlPoint(const Float3 &vertex, float radius);

    void mouse(int button, int state, int wheel, int direction, int x, int y) override;

    void render() override;

    bool pointIntersectsObject(Float3 point) override;

    void translate(Float3 float3) override;

private:
    bool dragging = false;
    Float3 curMousePosition;
    Float3 lastMousePosition;
};


#endif //BEZIERCURVES_CONTROLPOINT_H
