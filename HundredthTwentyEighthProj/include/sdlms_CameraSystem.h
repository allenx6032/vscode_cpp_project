#pragma once
#include "sdlms_System.h"
#include "sdlms_Transform.h"
#include "sdlms_Camera.h"

class CameraSystem : public System
{
public:
	void run(World &world) override;

private:
	void update_camera(Camera *cam, Transform *tr, World &world);
};
