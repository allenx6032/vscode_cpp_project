#ifndef CHUNK_H_INCLUDED
#define CHUNK_H_INCLUDED

#include <algorithm>
#include <vector>
#include <unordered_map>
#include "war_tile.h"
#include "war_object.h"
#include "war_unit.h"
#include "war_vao.h"

#define CHUNK_SIZE 32
#define PERLIN_VECTOR_MAX 2

class Chunk
{
    public:
    sf::Vector2i position;
    //glm::vec2 perlinVector;
    sf::Vector2i perlinVector;
    bool generated;
    Tile tiles[CHUNK_SIZE][CHUNK_SIZE];
    sf::Texture textures;
    float height[CHUNK_SIZE][CHUNK_SIZE];
    std::vector<Object> objects;
    std::vector<Unit> units;

    VAO mesh;

    Chunk();
    ~Chunk();
    Chunk(int x, int y);

    float GetHeight(int x, int z);
    float SGetHeight(int x, int z);

    void Update(const float & dt);
    void Draw();
    void Generate();
    void Recalculate();
};

long long coordsToKey(int x, int y);

#endif // CHUNK_H_INCLUDED
