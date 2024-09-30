#pragma once
#include "sdlms_System.h"
#include "sdlms_Video.h"

class VideoSystem : public System
{
public:
    void run(World &world) override;

private:
    void update_video(Video *video); // 视频
};
