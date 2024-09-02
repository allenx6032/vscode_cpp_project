#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Manager.h"



int screenWidth = 1000, screenHeight = 800;
int mouseX, mouseY;

Manager *manager = nullptr;

void render(){
   CV::translate(screenWidth/2, screenHeight/2);
   manager->render();
}


void keyboard(int key){
   manager->onKey(key);
}


void keyboardUp(int key){

}


void mouse(int button, int state, int wheel, int direction, int x, int y){
   mouseX = x;
   mouseY = y;
}

int main(void){
   
   manager = new Manager(screenWidth, screenHeight);
   CV::init(&screenWidth, &screenHeight, "Motor");
   CV::clear(0, 0, 0);
   CV::run();;
}
