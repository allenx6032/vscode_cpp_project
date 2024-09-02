#include "Manager.h"

Manager::Manager(int _w, int _h){
    w = _w;
    h = _h;
    motor = new Motor(100, w, h);
    frames = new Frames();

    PlaySound("Trab4LuisChesani/src/soundtrack.wav", NULL, SND_ASYNC | SND_LOOP);
}

void Manager::onKey(int key){
    if(key == '/'){
        _3d = !_3d;
    }
    if(key == '='){
        motor->dim += 1;
        return;
    }
    if(key == '-'){
        motor->dim -= 1;
        return;
    }
    if(key == 'w'){
        motor->RPM += 50;
        return;
    }
    if(key == 's'){
        motor->RPM -= 50;
        return;
    }
    if(key == ' '){
        motor->ort = !motor->ort;
    }
    if(key >= '0' && key <= '9'){
        motor->exibir[key-'0'] = !motor->exibir[key-'0'];
    }

}

void Manager::render(){
    float fps = frames->getFps();
    char aux[50];
    CV::color(1, 1, 1);

    sprintf(aux, "FPS: %.2f", fps);
    CV::text(-w/2.0f+10, 10-h/2.0f, aux);

    sprintf(aux, "RPM: %d", motor->RPM);
    CV::text(-w/2.0f+10, 25-h/2.0f, aux);

    sprintf(aux, "DIM: %d", motor->dim);
    CV::text(-w/2.0f+10, 40-h/2.0f, aux);

    fps = fps * 60.0f; // frames por minuto
    float step = 2*PI/fps; //divide uma volta em um minuto
    motor->ang += step*motor->RPM;
    motor->rot += step*5.0f;
    if(!_3d){
        motor->_2dRender();
    }
    else{
        motor->_3dRender();
    }
}

