#define _USE_MATH_DEFINES
#include "war_camera.h"
#include "war_window.h"
#include "war_world.h"
#include <cmath>
#include <iostream>

Transform camera;

bool cursor = true;

bool moveForward = false, moveBack = false, moveLeft = false, moveRight = false, moveUp = false, moveDown = false;
int prev_mouse_x = 0.0, prev_mouse_y = 0.0;

glm::mat4 model, view, projection;
float camRotSpeed = 0.01;
float camMoveSpeed = 0.05;

void MoveCamera()
{
    if (!cursor){
        sf::Vector2i delta = (sf::Mouse::getPosition(window) - window_center);
        camera.rotation.y -= float(delta.x) * camRotSpeed;
        camera.rotation.x -= float(delta.y) * camRotSpeed;
        //if (camera.rotation.x > -M_PI / 4)
        //    camera.rotation.x = -M_PI / 4;
        if (camera.rotation.x > M_PI / 2)
            camera.rotation.x = M_PI / 2;
        if (camera.rotation.x < -M_PI / 2)
            camera.rotation.x = -M_PI / 2;
        sf::Mouse::setPosition(window_center, window);
    }

    float camera_dx = 0., camera_dy = 0., camera_dz = 0.;

    if (moveForward) 
    {
        camera_dx += -sin(camera.rotation.y) * camMoveSpeed;
        camera_dz += -cos(camera.rotation.y) * camMoveSpeed;
        camera.position.x += -sin(camera.rotation.y) * camMoveSpeed;
        camera.position.z += -cos(camera.rotation.y) * camMoveSpeed;
    }
    if (moveBack) 
    {
        camera_dx += sin(camera.rotation.y) * camMoveSpeed;
        camera_dz += cos(camera.rotation.y) * camMoveSpeed;
        camera.position.x += sin(camera.rotation.y) * camMoveSpeed;
        camera.position.z += cos(camera.rotation.y) * camMoveSpeed;
    }
    if (moveLeft)
    {
        camera_dx += -cos(camera.rotation.y) * camMoveSpeed;
        camera_dz += sin(camera.rotation.y) * camMoveSpeed;
        camera.position.x += -cos(camera.rotation.y) * camMoveSpeed;
        camera.position.z += sin(camera.rotation.y) * camMoveSpeed;
    }
    if (moveRight)
    {
        camera_dx += cos(camera.rotation.y) * camMoveSpeed;
        camera_dz += -sin(camera.rotation.y) * camMoveSpeed;
        camera.position.x += cos(camera.rotation.y) * camMoveSpeed;
        camera.position.z += -sin(camera.rotation.y) * camMoveSpeed;
    }
    if (moveUp) 
    {
        camera_dy += camMoveSpeed;
        camera.position.y += camMoveSpeed;
    }
    if (moveDown) 
    {
        camera_dy += -camMoveSpeed;
        camera.position.y -= camMoveSpeed;
    }
    float cameraTileHeight = world[coordsToKey(camera.position.x, camera.position.z)].GetHeight(camera.position.x, camera.position.z);
    float hDiff = cameraTileHeight + 2.0 - camera.position.y;
    if (hDiff > 0)
    {
        camera.position.y += 0.09 * hDiff;
    }
    view = glm::mat4(1.0);
    //view = glm::translate(view, glm::vec3(0, 0, -10));
    view = glm::rotate(view, camera.rotation.x, glm::vec3(-1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, camera.rotation.y, glm::vec3(0.0f, -1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(-camera.position.x, -camera.position.y, -camera.position.z));
}
