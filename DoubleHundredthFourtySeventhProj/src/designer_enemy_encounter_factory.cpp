#ifndef ENEMY_ENCOUNTER_FACTORY_CPP
#define ENEMY_ENCOUNTER_FACTORY_CPP

#include "designer_enemy_encounter_factory.hpp"
#include "designer_encounter.hpp"
#include "designer_enemy_encounter.hpp"

EnemyEncounterFactory::EnemyEncounterFactory() {}

EnemyEncounterFactory::~EnemyEncounterFactory() {}

Encounter* EnemyEncounterFactory::genEncounter() {
    return new EnemyEncounter();
}

#endif // ENEMY_ENCOUNTER_FACTORY_CPP