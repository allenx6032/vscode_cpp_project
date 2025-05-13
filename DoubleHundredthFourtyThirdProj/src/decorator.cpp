/**
 * @Author: Amélie Heinrich
 * @Create Time: 2024-03-24 15:50:39
 */

#include "decorator.hpp"

#include <iostream>

void DungeonDecorator::DecorateDungeon(Map& map)
{
    // Generate exit
    int roomIndex = RNG::pRNG.GetIntInRange(0, map.rooms.size() - 1);
    Room& room = map.rooms[roomIndex];
    bool failed = true;
    while (failed) {
        int exitX = RNG::pRNG.GetIntInRange(room.x1 + 1, room.x2 - 1);
        int exitY = RNG::pRNG.GetIntInRange(room.y1 + 1, room.y2 - 1);
        if ((map.map[exitX + 1][exitY] != Tile::Solid && map.map[exitX - 1][exitY] != Tile::Solid) &&
            (map.map[exitX][exitY + 1] != Tile::Solid && map.map[exitX][exitY - 1] != Tile::Solid)) {
            failed = false;
            map.map[exitX][exitY] = Tile::Stairs;
        }
    }
}
