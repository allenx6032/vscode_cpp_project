#ifndef GAME_ANIMATION_KEY_FRAME_H
#define GAME_ANIMATION_KEY_FRAME_H

#include "CustomFighter_hit.h"
#include "CustomFighter_math.h"
#include "CustomFighter_skeleton.h"
#include "CustomFighter_move.h"

#include <vector>
#include <string>
#include <filesystem>

struct Frame {
    int key             = 0;
    int duration        = 1;
    Skeleton pose       = Skeleton();
    Vector2 impulse     = Vector2(0, 0);
    bool cancel         = false;
    bool stop           = false;
    int jump            = -1;
    std::string sound   = "";

    bool isGrab         = false;
    Skeleton grabee     = Skeleton();
    int grabBreak       = GrabBreak::Unbreakable;


    std::vector<HitBox> hitBoxes;
    std::vector<HurtBox> hurtBoxes;
};

struct Animation {
    Animation();

    // What the animation is
    std::string name                = "";
    int category                    = MoveCategory::Normal;

    // How you can get into the animation
    bool from                       [MoveCategory::Total];
    std::string customFrom          = "";

    std::vector<Frame> keyFrames;

    bool loadFromFile(std::filesystem::path path);
    void saveToFile(std::filesystem::path path);
    bool importFromFreeMoCap(std::filesystem::path path);

    void swapKeyFrame(int a, int b);

    void insertKeyFrame(int index, Frame k);
    void removeKeyFrame(int index);
    
    Frame& getKeyFrame(int index);
    int getKeyFrameCount();

    Frame getFrame(int frame);
    int getFrameCount();
    int getFrameFromKeyFrame(int index);

    int getStartup();
    int getDamage();
    int getOnHit();
    int getOnBlock();
};

#endif