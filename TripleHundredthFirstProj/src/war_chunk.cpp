#include "war_chunk.h"
#include "war_world.h"
#include <iostream>
using namespace std;

Chunk::~Chunk()
{

}

Chunk::Chunk()
{
    generated = false;
}

Chunk::Chunk(int x, int y)
{
    position.x = x;
    position.y = y;
    perlinVector.x = rand() % (PERLIN_VECTOR_MAX + 1) - PERLIN_VECTOR_MAX / 2;
    perlinVector.y = rand() % (PERLIN_VECTOR_MAX + 1) - PERLIN_VECTOR_MAX / 2;
    generated = false;
}

long long coordsToKey(int x, int y)
{
    return (((long long)x) << 32) + (long long)y;
}

void Chunk::Draw()
{
    mesh.Draw();
}

void Chunk::Generate()
{
    long long keyRight = coordsToKey(position.x + 1, position.y);
    long long keyBottom = coordsToKey(position.x, position.y + 1);
    long long keyBottomRight = coordsToKey(position.x + 1, position.y + 1);

    sf::Vector2i perlinVectorRight, perlinVectorBottom, perlinVectorBottomRight;

    if (world.find(keyRight) != world.end())
        perlinVectorRight = world[keyRight].perlinVector;
    else
    {
        world.emplace(std::piecewise_construct,
            std::forward_as_tuple(keyRight),
            std::forward_as_tuple(position.x + 1, position.y));
        perlinVectorRight = world[keyRight].perlinVector;
    }

    if (world.find(keyBottom) != world.end())
        perlinVectorBottom = world[keyBottom].perlinVector;
    else
    {
        world.emplace(std::piecewise_construct,
            std::forward_as_tuple(keyBottom),
            std::forward_as_tuple(position.x, position.y + 1));
        perlinVectorBottom = world[keyBottom].perlinVector;
    }

    if (world.find(keyBottomRight) != world.end())
        perlinVectorBottomRight = world[keyBottomRight].perlinVector;
    else
    {
        world.emplace(std::piecewise_construct,
            std::forward_as_tuple(keyBottomRight),
            std::forward_as_tuple(position.x + 1, position.y + 1));
        perlinVectorBottomRight = world[keyBottomRight].perlinVector;
    }

    for (int y = 0; y < CHUNK_SIZE; ++y)
        for (int x = 0; x < CHUNK_SIZE; ++x)
        {
            int termLeftTop, termLeftBot, termRightBot, termRightTop;
            termLeftTop = perlinVector.x * x + perlinVector.y * y;
            termLeftBot = perlinVectorBottom.x * x + perlinVectorBottom.y * (y - CHUNK_SIZE + 1);
            termRightBot = perlinVectorBottomRight.x * (x - CHUNK_SIZE + 1) + perlinVectorBottomRight.y * (y - CHUNK_SIZE + 1);
            termRightTop = perlinVectorRight.x * (x - CHUNK_SIZE + 1) + perlinVectorRight.y * y;

            float tx = float(x) / (CHUNK_SIZE - 1);
            float ty = float(y) / (CHUNK_SIZE - 1);
            float xTop = termLeftTop + tx * (termRightTop - termLeftTop);
            float xBot = termLeftBot + tx * (termRightBot - termLeftBot);
            float res = xTop + ty * (xBot - xTop);
            res = std::min(std::max(0.0f, res / 2), 8.0f);
            height[x][y] = res * 3;
            if (res >= 1) tiles[x][y].type = "water";
            if (res >= 2)
            {
                tiles[x][y].type = "sand";
                if (rand() % 20 == 0) objects.push_back(Object(position.x * CHUNK_SIZE + x, position.y * CHUNK_SIZE + y, 0, "palm1"));
            }
            if (res >= 3) tiles[x][y].type = "wet_grass";
            if (res >= 4) tiles[x][y].type = "dry_grass";
            if (res >= 5) tiles[x][y].type = "sand";
            if (res >= 6) tiles[x][y].type = "grass";
            if (res >= 7)
            {
                tiles[x][y].type = "cold_grass";
                if (rand() % 5 == 0) objects.push_back(Object(position.x * CHUNK_SIZE + x, position.y * CHUNK_SIZE + y, 0, "tree1"));
            }
            if (res >= 8) tiles[x][y].type = "ice_grass";
            if (res >= 9) tiles[x][y].type = "snow";
        }

    generated = true;

}

float Chunk::SGetHeight(int x, int z)
{
    if (x < 0) {
        if (z < 0) {
            long long key = coordsToKey(position.x - 1, position.y - 1);
            if (world.find(key) != world.end()) {
                return world[key].height[x % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + x % CHUNK_SIZE][z % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + z % CHUNK_SIZE];
            }
            else {
                return height[0][0];
            }
        }
        else if (z >= CHUNK_SIZE) {
            long long key = coordsToKey(position.x - 1, position.y + 1);
            if (world.find(key) != world.end()) {
                return world[key].height[x % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + x % CHUNK_SIZE][z % CHUNK_SIZE];
            }
            else {
                return height[0][CHUNK_SIZE - 1];
            }
        }
        else {
            long long key = coordsToKey(position.x - 1, position.y);
            if (world.find(key) != world.end()) {
                return world[key].height[x % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + x % CHUNK_SIZE][z];
            }
            else {
                return height[0][z];
            }
        }
    }
    else if (x >= CHUNK_SIZE) {
        if (z < 0) {
            long long key = coordsToKey(position.x + 1, position.y - 1);
            if (world.find(key) != world.end()) {
                return world[key].height[x % CHUNK_SIZE][z % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + z % CHUNK_SIZE];
            }
            else {
                return height[CHUNK_SIZE - 1][0];
            }
        }
        else if (z >= CHUNK_SIZE) {
            long long key = coordsToKey(position.x + 1, position.y + 1);
            if (world.find(key) != world.end()) {
                return world[key].height[x % CHUNK_SIZE][z % CHUNK_SIZE];
            }
            else {
                return height[CHUNK_SIZE - 1][CHUNK_SIZE - 1];
            }
        }
        else {
            long long key = coordsToKey(position.x + 1, position.y);
            if (world.find(key) != world.end()) {
                return world[key].height[x % CHUNK_SIZE][z];
            }
            else {
                return height[CHUNK_SIZE - 1][z];
            }
        }
    }
    else {
        if (z < 0) {
            long long key = coordsToKey(position.x, position.y - 1);
            if (world.find(key) != world.end()) {
                return world[key].height[x][z % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + z % CHUNK_SIZE];
            }
            else {
                return height[x][0];
            }
        }
        else if (z >= CHUNK_SIZE) {
            long long key = coordsToKey(position.x, position.y + 1);
            if (world.find(key) != world.end()) {
                return world[key].height[x][z % CHUNK_SIZE];
            }
            else {
                return height[x][CHUNK_SIZE - 1];
            }
        }
        else {
            return height[x][z];
        }
    }
}

////
////
////
////
////
////

float Chunk::GetHeight(int x, int z)
{
    if (x < 0) {
        if (z < 0) {
            long long key = coordsToKey(position.x - 1 - abs((x + 1) / CHUNK_SIZE), position.y - 1 - abs((z + 1) / CHUNK_SIZE));
            //cout << (x % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + x % CHUNK_SIZE) << ";" << (z % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + z % CHUNK_SIZE);
            if (world.find(key) != world.end()) {
                return world[key].height[x % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + x % CHUNK_SIZE][z % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + z % CHUNK_SIZE];
            }
            else {
                return height[0][0];
            }
        }
        else if (z >= CHUNK_SIZE) {
            long long key = coordsToKey(position.x - 1 - abs((x + 1) / CHUNK_SIZE), position.y + z / CHUNK_SIZE);
            //cout << (x % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + x % CHUNK_SIZE) << ";" << (z % CHUNK_SIZE);
            if (world.find(key) != world.end()) {
                return world[key].height[x % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + x % CHUNK_SIZE][z % CHUNK_SIZE];
            }
            else {
                return height[0][CHUNK_SIZE - 1];
            }
        }
        else {
            long long key = coordsToKey(position.x - 1 - abs((x + 1) / CHUNK_SIZE), position.y);
            //cout << (x % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + x % CHUNK_SIZE) << ";" << (z);
            if (world.find(key) != world.end()) {
                return world[key].height[x % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + x % CHUNK_SIZE][z];
            }
            else {
                return height[0][z];
            }
        }
    }
    else if (x >= CHUNK_SIZE) {
        if (z < 0) {
            long long key = coordsToKey(position.x + x / CHUNK_SIZE, position.y - 1 - abs((z + 1) / CHUNK_SIZE));
            //cout << (x % CHUNK_SIZE) << ";" << (z % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + z % CHUNK_SIZE);
            if (world.find(key) != world.end()) {
                return world[key].height[x % CHUNK_SIZE][z % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + z % CHUNK_SIZE];
            }
            else {
                return height[CHUNK_SIZE - 1][0];
            }
        }
        else if (z >= CHUNK_SIZE) {
            long long key = coordsToKey(position.x + x / CHUNK_SIZE, position.y + z / CHUNK_SIZE);
            //cout << (x % CHUNK_SIZE) << ";" << (z % CHUNK_SIZE);
            if (world.find(key) != world.end()) {
                return world[key].height[x % CHUNK_SIZE][z % CHUNK_SIZE];
            }
            else {
                return height[CHUNK_SIZE - 1][CHUNK_SIZE - 1];
            }
        }
        else {
            long long key = coordsToKey(position.x + x / CHUNK_SIZE, position.y);
            //cout << (x % CHUNK_SIZE) << ";" << (z);
            if (world.find(key) != world.end()) {
                return world[key].height[x % CHUNK_SIZE][z];
            }
            else {
                return height[CHUNK_SIZE - 1][z];
            }
        }
    }
    else {
        if (z < 0) {
            long long key = coordsToKey(position.x, position.y - 1 - abs((z + 1) / CHUNK_SIZE));
            //cout << (x) << ";" << (z % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + z % CHUNK_SIZE);
            if (world.find(key) != world.end()) {
                return world[key].height[x][z % CHUNK_SIZE == 0 ? 0 : CHUNK_SIZE + z % CHUNK_SIZE];
            }
            else {
                return height[x][0];
            }
        }
        else if (z >= CHUNK_SIZE) {
            long long key = coordsToKey(position.x, position.y + z / CHUNK_SIZE);
            //cout << (x) << ";" << (z % CHUNK_SIZE);
            if (world.find(key) != world.end()) {
                return world[key].height[x][z % CHUNK_SIZE];
            }
            else {
                return height[x][CHUNK_SIZE - 1];
            }
        }
        else {
            //cout << (x) << ";" << (z);
            return height[x][z];
        }
    }
}

void Chunk::Recalculate()
{
    mesh.Clear();

    std::vector<glm::vec3> vertexes;
    std::vector<glm::vec3> colors;
    unsigned int x;
    for (x = 0; x < CHUNK_SIZE; ++x) {
        unsigned int z;
        //	  o o o o .
        //	   . . . .
        //	  o o o o .
        //	   . . . .
        //	z o o o o .
        //	^  . . . .
        //	| o o o o .
        //	|  . . . .
        //	| o o o o .
        //	  -----> x
        for (z = 0; z <= CHUNK_SIZE; ++z) {
            float aver_height = (GetHeight(x-1,z-1) + GetHeight(x-1,z) + GetHeight(x,z-1) + GetHeight(x,z)) / 4.0f;
            vertexes.push_back({x, aver_height, z}); colors.push_back({aver_height/6, 0,(1.0-aver_height/6)});
        }
        //	  . . . . .
        //	   o o o o
        //	  . . . . .
        //	   o o o o
        //	z . . . . .
        //	^  o o o o
        //	| . . . . .
        //	|  o o o o
        //	| . . . . .
        //	  -----> x
        for (z = 0; z < CHUNK_SIZE; ++z) {
            vertexes.push_back({x + 0.5, height[x][z], z + 0.5}); colors.push_back({height[x][z]/6, 0.5,(1.0-height[x][z]/6)});
        }
    }
    //	  . . . . o
    //	   . . . .
    //	  . . . . o
    //	   . . . .
    //	z . . . . o
    //	^  . . . .
    //	| . . . . o
    //	|  . . . .
    //	| . . . . o
    //	  -----> x
    for (unsigned int z = 0; z <= CHUNK_SIZE; ++z) {
        float aver_height = (GetHeight(CHUNK_SIZE-1,z-1) + GetHeight(CHUNK_SIZE-1,z) + GetHeight(CHUNK_SIZE,z-1) + GetHeight(CHUNK_SIZE,z)) / 4.0f;
        vertexes.push_back({x, aver_height, z}); colors.push_back({aver_height/6, 0,(1.0-aver_height/6)});
    }
    mesh.AddVBO(vertexes);
    mesh.AddVBO(colors);

    std::vector<unsigned> indices;
    for (unsigned int x = 0; x < CHUNK_SIZE; ++x){
        for (unsigned int z = 0; z < CHUNK_SIZE; ++z){
            unsigned i[5] = {x * (CHUNK_SIZE * 2 + 1) + z, x * (CHUNK_SIZE * 2 + 1) + z + 1, x * (CHUNK_SIZE * 2 + 1) + z + CHUNK_SIZE + 1, (x + 1) * (CHUNK_SIZE * 2 + 1) + z, (x + 1) * (CHUNK_SIZE * 2 + 1) + z + 1};
            indices.push_back(i[0]); indices.push_back(i[2]); indices.push_back(i[1]);
            indices.push_back(i[1]); indices.push_back(i[2]); indices.push_back(i[4]);
            indices.push_back(i[4]); indices.push_back(i[2]); indices.push_back(i[3]);
            indices.push_back(i[3]); indices.push_back(i[2]); indices.push_back(i[0]);
        }
    }
    mesh.AddEBO(indices);

    sf::Uint8 pixels[CHUNK_SIZE*CHUNK_SIZE*4];
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            pixels[CHUNK_SIZE*z*4 + x*4]     = height[x][z] > 5 ? (int)height[x][z] * 10 : 0; //tilelib[tiles[x][z].type].texture % 32;
            pixels[CHUNK_SIZE*z*4 + x*4 + 1] = (int)height[x][z] * 5; //tilelib[tiles[x][z].type].texture / 32;
            pixels[CHUNK_SIZE*z*4 + x*4 + 2] = 0;
            pixels[CHUNK_SIZE*z*4 + x*4 + 3] = 255;
        }
    }
    textures.create(CHUNK_SIZE, CHUNK_SIZE);
    textures.update(pixels);
}
