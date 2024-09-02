#ifndef __MATRIZ_H__
#define __MATRIZ_H__

#include "Vector3.h"
#include "gl_canvas2d.h"

class Matriz{
public:
    float desc[4][4] = {
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f}
    };

    Vector3 apply(const Vector3& point, const Matriz& matriz) {
        float homogeneousPoint[4] = { point.x, point.y, point.z, 1.0f };
        float transformedPoint[4] = { 0.0f };

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                transformedPoint[i] += matriz.desc[i][j] * homogeneousPoint[j];
            }
        }

        // Conversão do vetor resultante de volta para coordenadas 3D
        Vector3 resultPoint;
        if(transformedPoint[3] != 0){
            resultPoint.x = transformedPoint[0] / transformedPoint[3];
            resultPoint.y = transformedPoint[1] / transformedPoint[3];
            resultPoint.z = transformedPoint[2] / transformedPoint[3];
        }


        return resultPoint;
    }

    Matriz operator*(const Matriz& m) {
        Matriz result;
        for (int i = 0; i < 4; i++) { // linha
            for (int j = 0; j < 4; j++) { // coluna
                for (int k = 0; k < 4; k++) { // elemento
                    result.desc[i][j] += desc[i][k] * m.desc[k][j];
                }
            }
        }
        return result;
    }
    Matriz rot(float y, float b, float a){
        Matriz m = Matriz();
        m.desc[0][0] = cos(b)*cos(y);
        m.desc[0][1] = sin(a)*sin(b)*cos(y) - cos(a)*sin(y);
        m.desc[0][2] = cos(a)*sin(b)*cos(y) + sin(a)*sin(y);

        m.desc[1][0] = cos(b)*sin(y);
        m.desc[1][1] = sin(a)*sin(b)*sin(y) + cos(a)*cos(y);
        m.desc[1][2] = cos(a)*sin(b)*sin(y) - sin(a)*cos(y);

        m.desc[2][0] = -sin(b);
        m.desc[2][1] = sin(a)*cos(b);
        m.desc[2][2] = cos(a)*cos(b);

        m.desc[3][3] = 1.0f;
        return m;
    }

    Matriz esc(float sx, float sy, float sz){
        Matriz m = Matriz();
        m.desc[0][0] = sx;

        m.desc[1][1] = sy;

        m.desc[2][2] = sz;

        m.desc[3][3] = 1.0f;
        return m;
    }

    Matriz transl(float tx, float ty, float tz){
        Matriz m = Matriz();
        m.desc[0][0] = 1.0f;

        m.desc[0][3] = tx;

        m.desc[1][1] = 1.0f;

        m.desc[1][3] = ty;

        m.desc[2][2] = 1.0f;
        m.desc[2][3] = tz;

        m.desc[3][3] = 1.0f;
        return m;
    }

    Matriz persp(float d){
        Matriz m = Matriz();

        m.desc[0][0] = 1.0f;
        m.desc[1][1] = 1.0f;
        m.desc[2][2] = 1.0f;
        m.desc[3][2] = 1.0f/d;
        return m;
    }


    Matriz lookat(Vector3 eye, Vector3 center, Vector3 up){
        Matriz m = Matriz(); //matriz - eye
        m.desc[0][0] = 1.0f;
        m.desc[1][1] = 1.0f;
        m.desc[2][2] = 1.0f;
        m.desc[3][3] = 1.0f;
        
        m = m.transl(-eye.x, -eye.y, -eye.z);
        Vector3 normal = (eye - center).normalize();
        Vector3 right = up.cross(normal).normalize();
        Vector3 newup = normal.cross(right).normalize();

        Matriz look = Matriz(); //matriz lookat
        look.desc[0][0] = right.x;
        look.desc[0][1] = right.y;
        look.desc[0][2] = right.z;

        look.desc[1][0] = newup.x;
        look.desc[1][1] = newup.y;
        look.desc[1][2] = newup.z;

        look.desc[2][0] = normal.x;
        look.desc[2][1] = normal.y;
        look.desc[2][2] = normal.z;

        look.desc[3][3] = 1.0f;

        return look * m;
    }
};




class Coord{
public:
    int DIM;
    Vector3 **desc;
    Vector3 **proj;
    void init(int d){
        DIM = d;
        desc = new Vector3*[DIM+1];
        proj = new Vector3*[DIM+1];
        for (int i = 0; i <= DIM; i++) {
            desc[i] = new Vector3[DIM+1];
            proj[i] = new Vector3[DIM+1];
        }
    }

    void transform(Matriz t[], int n){ //recebe uma concatenacao de matrizes
        Vector3 p;
        for(int i = 0; i <= this->DIM; i++){
            for(int j = 0; j <= this->DIM; j++){
                p = desc[i][j];
                for(int k = 0; k < n; k++){
                    p = Matriz().apply(p, t[k]);
                }
                proj[i][j] = p;
            }
        }
    }
};






#endif
