#include "Motor.h"

Vector3 inicio = Vector3(0.0f, 0.0f, -1000.0f); //evitar origem

void Motor::build(){
    Forma f0 = Forma(dim);
    f0.cilindro(r, 100); //manivela
    Forma f1 = Forma(dim);
    f1.disco(r*2+10); //conector da manivela 1
    Forma f2 = Forma(dim);
    f2.cilindro(r*2, r*2); //pistao 1
    Forma f3 = Forma(dim);
    f3.disco(r*2+10); //conector da manivela 2
    Forma f4 = Forma(dim);
    f4.cilindro(r*2, r*2); //pistao 2
    Forma f5 = Forma(dim); //biela 1
    f5.cilindro(r/4, sqrt(pow((oposto/tan(PI/4.0f)), 2) + pow(oposto, 2))); //parafuso 1
    Forma f6 = Forma(dim); //biela 2
    f6.cilindro(r/4, sqrt(pow((oposto/tan(PI/4.0f)), 2) + pow(oposto, 2))); //parafuso 2
    Forma f7 = Forma(dim); //camisa 1
    f7.cilindro(r*2, r*6);
    Forma f8 = Forma(dim); //camisa 1
    f8.cilindro(r*2, r*6); //camisa 2

    _3d.push_back(f0);
    _3d.push_back(f1);
    _3d.push_back(f2);
    _3d.push_back(f3);
    _3d.push_back(f4);
    _3d.push_back(f5);
    _3d.push_back(f6);
    _3d.push_back(f7);
    _3d.push_back(f8);
}

Motor::Motor(int rpm, int _w, int _h){
    rot = 0.0f;
    r = 30.0f;
    w = _w;
    h = _h;
    RPM = rpm;
    ang = 0.0f;
    ligado = false;
    build();
}

void Motor::chave(){
    ligado = !ligado;
}



void Motor::_2dRender(){
    float dd = _3d[0].m.DIM;
    for(int i = 0; i < _3d.size(); i++){
        for(int j = 0; j < dd; j++){
            delete[] _3d[i].m.desc[j];
            delete[] _3d[i].m.proj[j];
        }
        delete[] _3d[i].m.desc;
        delete[] _3d[i].m.proj;
    }

    _3d.clear();
    build();


    Matriz t1[] = {
        Matriz().rot(0.0f, PI/2.0f, ang),
        Matriz().transl(inicio.x, inicio.y, inicio.z),
        Matriz().esc(1.0f, 1.0f, 0.0f)
    };
    _3d[1].m.transform(t1, 2);


    Matriz t2[] = {
        Matriz().rot(PI/4.0f, 0.0f, 0.0f),
        Matriz().esc(1.0f, 1.0f, 0.0f),
        Matriz().transl((oposto/tan(PI/4.0f))+inicio.x+r*2*-(sin(ang)+cos(ang))/2.0f, oposto + inicio.y+r*2*-(sin(ang)+cos(ang))/2.0f, 0.0f),
    };
    _3d[2].m.transform(t2, 3);

    Matriz t4[] = {
        Matriz().rot(-PI/4.0f, 0.0f, 0.0f),
        Matriz().esc(1.0f, 1.0f, 0.0f),
        Matriz().transl(-40.0f-(oposto/tan(PI/4.0f))+inicio.x+r*2*-(sin(ang)+cos(ang))/2.0f, +40.0f + oposto + inicio.y+r*2*(sin(ang)+cos(ang))/2.0f, 0.0f),
    };
    _3d[4].m.transform(t4, 3);


    float _x = inicio.x+r*2*+cos(ang) - (oposto/tan(PI/4.0f))+inicio.x+r*2*+(sin(ang)+cos(ang))/2.0f; //distancia em x do pistao ao parafuso
    float _y = inicio.y+r*2*+sin(ang) - (oposto + inicio.y+r*2*-(sin(ang)+cos(ang))/2.0f); //distancia em y do pistao ao parafuso
    float angle = atan(_y/_x);

    Matriz t5[] = {
        Matriz().rot(PI, 0.0f, 0.0f),
        Matriz().rot(angle, 0.0f, 0.0f),
        Matriz().transl((oposto/tan(PI/4.0f))+inicio.x+r*2*-(sin(ang)+cos(ang))/2.0f, oposto + inicio.y+r*2*-(sin(ang)+cos(ang))/2.0f, 0.0f),
        Matriz().esc(1.0f, 1.0f, 0.0f)
    };
    _3d[5].m.transform(t5, 4);

    Matriz t6[] = {
        Matriz().rot(0.0f, 0.0f, 0.0f),
        Matriz().rot(-angle, 0.0f, 0.0f),
        Matriz().transl(-(oposto/tan(PI/4.0f))+inicio.x+r*2*-(sin(ang)+cos(ang))/2.0f, oposto + inicio.y+r*2*(sin(ang)+cos(ang))/2.0f, 0.0f),
        Matriz().esc(1.0f, 1.0f, 0.0f)

    };
    _3d[6].m.transform(t6, 4);

    Matriz t7[] = {
        Matriz().rot(PI/4.0f, 0.0f, 0.0f),
        Matriz().transl((oposto/tan(PI/4.0f))+inicio.x-r-15.0f, oposto + inicio.y-r-15.0f, 0.0f),
    };
    _3d[7].m.transform(t7, 2);

    Matriz t8[] = {
        Matriz().rot(-PI/4.0f, 0.0f, 0.0f),
        Matriz().transl(-40.0f-(oposto/tan(PI/4.0f))+inicio.x+r-15.0f-r*2, +40.0f + oposto + inicio.y-15.0f+r*2, 0.0f),
    };
    _3d[8].m.transform(t8, 2);

    if(exibir[2]){
        _3d[2].render();
    }
    if(exibir[3]){
        _3d[3].render();
    }
    if(exibir[0]){
        _3d[0].render();
    }
    if(exibir[6]){
        _3d[6].render();
    }
    if(exibir[4]){
        _3d[4].render();
    }
    if(exibir[5]){
        _3d[5].render();
    }
    if(exibir[1]){
        _3d[1].render();
    }
    if(exibir[7]){
        _3d[7].render();
    }
    if(exibir[8]){
        _3d[8].render();
    }
}


void Motor::_3dRender(){
    float dd = _3d[0].m.DIM;
    if(dd != this->dim){
        for(int i = 0; i < _3d.size(); i++){
            for(int j = 0; j < dd; j++){
                delete[] _3d[i].m.desc[j];
                delete[] _3d[i].m.proj[j];
            }
        delete[] _3d[i].m.desc;
        delete[] _3d[i].m.proj;
        }

        _3d.clear();
        build();
    }
    Matriz look = Matriz().lookat(Vector3(500.0f*cos(rot), 0.0f, -800.0f+inicio.z+500*sin(rot)), Vector3(inicio.x, inicio.y + 100.0f, inicio.z), Vector3(0, 1, 0));

    Matriz t0[] = {
        Matriz().rot(0.0f, PI/2.0f, ang),
        Matriz().transl(inicio.x, inicio.y, inicio.z),
        look,
        Matriz().persp(local),
    };
    _3d[0].m.transform(t0, 4-ort);

    Matriz t1[] = {
        Matriz().rot(0.0f, PI/2.0f, ang),
        Matriz().transl(inicio.x, inicio.y, inicio.z),
        look,
        Matriz().persp(local),
    };
    _3d[1].m.transform(t1, 4-ort);


    Matriz t2[] = {
        Matriz().rot(PI/4.0f, 0.0f, 0.0f),
        Matriz().transl((oposto/tan(PI/4.0f))+inicio.x+r*2*-(sin(ang)+cos(ang))/2.0f, oposto + inicio.y+r*2*-(sin(ang)+cos(ang))/2.0f, inicio.z),
        look,
        Matriz().persp(local),
    };
    _3d[2].m.transform(t2, 4-ort);

    Matriz t3[] = {
        Matriz().rot(0.0f, PI/2.0f, ang),
        Matriz().transl(inicio.x, inicio.y, inicio.z-100.0f),
        look,
        Matriz().persp(local),
    };
    _3d[3].m.transform(t3, 4-ort);

    Matriz t4[] = {
        Matriz().rot(-PI/4.0f, 0.0f, 0.0f),
        Matriz().transl(-40.0f-(oposto/tan(PI/4.0f))+inicio.x+r*2*-(sin(ang)+cos(ang))/2.0f, +40.0f + oposto + inicio.y+r*2*(sin(ang)+cos(ang))/2.0f, inicio.z-100.0f),
        look,
        Matriz().persp(local),
    };
    _3d[4].m.transform(t4, 4-ort);


    float _x = inicio.x+r*2*+cos(ang) - (oposto/tan(PI/4.0f))+inicio.x+r*2*+(sin(ang)+cos(ang))/2.0f; //distancia em x do pistao ao parafuso
    float _y = inicio.y+r*2*+sin(ang) - (oposto + inicio.y+r*2*-(sin(ang)+cos(ang))/2.0f); //distancia em y do pistao ao parafuso
    float angle = atan(_y/_x);

    Matriz t5[] = {
        Matriz().rot(PI, 0.0f, 0.0f),
        Matriz().rot(angle, 0.0f, 0.0f),
        Matriz().transl((oposto/tan(PI/4.0f))+inicio.x+r*2*-(sin(ang)+cos(ang))/2.0f, oposto + inicio.y+r*2*-(sin(ang)+cos(ang))/2.0f, inicio.z),
        look,
        Matriz().persp(local),
    };
    _3d[5].m.transform(t5, 5-ort);

    Matriz t6[] = {
        Matriz().rot(0.0f, 0.0f, 0.0f),
        Matriz().rot(-angle, 0.0f, 0.0f),
        Matriz().transl(-(oposto/tan(PI/4.0f))+inicio.x+r*2*-(sin(ang)+cos(ang))/2.0f, oposto + inicio.y+r*2*(sin(ang)+cos(ang))/2.0f, inicio.z-100.0f),
        look,
        Matriz().persp(local),
    };
    _3d[6].m.transform(t6, 5-ort);

    Matriz t7[] = {
        Matriz().rot(PI/4.0f, 0.0f, 0.0f),
        Matriz().transl((oposto/tan(PI/4.0f))+inicio.x-r-15.0f, oposto + inicio.y-r-15.0f, inicio.z),
        look,
        Matriz().persp(local),
    };
    _3d[7].m.transform(t7, 4-ort);

    Matriz t8[] = {
        Matriz().rot(-PI/4.0f, 0.0f, 0.0f),
        Matriz().transl(-40.0f-(oposto/tan(PI/4.0f))+inicio.x+r-15.0f-r*2, +40.0f + oposto + inicio.y-15.0f+r*2, inicio.z-100.0f),
        look,
        Matriz().persp(local),
    };
    _3d[8].m.transform(t8, 4-ort);

    if(exibir[2]){
        _3d[2].render();
    }
    
    if(exibir[3]){
        _3d[3].render();
    }
    if(exibir[0]){
        _3d[0].render();
    }
    if(exibir[4]){
        _3d[4].render();
    }
    if(exibir[6]){
        _3d[6].render();
    }
    if(exibir[5]){
        _3d[5].render();
    }
    if(exibir[1]){
        _3d[1].render();
    }
    if(exibir[7]){
        _3d[7].render();
    }
    if(exibir[8]){
        _3d[8].render();
    }
}

