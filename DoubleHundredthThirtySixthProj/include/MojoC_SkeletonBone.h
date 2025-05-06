/*
 * Copyright (c) scott.cgi All Rights Reserved.
 *
 * This source code belongs to project Mojoc, which is a pure C Game Engine hosted on GitHub.
 * The Mojoc Game Engine is licensed under the MIT License, and will continue to be iterated with coding passion.
 *
 * License  : https://github.com/scottcgi/Mojoc/blob/master/LICENSE
 * GitHub   : https://github.com/scottcgi/Mojoc
 * CodeStyle: https://github.com/scottcgi/Mojoc/blob/master/Docs/CodeStyle.md
 *
 * Since    : 2013-7-2
 * Update   : 2019-2-13
 * Author   : scott.cgi
 */


#ifndef SKELETON_BONE_H
#define SKELETON_BONE_H


#include "MojoC_SkeletonData.h"
#include "MojoC_Drawable.h"


/**
 * The skeleton bone can transform and draw by drawable.
 */
typedef struct
{
    Drawable          drawable[1];
    SkeletonBoneData* boneData;
}
SkeletonBone;


/**
 * Control SkeletonBone.
 */
struct ASkeletonBone
{
    SkeletonBone* (*Create)        (SkeletonBoneData* boneData);
    void          (*Init)          (SkeletonBoneData* boneData, SkeletonBone* outBone);

    /**
     * Set bone to setup pose.
     */
    void          (*SetToSetupPose)(SkeletonBone* bone);
};


extern struct ASkeletonBone ASkeletonBone[1];


#endif
