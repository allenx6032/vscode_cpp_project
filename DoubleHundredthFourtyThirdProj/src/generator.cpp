/**
 * @Author: Amélie Heinrich
 * @Create Time: 2024-03-23 18:02:00
 */

#include "generator.hpp"

#include "tunneling.hpp"

Scope<DungeonGenerator> DungeonGenerator::CreateGenerator(GeneratorType type)
{
    switch (type) {
        case GeneratorType::Tunneling: {
            return CreateScope<TunnelingGenerator>();
        }
        default: {
            return nullptr;
        }
    }
    return nullptr;
}
