/**
 * @Author: Amélie Heinrich
 * @Create Time: 2024-03-23 17:28:25
 */

#pragma once

#include <string>
#include <memory>
#include <vector>

#include "pointer.hpp"

enum Tile : int
{
    Walkable = 0,
    Solid = 1,
    Shop = 2,
    Stairs = 3
};

struct Room
{
    int x1;
    int y1;
    int x2;
    int y2;

    Room(int xpos, int ypos, int width, int height)
    {
        x1 = xpos;
        y1 = ypos;
        x2 = x1 + width;
        y2 = y1 + height;
    }

    std::pair<float, float> Center() {
        float centerX = (x1 + x2) / 2.0f;
        float centerY = (y1 + y2) / 2.0f;
        return { centerX, centerY };
    }

    bool Intersects(const Room& other) {
        return (x1 <= other.x2 && x2 >= other.x1 && y1 <= other.y2 && y2 >= other.y1);
    }
};

struct Map
{
    bool generated = false;
    int width;
    int height;
    Tile **map;

    std::vector<Room> rooms; // Used later on by the dungeon decorator

    ~Map() {
        if (generated) {
            for (int i = 0; i < height; i++) {
                delete map[i];
            }
            delete[] map;
        }
    }
};

enum class GeneratorType
{
    None,
    Tunneling, // SUPPORTED
    BSPTree, // UNSUPPORTED
    Nystrom, // UNSUPPORTED
};

using MapPosition = std::pair<int, int>;

class DungeonGenerator
{
public:
    virtual void GenerateMap(Map& map) = 0;

    virtual const std::string& GetName() const = 0;
    virtual GeneratorType GetType() const {
        return GeneratorType::None;
    }

    static Scope<DungeonGenerator> CreateGenerator(GeneratorType type);
};
