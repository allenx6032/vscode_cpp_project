//
// Created by menegais1 on 02/05/2020.
//

#ifndef BEZIERCURVES_CONVEXHULL_H
#define BEZIERCURVES_CONVEXHULL_H


#include "Float3.h"
#include <vector>

class ConvexHull {
public:
    std::vector<Float3> hull;

    void calculateConvexHull(std::vector<Float3> points);

private:
    int getLeftMostPoint(std::vector<Float3> points);

    int angleOrientation(Float3 v1, Float3 v2);

    int comparePoints(Float3 v1, Float3 v2);
};


#endif //BEZIERCURVES_CONVEXHULL_H
