#include "FreeCamera.h"
Camera FreeCam;

void SetupCamera()
{
    FreeCam.FOV = 60;
    FreeCam.ScreenFar = 1000;
    FreeCam.ScreenNear = 0.1f;
    FreeCam.transform.position = Vec3(0, 2, -5);
    FreeCam.transform.rotation = Vec3(0, 0, 0);
    FreeCam.speed = 0.1f;
}

void UpdateCamera()
{
    if (Input.Mouse.cursorLocked)
    {
        float mouseSens = 0.05f;
        FreeCam.transform.rotation.x += Input.Mouse.Delta.y * mouseSens;
        FreeCam.transform.rotation.y += Input.Mouse.Delta.x * mouseSens;
        vec4 Ivec = Vec4(0, 0, 0, 1);

        FreeCam.speed = Clamp(FreeCam.speed + Input.Mouse.ScrollDelta.y * .01f, .05f, 0.2f);
        float s = FreeCam.speed;
        if (glfwGetKey(wnd, GLFW_KEY_LEFT_SHIFT))
            s *= 2;        
        if (glfwGetKey(wnd, GLFW_KEY_W))
            Ivec.z += s;
        if (glfwGetKey(wnd, GLFW_KEY_S))
            Ivec.z -= s;
        if (glfwGetKey(wnd, GLFW_KEY_D))
            Ivec.x += s;
        if (glfwGetKey(wnd, GLFW_KEY_A))
            Ivec.x -= s;
        if (glfwGetKey(wnd, GLFW_KEY_E) || glfwGetKey(wnd,GLFW_KEY_SPACE))
            Ivec.y += s;
        if (glfwGetKey(wnd, GLFW_KEY_Q)|| glfwGetKey(wnd,GLFW_KEY_LEFT_CONTROL))
            Ivec.y -= s;
        Ivec = MultMatVec(Ivec, RotMatrix(FreeCam.transform.rotation));
        FreeCam.transform.position.x += Ivec.x;
        FreeCam.transform.position.y += Ivec.y;
        FreeCam.transform.position.z += Ivec.z;
    }
}