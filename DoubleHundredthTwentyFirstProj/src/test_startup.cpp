// Copyright 2016 KeNan Liu
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "test_startup.h"
#include <stdio.h>

void Rapid2D_startup(float width, float height)
{
    // 1. init screen size
    Rapid2D_setFrameSize(width, height);
    printf("Rapid2D_setFrameSize Ok!");
    // 2. set ResolutionModeSize, size equal to BG picture size
    Rapid2D_setResolutionModeSize(1, 320, 480);
    printf("Rapid2D_setResolutionModeSize Ok!");
    // 3. start main loop
    Rapid2D_start(0, 0);// from source files..
    printf("Rapid2D_start Ok!");
    //Rapid2D_start("game.rd", "11");//from luaChunks
}
