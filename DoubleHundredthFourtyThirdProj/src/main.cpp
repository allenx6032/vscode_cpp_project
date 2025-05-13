/**
 * @Author: Amélie Heinrich
 * @Create Time: 2024-03-23 17:13:28
 */

#include "decorator.hpp"

#include <iostream>

void print_usage(void)
{
    std::cout << "USAGE: ./dungeon_gen w h" << std::endl;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        print_usage();
        return 0;
    }

    Map map;
    map.width = std::atoi(argv[1]);
    map.height = std::atoi(argv[2]);

    std::cout << "Generating map of size (" << map.width << ", " << map.height << ")" << std::endl;

    Scope<DungeonGenerator> generator = DungeonGenerator::CreateGenerator(GeneratorType::Tunneling);
    generator->GenerateMap(map);
    DungeonDecorator::DecorateDungeon(map);

    for (int x = 0; x < map.width; x++) {
        for (int y = 0; y < map.height; y++) {
            if (map.map[x][y] == Tile::Walkable) {
                std::cout << ".";
            } else if (map.map[x][y] == Tile::Solid) {
                std::cout << "#";
            } else if (map.map[x][y] == Tile::Stairs) {
                std::cout << "!";
            }
        }
        std::cout << "\n";
    }
}
