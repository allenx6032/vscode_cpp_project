#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "3d.h"
#include <vector>



class Motor{

protected:
    float oposto = 250.0f;
    float local = -800.0f;
    int x, y;
    int w, h;
    bool ligado;
    std::vector <Forma> _3d;
    void build();
    float r;
public:
    int dim = 20;
    bool ort = false;
    float ang;
    int RPM;
    float rot;
    bool exibir[9] = {true, true, true, true, true, true, true, true, true};
    Motor(int rpm, int _w, int _h);
    void chave();
    void _2dRender();
    void _3dRender();
};








#endif