#include "GE_HelperFunctions.h"

#include <random>

#include "GE_Game.h"

glm::vec2 Helpers::CalculateRandomPosition()
{
    glm::vec2 firstAvailableSpawnPosition{ Game::m_windowWidth * .1f, Game::m_windowHeight * .1f };
    glm::vec2 spawnArea{ Game::m_windowWidth * .8f ,Game::m_windowHeight * .8f };

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> positionXRange(firstAvailableSpawnPosition.x, firstAvailableSpawnPosition.x + spawnArea.x);
    std::uniform_int_distribution<> positionYRange(firstAvailableSpawnPosition.y, firstAvailableSpawnPosition.y + spawnArea.y);

    return glm::vec2(
        positionXRange(gen),
        positionYRange(gen)
    );
}
