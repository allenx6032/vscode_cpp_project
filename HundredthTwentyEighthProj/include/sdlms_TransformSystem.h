#pragma once
#include "sdlms_System.h"
#include "sdlms_Transform.h"
#include "sdlms_RelativeTransform.h"
#include "sdlms_LimitTransform.h"

class TransformSystem : public System
{
public:
    void run(World &world) override;

private:
    void update_relative_tr(int index, RelativeTransform *rtr,World &world);
    void update_limit_tr(LimitTransform *ltr,World &world);
};
