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
 * Since    : 2014-6-11
 * Update   : 2019-1-18
 * Author   : scott.cgi
 */


#include "MojoC_Physics.h"
#include "MojoC_PhysicsWorld.h"


static void Init()
{
    APhysicsWorld->gravity.y = -1.5f;
}


struct APhysics APhysics[1] =
{{
    Init
}};
