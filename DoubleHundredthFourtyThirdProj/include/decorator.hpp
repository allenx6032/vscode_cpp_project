/**
 * @Author: Amélie Heinrich
 * @Create Time: 2024-03-24 15:46:56
 */

#pragma once

#include "generator.hpp"

#include <rng.hpp>

class DungeonDecorator
{
public:
    // Places an exit, shops, and potential items
    static void DecorateDungeon(Map& map);
};
