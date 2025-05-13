/**
 * @Author: Amélie Heinrich
 * @Create Time: 2024-03-23 17:36:17
 */

#include "tunneling.hpp"

#include <cstdlib>

void TunnelingGenerator::GenerateMap(Map& map)
{
    RNG rng;

    map.map = new Tile*[map.width];
    for (int i = 0; i < map.width; i++) {
        map.map[i] = new Tile[map.height];
    }

    for (int i = 0; i < map.width; i++) {
        for (int j = 0; j < map.height; j++) {
            map.map[i][j] = Tile::Solid;
        }
    }

    int numRooms = 0;

    for (int r = 0; r < _maxRooms; r++) {
        int w = RNG::pRNG.GetIntInRange(_roomMinSize, _roomMaxSize);
        int h = RNG::pRNG.GetIntInRange(_roomMinSize, _roomMaxSize);
        int x = RNG::pRNG.GetIntInRange(1, map.width - w - 1);
        int y = RNG::pRNG.GetIntInRange(1, map.height - h - 1);

        Room room = Room(x, y, w, h);
        bool failed = false;
        for (Room& otherRoom : map.rooms) {
            if (room.Intersects(otherRoom)) {
                failed = true;
                break;
            }
        }

        if (!failed) {
            CreateRoom(map, room);
            auto newCenter = room.Center();
            if (numRooms != 0) {
                auto prev = map.rooms[numRooms - 1].Center();

                if (rng.GetIntInRange(0, 1) == 1) {
                    CreateHorTunnel(map, prev.first, newCenter.first, prev.second);
                    CreateVirTunnel(map, prev.second, newCenter.second, newCenter.first);
                } else {
                    CreateVirTunnel(map, prev.second, newCenter.second, prev.first);
                    CreateHorTunnel(map, prev.first, newCenter.first, newCenter.second);
                }
            }

            map.rooms.push_back(room);
            numRooms += 1;
        }
    }
}

void TunnelingGenerator::CreateRoom(Map& map, Room& room)
{
    for (int x = room.x1; x < room.x2; x++) {
        for (int y = room.y1; y < room.y2; y++) {
            map.map[x][y] = Tile::Walkable;
        }
    }
}

void TunnelingGenerator::CreateHorTunnel(Map& map, int x1, int x2, int y)
{
    for (int x = std::min(x1, x2); x < std::max(x1, x2) + 1; x++) {
        map.map[x][y] = Tile::Walkable;
    }
}

void TunnelingGenerator::CreateVirTunnel(Map& map, int y1, int y2, int x)
{
    for (int y = std::min(y1, y2); y < std::max(y1, y2) + 1; y++) {
        map.map[x][y] = Tile::Walkable;
    }
}
