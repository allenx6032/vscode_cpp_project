//
// Created by menegais1 on 02/05/2020.
//

#include "ConvexHull.h"


//Return 1 if v2 is left from v1 and -1 if v2 is right from v1
int ConvexHull::angleOrientation(Float3 v1, Float3 v2) {
    return (v1.x * v2.y - v1.y * v2.x) > 0 ? 1 : -1;
}

int ConvexHull::getLeftMostPoint(std::vector<Float3> points) {
    int leftMostIndex = 0;
    Float3 point = points[0];
    for (int i = 0; i < points.size(); i++) {
        if (points[i].x < point.x) {
            leftMostIndex = i;
            point = points[i];
        }
    }
    return leftMostIndex;
}

int ConvexHull::comparePoints(Float3 v1, Float3 v2) {
    return v1.x == v2.x && v1.y == v2.y;
}


void ConvexHull::calculateConvexHull(std::vector<Float3> points) {
    hull.clear();
    int cont = 0;
    int currentLeftMost = getLeftMostPoint(points);
    hull.push_back(points[currentLeftMost]);
    cont++;
    do {
        currentLeftMost = 0;
        if (comparePoints(hull[cont - 1], points[currentLeftMost])) currentLeftMost = 1;
        Float3 v1 = points[currentLeftMost] - hull[cont - 1];
        for (int i = 1; i < points.size(); i++) {
            Float3 v2 = points[i] - hull[cont - 1];
            if (comparePoints(v1, v2)) continue;
            //  printVector(v1);
            // printf("\ncont:%d \t %d",cont,angleOrientation(v1, v2));
            if (angleOrientation(v1, v2) > 0) {
                currentLeftMost = i;
                v1 = points[currentLeftMost] - hull[cont - 1];
            }
        }
        hull.push_back(points[currentLeftMost]);
        cont++;
        if (cont >= points.size())
            break;
    } while (!comparePoints(hull[cont - 1], hull[0]));
    hull.push_back(hull[0]);
}
