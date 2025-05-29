#ifndef WORLD_H
#define WORLD_H

#include <SFML_Graphics.hpp>

#include <iostream>
#include <math.h>

#include "war_transform.h"
#include "war_chunk.h"
#include "war_content.h"

extern std::unordered_map<long long, Chunk> world;

void InitWorld();
void AddObject(int x, int y, Object & obj);
void UpdateWorld(const float & dt);
void DrawWorld(Transform * camera);

#endif // WORLD_H
